#include "../core/psp/allegrex_cpu.h"

#include <assert.h>
#include <stdio.h>

#define R(rs, rt, rd, sh, fn) (((uint32_t)(rs) << 21) | ((uint32_t)(rt) << 16) | ((uint32_t)(rd) << 11) | ((uint32_t)(sh) << 6) | (uint32_t)(fn))
#define SPECIAL2(rs, rt, rd, fn) ((UINT32_C(0x1C) << 26) | ((uint32_t)(rs) << 21) | ((uint32_t)(rt) << 16) | ((uint32_t)(rd) << 11) | (uint32_t)(fn))

static void put(PspMemory *mem, uint32_t address, uint32_t instruction)
{
    assert(psp_memory_write32(mem, address, instruction));
}

static void reset(AllegrexState *cpu, uint32_t pc)
{
    allegrex_state_reset(cpu);
    cpu->pc = pc;
    cpu->next_pc = pc + 4u;
}

int main(void)
{
    PspMemory mem;
    AllegrexState cpu;
    const uint32_t base = UINT32_C(0x08002000);

    assert(psp_memory_init(&mem));

    reset(&cpu, base);
    cpu.gpr[1] = UINT32_C(0x00F00000);
    cpu.gpr[2] = 0u;
    put(&mem, base, SPECIAL2(1, 0, 3, 0x20)); /* CLZ */
    put(&mem, base + 4u, SPECIAL2(1, 0, 4, 0x21)); /* CLO */
    put(&mem, base + 8u, SPECIAL2(2, 0, 5, 0x20)); /* CLZ zero */
    put(&mem, base + 12u, SPECIAL2(2, 0, 6, 0x21)); /* CLO zero */
    assert(allegrex_step(&cpu, &mem) == ALLEGREX_STEP_OK && cpu.gpr[3] == 8u);
    assert(allegrex_step(&cpu, &mem) == ALLEGREX_STEP_OK && cpu.gpr[4] == 0u);
    assert(allegrex_step(&cpu, &mem) == ALLEGREX_STEP_OK && cpu.gpr[5] == 32u);
    assert(allegrex_step(&cpu, &mem) == ALLEGREX_STEP_OK && cpu.gpr[6] == 0u);

    reset(&cpu, base + 0x40u);
    cpu.gpr[1] = UINT32_C(0x12345678);
    cpu.gpr[2] = 0u;
    cpu.gpr[3] = 1u;
    cpu.gpr[4] = UINT32_C(0xAAAAAAAA);
    cpu.gpr[5] = UINT32_C(0xBBBBBBBB);
    put(&mem, base + 0x40u, R(1, 2, 4, 0, 0x0A)); /* MOVZ: write */
    put(&mem, base + 0x44u, R(1, 3, 5, 0, 0x0B)); /* MOVN: write */
    put(&mem, base + 0x48u, R(1, 3, 4, 0, 0x0A)); /* MOVZ: preserve */
    put(&mem, base + 0x4Cu, R(1, 2, 5, 0, 0x0B)); /* MOVN: preserve */
    assert(allegrex_step(&cpu, &mem) == ALLEGREX_STEP_OK && cpu.gpr[4] == cpu.gpr[1]);
    assert(allegrex_step(&cpu, &mem) == ALLEGREX_STEP_OK && cpu.gpr[5] == cpu.gpr[1]);
    assert(allegrex_step(&cpu, &mem) == ALLEGREX_STEP_OK && cpu.gpr[4] == cpu.gpr[1]);
    assert(allegrex_step(&cpu, &mem) == ALLEGREX_STEP_OK && cpu.gpr[5] == cpu.gpr[1]);

    /* Destination register zero remains immutable for the new operations. */
    reset(&cpu, base + 0x80u);
    cpu.gpr[1] = 0u;
    put(&mem, base + 0x80u, SPECIAL2(1, 0, 0, 0x20));
    assert(allegrex_step(&cpu, &mem) == ALLEGREX_STEP_OK && cpu.gpr[0] == 0u);

    psp_memory_destroy(&mem);
    puts("PASS: Allegrex CLZ/CLO and conditional move instructions.");
    return 0;
}
