#include "../core/psp/allegrex_cpu.h"

#include <assert.h>
#include <stdio.h>

#define R(rs, rt, rd, sh, fn) (((uint32_t)(rs) << 21) | ((uint32_t)(rt) << 16) | ((uint32_t)(rd) << 11) | ((uint32_t)(sh) << 6) | (uint32_t)(fn))
#define I(op, rs, rt, imm) (((uint32_t)(op) << 26) | ((uint32_t)(rs) << 21) | ((uint32_t)(rt) << 16) | ((uint32_t)(imm) & UINT32_C(0xFFFF)))

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
    const uint32_t base = UINT32_C(0x08001000);

    assert(psp_memory_init(&mem));

    /* Taken BEQ: the delay-slot instruction runs before the target. */
    reset(&cpu, base);
    cpu.gpr[1] = 7u;
    put(&mem, base, I(0x04, 1, 1, 2));
    put(&mem, base + 4u, I(0x09, 0, 2, 9));
    put(&mem, base + 8u, I(0x09, 0, 4, 99));
    put(&mem, base + 12u, I(0x09, 0, 3, 11));
    assert(allegrex_step(&cpu, &mem) == ALLEGREX_STEP_OK);
    assert(cpu.pc == base + 4u && cpu.next_pc == base + 12u);
    assert(allegrex_step(&cpu, &mem) == ALLEGREX_STEP_OK);
    assert(cpu.gpr[2] == 9u && cpu.gpr[4] == 0u);
    assert(cpu.pc == base + 12u);
    assert(allegrex_step(&cpu, &mem) == ALLEGREX_STEP_OK);
    assert(cpu.gpr[3] == 11u);

    /* Not-taken BEQ still executes the next instruction and falls through. */
    reset(&cpu, base + 0x40u);
    cpu.gpr[1] = 1u;
    cpu.gpr[2] = 2u;
    put(&mem, base + 0x40u, I(0x04, 1, 2, 3));
    put(&mem, base + 0x44u, I(0x09, 0, 5, 0x33));
    assert(allegrex_step(&cpu, &mem) == ALLEGREX_STEP_OK);
    assert(cpu.next_pc == base + 0x48u);
    assert(allegrex_step(&cpu, &mem) == ALLEGREX_STEP_OK);
    assert(cpu.gpr[5] == 0x33u && cpu.pc == base + 0x48u);

    /* Failed data access reports a fault and keeps the faulting PC. */
    reset(&cpu, base + 0x80u);
    cpu.gpr[2] = UINT32_C(0x0A000000);
    put(&mem, base + 0x80u, I(0x23, 2, 1, 0));
    assert(allegrex_step(&cpu, &mem) == ALLEGREX_STEP_MEMORY_FAULT);
    assert(cpu.pc == base + 0x80u);
    assert(cpu.gpr[1] == 0u);

    /* Unsupported instruction is reported without advancing the PC. */
    reset(&cpu, base + 0xC0u);
    put(&mem, base + 0xC0u, UINT32_C(0xFFFFFFFF));
    assert(allegrex_step(&cpu, &mem) == ALLEGREX_STEP_ILLEGAL_INSTRUCTION);
    assert(cpu.pc == base + 0xC0u);

    psp_memory_destroy(&mem);
    puts("PASS: branch delay slots, fall-through, memory fault and illegal opcode.");
    return 0;
}
