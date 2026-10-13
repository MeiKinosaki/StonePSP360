#include "allegrex_cpu.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

static uint32_t sign_extend16(uint32_t value)
{
    return (uint32_t)(int32_t)(int16_t)(value & UINT32_C(0xFFFF));
}

static bool load_instruction(const PspMemory *memory, uint32_t pc, uint32_t *op)
{
    if ((pc & 3u) != 0u) {
        return false;
    }
    return psp_memory_read32(memory, pc, op);
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

AllegrexStepResult allegrex_step(AllegrexState *state, PspMemory *memory)
{
    uint32_t op, instruction_pc, old_next_pc, rs, rt, rd, shamt, funct, imm;
    uint32_t a, b, result, target;
    uint32_t opcode;

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

    /* Advance first; control-flow instructions replace next_pc, preserving delay slots. */
    state->pc = old_next_pc;
    state->next_pc = old_next_pc + 4u;
    result = 0u;

    switch (opcode) {
    case 0u: /* SPECIAL */
        switch (funct) {
        case 0x00u: result = b << shamt; break;                 /* SLL / NOP */
        case 0x02u: result = b >> shamt; break;                 /* SRL */
        case 0x03u: result = (uint32_t)((int32_t)b >> shamt); break; /* SRA */
        case 0x04u: result = b << (a & 31u); break;             /* SLLV */
        case 0x06u: result = b >> (a & 31u); break;             /* SRLV */
        case 0x07u: result = (uint32_t)((int32_t)b >> (a & 31u)); break; /* SRAV */
        case 0x08u: state->next_pc = a; state->gpr[0] = 0u; return ALLEGREX_STEP_OK; /* JR */
        case 0x09u: /* JALR */
            if (rd != 0u) state->gpr[rd] = old_next_pc + 4u;
            state->next_pc = a;
            state->gpr[0] = 0u;
            return ALLEGREX_STEP_OK;
        case 0x21u: result = a + b; break;                      /* ADDU */
        case 0x23u: result = a - b; break;                      /* SUBU */
        case 0x24u: result = a & b; break;
        case 0x25u: result = a | b; break;
        case 0x26u: result = a ^ b; break;
        case 0x27u: result = ~(a | b); break;
        case 0x2Au: result = (int32_t)a < (int32_t)b ? 1u : 0u; break;
        case 0x2Bu: result = a < b ? 1u : 0u; break;
        default: state->pc = instruction_pc; state->next_pc = old_next_pc; return ALLEGREX_STEP_ILLEGAL_INSTRUCTION;
        }
        if (rd != 0u) state->gpr[rd] = result;
        break;
    case 0x02u: /* J */
    case 0x03u: /* JAL */
        if (opcode == 0x03u) state->gpr[31] = old_next_pc + 4u;
        state->next_pc = (old_next_pc & UINT32_C(0xF0000000)) |
                         ((op & UINT32_C(0x03FFFFFF)) << 2);
        break;
    case 0x04u: if (a == b) state->next_pc = old_next_pc + (sign_extend16(imm) << 2); break;
    case 0x05u: if (a != b) state->next_pc = old_next_pc + (sign_extend16(imm) << 2); break;
    case 0x06u: if ((int32_t)a <= 0) state->next_pc = old_next_pc + (sign_extend16(imm) << 2); break;
    case 0x07u: if ((int32_t)a > 0) state->next_pc = old_next_pc + (sign_extend16(imm) << 2); break;
    case 0x08u: /* ADDI overflow exception intentionally not modeled yet */
    case 0x09u: result = a + sign_extend16(imm); if (rt != 0u) state->gpr[rt] = result; break;
    case 0x0Au: if (rt != 0u) state->gpr[rt] = (int32_t)a < (int32_t)sign_extend16(imm) ? 1u : 0u; break;
    case 0x0Bu: if (rt != 0u) state->gpr[rt] = a < sign_extend16(imm) ? 1u : 0u; break;
    case 0x0Cu: if (rt != 0u) state->gpr[rt] = a & imm; break;
    case 0x0Du: if (rt != 0u) state->gpr[rt] = a | imm; break;
    case 0x0Eu: if (rt != 0u) state->gpr[rt] = a ^ imm; break;
    case 0x0Fu: if (rt != 0u) state->gpr[rt] = imm << 16; break;
    case 0x20u: /* LB */
    case 0x24u: /* LBU */
    case 0x21u: /* LH */
    case 0x25u: /* LHU */
    case 0x23u: /* LW */
        target = a + sign_extend16(imm);
        if (!load_data(memory, target,
                       (opcode == 0x20u || opcode == 0x24u) ? 1u :
                       (opcode == 0x21u || opcode == 0x25u) ? 2u : 4u, &result)) {
            state->pc = instruction_pc; state->next_pc = old_next_pc; return ALLEGREX_STEP_MEMORY_FAULT;
        }
        if (opcode == 0x20u) result = (uint32_t)(int32_t)(int8_t)result;
        if (opcode == 0x21u) result = (uint32_t)(int32_t)(int16_t)result;
        if (rt != 0u) state->gpr[rt] = result;
        break;
    case 0x28u: /* SB */
    case 0x29u: /* SH */
    case 0x2Bu: /* SW */
        target = a + sign_extend16(imm);
        if (!store_data(memory, target,
                        opcode == 0x28u ? 1u : opcode == 0x29u ? 2u : 4u, b)) {
            state->pc = instruction_pc; state->next_pc = old_next_pc; return ALLEGREX_STEP_MEMORY_FAULT;
        }
        break;
    default:
        state->pc = instruction_pc;
        state->next_pc = old_next_pc;
        return ALLEGREX_STEP_ILLEGAL_INSTRUCTION;
    }

    state->gpr[0] = 0u;
    return ALLEGREX_STEP_OK;
}
