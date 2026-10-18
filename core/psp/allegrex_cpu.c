#include "allegrex_cpu.h"

#include <stdbool.h>
#include <stdint.h>

static uint32_t sign_extend16(uint32_t value)
{
    return (uint32_t)(int32_t)(int16_t)(value & UINT32_C(0xFFFF));
}


static uint32_t count_leading(uint32_t value, bool ones)
{
    uint32_t count = 0u;
    uint32_t mask = UINT32_C(0x80000000);
    while (mask != 0u && (((value & mask) != 0u) == ones)) {
        ++count;
        mask >>= 1;
    }
    return count;
}

static bool load_instruction(const PspMemory *memory, uint32_t pc, uint32_t *op)
{
    return (pc & 3u) == 0u && psp_memory_read32(memory, pc, op);
}

static bool load_data(const PspMemory *memory, uint32_t address,
                      unsigned size, uint32_t *value)
{
    if (size == 1u) {
        uint8_t v8;
        if (!psp_memory_read8(memory, address, &v8)) return false;
        *value = v8;
        return true;
    }
    if (size == 2u) {
        uint16_t v16;
        if ((address & 1u) != 0u || !psp_memory_read16(memory, address, &v16)) return false;
        *value = v16;
        return true;
    }
    if (size == 4u) {
        if ((address & 3u) != 0u || !psp_memory_read32(memory, address, value)) return false;
        return true;
    }
    return false;
}

static bool store_data(PspMemory *memory, uint32_t address,
                       unsigned size, uint32_t value)
{
    if (size == 1u) return psp_memory_write8(memory, address, (uint8_t)value);
    if (size == 2u) {
        if ((address & 1u) != 0u) return false;
        return psp_memory_write16(memory, address, (uint16_t)value);
    }
    if (size == 4u) {
        if ((address & 3u) != 0u) return false;
        return psp_memory_write32(memory, address, value);
    }
    return false;
}

static void restore_pc(AllegrexState *state, uint32_t instruction_pc,
                       uint32_t old_next_pc)
{
    state->pc = instruction_pc;
    state->next_pc = old_next_pc;
}

