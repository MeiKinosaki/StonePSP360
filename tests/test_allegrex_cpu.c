#include "../core/psp/allegrex_cpu.h"

#include <assert.h>
#include <stdio.h>

#define R(rs, rt, rd, sh, fn) (((uint32_t)(rs) << 21) | ((uint32_t)(rt) << 16) | ((uint32_t)(rd) << 11) | ((uint32_t)(sh) << 6) | (uint32_t)(fn))
#define I(op, rs, rt, imm) (((uint32_t)(op) << 26) | ((uint32_t)(rs) << 21) | ((uint32_t)(rt) << 16) | ((uint32_t)(imm) & UINT32_C(0xFFFF)))

static void put_instruction(PspMemory *mem, uint32_t address, uint32_t instruction)
{
    assert(psp_memory_write32(mem, address, instruction));
}

int main(void)
{
    PspMemory mem;
    AllegrexState cpu;
    assert(psp_memory_init(&mem));
    allegrex_state_reset(&cpu);
    cpu.pc = UINT32_C(0x08000000);
    cpu.next_pc = UINT32_C(0x08000004);

    /* addiu r1, r0, 7; addiu r2, r0, 9; addu r3, r1, r2 */
    put_instruction(&mem, UINT32_C(0x08000000), I(0x09, 0, 1, 7));
    put_instruction(&mem, UINT32_C(0x08000004), I(0x09, 0, 2, 9));
    put_instruction(&mem, UINT32_C(0x08000008), R(1, 2, 3, 0, 0x21));
    assert(allegrex_step(&cpu, &mem) == ALLEGREX_STEP_OK);
    assert(cpu.gpr[1] == 7u && cpu.pc == UINT32_C(0x08000004));
    assert(allegrex_step(&cpu, &mem) == ALLEGREX_STEP_OK);
    assert(cpu.gpr[2] == 9u);
    assert(allegrex_step(&cpu, &mem) == ALLEGREX_STEP_OK);
    assert(cpu.gpr[3] == 16u);

    /* sw r3, 0x100(r0); lw r4, 0x100(r0) */
    put_instruction(&mem, UINT32_C(0x0800000C), I(0x2B, 0, 3, 0x100));
    put_instruction(&mem, UINT32_C(0x08000010), I(0x23, 0, 4, 0x100));
    assert(allegrex_step(&cpu, &mem) == ALLEGREX_STEP_OK);
    assert(allegrex_step(&cpu, &mem) == ALLEGREX_STEP_OK);
    assert(cpu.gpr[4] == 16u);

    /* r0 is immutable, even when an instruction names it as destination. */
    put_instruction(&mem, UINT32_C(0x08000014), I(0x09, 0, 0, 123));
    assert(allegrex_step(&cpu, &mem) == ALLEGREX_STEP_OK);
    assert(cpu.gpr[0] == 0u);

    /* Unsupported opcodes are rejected without advancing architectural PC. */
    put_instruction(&mem, UINT32_C(0x08000018), UINT32_C(0xFFFFFFFF));
    assert(allegrex_step(&cpu, &mem) == ALLEGREX_STEP_ILLEGAL_INSTRUCTION);
    assert(cpu.pc == UINT32_C(0x08000018));

    psp_memory_destroy(&mem);
    puts("PASS: Allegrex subset executes arithmetic, loads/stores, and protects r0.");
    return 0;
}
