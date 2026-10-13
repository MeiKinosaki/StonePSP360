#include "../core/psp/allegrex_cpu.h"

#include <assert.h>
#include <stdio.h>

#define R(rs, rt, rd, sh, fn) (((uint32_t)(rs) << 21) | ((uint32_t)(rt) << 16) | ((uint32_t)(rd) << 11) | ((uint32_t)(sh) << 6) | (uint32_t)(fn))
#define I(op, rs, rt, imm) (((uint32_t)(op) << 26) | ((uint32_t)(rs) << 21) | ((uint32_t)(rt) << 16) | ((uint32_t)(imm) & UINT32_C(0xFFFF)))

static void put_instruction(PspMemory *mem, uint32_t address, uint32_t instruction)
{
    assert(psp_memory_write32(mem, address, instruction));
}

static void reset_at(AllegrexState *cpu, uint32_t address)
{
    allegrex_state_reset(cpu);
    cpu->pc = address;
    cpu->next_pc = address + 4u;
}

int main(void)
{
    PspMemory mem;
    AllegrexState cpu;
    const uint32_t base = UINT32_C(0x08000000);
    assert(psp_memory_init(&mem));
    reset_at(&cpu, base);
    cpu.gpr[5] = base;

    put_instruction(&mem, base + 0x00u, I(0x09, 0, 1, 7));
    put_instruction(&mem, base + 0x04u, I(0x09, 0, 2, 9));
    put_instruction(&mem, base + 0x08u, R(1, 2, 3, 0, 0x21));
    assert(allegrex_step(&cpu, &mem) == ALLEGREX_STEP_OK);
    assert(cpu.gpr[1] == 7u && cpu.pc == base + 4u);
    assert(allegrex_step(&cpu, &mem) == ALLEGREX_STEP_OK);
    assert(cpu.gpr[2] == 9u);
    assert(allegrex_step(&cpu, &mem) == ALLEGREX_STEP_OK);
    assert(cpu.gpr[3] == 16u);

    put_instruction(&mem, base + 0x0Cu, I(0x2B, 5, 3, 0x100));
    put_instruction(&mem, base + 0x10u, I(0x23, 5, 4, 0x100));
    assert(allegrex_step(&cpu, &mem) == ALLEGREX_STEP_OK);
    assert(allegrex_step(&cpu, &mem) == ALLEGREX_STEP_OK);
    assert(cpu.gpr[4] == 16u);

    put_instruction(&mem, base + 0x14u, I(0x09, 0, 0, 123));
    assert(allegrex_step(&cpu, &mem) == ALLEGREX_STEP_OK);
    assert(cpu.gpr[0] == 0u);
    put_instruction(&mem, base + 0x18u, UINT32_C(0xFFFFFFFF));
    assert(allegrex_step(&cpu, &mem) == ALLEGREX_STEP_ILLEGAL_INSTRUCTION);
    assert(cpu.pc == base + 0x18u);

    /* Signed multiply: -7 * 9 = -63; MFLO/MFHI expose the 64-bit result. */
    reset_at(&cpu, base + 0x40u);
    cpu.gpr[1] = UINT32_C(0xFFFFFFF9);
    cpu.gpr[2] = 9u;
    put_instruction(&mem, base + 0x40u, R(1, 2, 0, 0, 0x18));
    put_instruction(&mem, base + 0x44u, R(0, 0, 3, 0, 0x12));
    put_instruction(&mem, base + 0x48u, R(0, 0, 4, 0, 0x10));
    assert(allegrex_step(&cpu, &mem) == ALLEGREX_STEP_OK);
    assert(cpu.lo == UINT32_C(0xFFFFFFC1) && cpu.hi == UINT32_MAX);
    assert(allegrex_step(&cpu, &mem) == ALLEGREX_STEP_OK && cpu.gpr[3] == UINT32_C(0xFFFFFFC1));
    assert(allegrex_step(&cpu, &mem) == ALLEGREX_STEP_OK && cpu.gpr[4] == UINT32_MAX);

    /* Signed divide: -63 / 9 = -7 remainder 0. */
    reset_at(&cpu, base + 0x60u);
    cpu.gpr[1] = UINT32_C(0xFFFFFFC1);
    cpu.gpr[2] = 9u;
    put_instruction(&mem, base + 0x60u, R(1, 2, 0, 0, 0x1A));
    assert(allegrex_step(&cpu, &mem) == ALLEGREX_STEP_OK);
    assert(cpu.lo == UINT32_C(0xFFFFFFF9) && cpu.hi == 0u);

    /* BEQ is taken, its delay-slot instruction runs, then execution reaches target. */
    reset_at(&cpu, base + 0x100u);
    cpu.gpr[1] = 5u;
    put_instruction(&mem, base + 0x100u, I(0x04, 1, 1, 2));
    put_instruction(&mem, base + 0x104u, I(0x09, 0, 6, 42));
    put_instruction(&mem, base + 0x108u, I(0x09, 0, 7, 77));
    put_instruction(&mem, base + 0x10Cu, I(0x09, 0, 8, 88));
    assert(allegrex_step(&cpu, &mem) == ALLEGREX_STEP_OK);
    assert(cpu.pc == base + 0x104u && cpu.next_pc == base + 0x10Cu);
    assert(allegrex_step(&cpu, &mem) == ALLEGREX_STEP_OK);
    assert(cpu.gpr[6] == 42u && cpu.pc == base + 0x10Cu);
    assert(allegrex_step(&cpu, &mem) == ALLEGREX_STEP_OK);
    assert(cpu.gpr[8] == 88u && cpu.gpr[7] == 0u);

    psp_memory_destroy(&mem);
    puts("PASS: Allegrex arithmetic, memory, HI/LO multiply/divide, and branch delay slot.");
    return 0;
}