AllegrexStepResult allegrex_step(AllegrexState *state, PspMemory *memory)
{
    uint32_t op, instruction_pc, old_next_pc, rs, rt, rd, shamt, funct, imm;
    uint32_t a, b, result = 0u, target, opcode;
    bool write_gpr = false;

    if (state == NULL || memory == NULL) return ALLEGREX_STEP_INVALID_ARGUMENT;
    instruction_pc = state->pc;
    old_next_pc = state->next_pc;
    if (!load_instruction(memory, instruction_pc, &op)) return ALLEGREX_STEP_MEMORY_FAULT;

    opcode = op >> 26;
    rs = (op >> 21) & 31u;
    rt = (op >> 16) & 31u;
    rd = (op >> 11) & 31u;
    shamt = (op >> 6) & 31u;
    funct = op & 63u;
    imm = op & UINT32_C(0xFFFF);
    a = state->gpr[rs];
    b = state->gpr[rt];

    /* Keep one pending PC: a taken branch still executes the following delay slot. */
    state->pc = old_next_pc;
    state->next_pc = old_next_pc + 4u;

    switch (opcode) {
    case 0u: /* SPECIAL */
        switch (funct) {
        case 0x00u: result = b << shamt; write_gpr = true; break;
        case 0x02u: result = b >> shamt; write_gpr = true; break;
        case 0x03u: result = (uint32_t)((int32_t)b >> shamt); write_gpr = true; break;
        case 0x04u: result = b << (a & 31u); write_gpr = true; break;
        case 0x06u: result = b >> (a & 31u); write_gpr = true; break;
        case 0x07u: result = (uint32_t)((int32_t)b >> (a & 31u)); write_gpr = true; break;
        case 0x08u: state->next_pc = a; break; /* JR */
        case 0x0Au: if (b == 0u) { result = a; write_gpr = true; } break; /* MOVZ */
        case 0x0Bu: if (b != 0u) { result = a; write_gpr = true; } break; /* MOVN */
        case 0x09u:
            if (rd != 0u) state->gpr[rd] = old_next_pc + 4u;
            state->next_pc = a;
            break;
        case 0x10u: result = state->hi; write_gpr = true; break;
        case 0x11u: state->hi = a; break;
        case 0x12u: result = state->lo; write_gpr = true; break;
        case 0x13u: state->lo = a; break;
        case 0x18u: {
            int64_t product = (int64_t)(int32_t)a * (int64_t)(int32_t)b;
            uint64_t bits = (uint64_t)product;
            state->lo = (uint32_t)bits;
            state->hi = (uint32_t)(bits >> 32);
            break;
        }
        case 0x19u: {
            uint64_t product = (uint64_t)a * (uint64_t)b;
            state->lo = (uint32_t)product;
            state->hi = (uint32_t)(product >> 32);
            break;
        }
        case 0x1Au:
            if (b != 0u) {
                int32_t dividend = (int32_t)a;
                int32_t divisor = (int32_t)b;
                if (dividend == INT32_MIN && divisor == -1) {
                    state->lo = (uint32_t)INT32_MIN;
                    state->hi = 0u;
                } else {
                    state->lo = (uint32_t)(dividend / divisor);
                    state->hi = (uint32_t)(dividend % divisor);
                }
            }
            break;
        case 0x1Bu:
            if (b != 0u) {
                state->lo = a / b;
                state->hi = a % b;
            }
            break;
        case 0x21u: result = a + b; write_gpr = true; break;
        case 0x23u: result = a - b; write_gpr = true; break;
        case 0x24u: result = a & b; write_gpr = true; break;
        case 0x25u: result = a | b; write_gpr = true; break;
        case 0x26u: result = a ^ b; write_gpr = true; break;
        case 0x27u: result = ~(a | b); write_gpr = true; break;
        case 0x2Au: result = (int32_t)a < (int32_t)b ? 1u : 0u; write_gpr = true; break;
        case 0x2Bu: result = a < b ? 1u : 0u; write_gpr = true; break;
        default:
            restore_pc(state, instruction_pc, old_next_pc);
            return ALLEGREX_STEP_ILLEGAL_INSTRUCTION;
        }
        if (write_gpr && rd != 0u) state->gpr[rd] = result;
        break;

    case 0x1Cu: /* SPECIAL2 */
        if (funct == 0x00u) {
            int64_t product = (int64_t)(int32_t)a * (int64_t)(int32_t)b;
            uint64_t acc = ((uint64_t)state->hi << 32) | state->lo;
            uint64_t sum = acc + (uint64_t)product;
            state->lo = (uint32_t)sum; state->hi = (uint32_t)(sum >> 32);
        } else if (funct == 0x01u) {
            uint64_t product = (uint64_t)a * (uint64_t)b;
            uint64_t acc = ((uint64_t)state->hi << 32) | state->lo;
            uint64_t sum = acc + product;
            state->lo = (uint32_t)sum; state->hi = (uint32_t)(sum >> 32);
        } else if (funct == 0x02u) {
            int64_t product = (int64_t)(int32_t)a * (int64_t)(int32_t)b;
            result = (uint32_t)(uint64_t)product; write_gpr = true;
        } else if (funct == 0x04u) {
            int64_t product = (int64_t)(int32_t)a * (int64_t)(int32_t)b;
            uint64_t acc = ((uint64_t)state->hi << 32) | state->lo;
            uint64_t diff = acc - (uint64_t)product;
            state->lo = (uint32_t)diff; state->hi = (uint32_t)(diff >> 32);
        } else if (funct == 0x05u) {
            uint64_t product = (uint64_t)a * (uint64_t)b;
            uint64_t acc = ((uint64_t)state->hi << 32) | state->lo;
            uint64_t diff = acc - product;
            state->lo = (uint32_t)diff; state->hi = (uint32_t)(diff >> 32);
        } else if (funct == 0x20u) {
            result = count_leading(a, false); write_gpr = true;
        } else if (funct == 0x21u) {
            result = count_leading(a, true); write_gpr = true;
        } else {
            restore_pc(state, instruction_pc, old_next_pc);
            return ALLEGREX_STEP_ILLEGAL_INSTRUCTION;
        }
        if (write_gpr && rd != 0u) state->gpr[rd] = result;
        break;

    case 0x01u:
        target = old_next_pc + (sign_extend16(imm) << 2);
        switch (rt) {
        case 0x00u: if ((int32_t)a < 0) state->next_pc = target; break;
        case 0x01u: if ((int32_t)a >= 0) state->next_pc = target; break;
        case 0x10u:
            if ((int32_t)a < 0) {
                state->gpr[31] = old_next_pc + 4u;
                state->next_pc = target;
            }
            break;
        case 0x11u:
            if ((int32_t)a >= 0) {
                state->gpr[31] = old_next_pc + 4u;
                state->next_pc = target;
            }
            break;
        default:
            restore_pc(state, instruction_pc, old_next_pc);
            return ALLEGREX_STEP_ILLEGAL_INSTRUCTION;
        }
        break;

    case 0x02u:
    case 0x03u:
        if (opcode == 0x03u) state->gpr[31] = old_next_pc + 4u;
        state->next_pc = (old_next_pc & UINT32_C(0xF0000000)) |
                         ((op & UINT32_C(0x03FFFFFF)) << 2);
        break;
    case 0x04u: if (a == b) state->next_pc = old_next_pc + (sign_extend16(imm) << 2); break;
    case 0x05u: if (a != b) state->next_pc = old_next_pc + (sign_extend16(imm) << 2); break;
    case 0x06u: if ((int32_t)a <= 0) state->next_pc = old_next_pc + (sign_extend16(imm) << 2); break;
    case 0x07u: if ((int32_t)a > 0) state->next_pc = old_next_pc + (sign_extend16(imm) << 2); break;
    case 0x08u: /* ADDI overflow exceptions are not modeled yet */
    case 0x09u: result = a + sign_extend16(imm); write_gpr = true; break;
    case 0x0Au: result = (int32_t)a < (int32_t)sign_extend16(imm) ? 1u : 0u; write_gpr = true; break;
    case 0x0Bu: result = a < sign_extend16(imm) ? 1u : 0u; write_gpr = true; break;
    case 0x0Cu: result = a & imm; write_gpr = true; break;
    case 0x0Du: result = a | imm; write_gpr = true; break;
    case 0x0Eu: result = a ^ imm; write_gpr = true; break;
    case 0x0Fu: result = imm << 16; write_gpr = true; break;
    case 0x20u:
    case 0x24u:
    case 0x21u:
    case 0x25u:
    case 0x23u:
        target = a + sign_extend16(imm);
        if (!load_data(memory, target,
                       (opcode == 0x20u || opcode == 0x24u) ? 1u :
                       (opcode == 0x21u || opcode == 0x25u) ? 2u : 4u, &result)) {
            restore_pc(state, instruction_pc, old_next_pc);
            return ALLEGREX_STEP_MEMORY_FAULT;
        }
        if (opcode == 0x20u) result = (uint32_t)(int32_t)(int8_t)result;
        if (opcode == 0x21u) result = (uint32_t)(int32_t)(int16_t)result;
        write_gpr = true;
        break;
    case 0x28u:
    case 0x29u:
    case 0x2Bu:
        target = a + sign_extend16(imm);
        if (!store_data(memory, target,
                        opcode == 0x28u ? 1u : opcode == 0x29u ? 2u : 4u, b)) {
            restore_pc(state, instruction_pc, old_next_pc);
            return ALLEGREX_STEP_MEMORY_FAULT;
        }
        break;
    default:
        restore_pc(state, instruction_pc, old_next_pc);
        return ALLEGREX_STEP_ILLEGAL_INSTRUCTION;
    }

    if (write_gpr && rt != 0u && opcode != 0u) state->gpr[rt] = result;
    state->gpr[0] = 0u;
    return ALLEGREX_STEP_OK;
}

AllegrexStepResult allegrex_run(AllegrexState *state, PspMemory *memory,
                                uint32_t max_steps, uint32_t *executed_steps)
{
    uint32_t count = 0u;
    AllegrexStepResult result = ALLEGREX_STEP_OK;

    if (executed_steps != NULL) *executed_steps = 0u;
    if (state == NULL || memory == NULL) return ALLEGREX_STEP_INVALID_ARGUMENT;

    while (count < max_steps) {
        result = allegrex_step(state, memory);
        if (result != ALLEGREX_STEP_OK) {
            if (executed_steps != NULL) *executed_steps = count;
            return result;
        }
        ++count;
    }

    if (executed_steps != NULL) *executed_steps = count;
    return ALLEGREX_STEP_OK;
}
