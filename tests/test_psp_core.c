#include "../core/psp/psp_core.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>

#define I(op, rs, rt, imm) (((uint32_t)(op) << 26) | ((uint32_t)(rs) << 21) | ((uint32_t)(rt) << 16) | ((uint32_t)(imm) & UINT32_C(0xFFFF)))

static void put16(uint8_t *p, uint16_t v)
{
    p[0] = (uint8_t)v; p[1] = (uint8_t)(v >> 8);
}
static void put32(uint8_t *p, uint32_t v)
{
    p[0] = (uint8_t)v; p[1] = (uint8_t)(v >> 8);
    p[2] = (uint8_t)(v >> 16); p[3] = (uint8_t)(v >> 24);
}
static void make_test_elf(uint8_t *p, size_t size)
{
    memset(p, 0, size);
    p[0] = 0x7F; p[1] = 'E'; p[2] = 'L'; p[3] = 'F';
    p[4] = 1; p[5] = 1; p[6] = 1;
    put16(p + 16, 2); put16(p + 18, 8); put32(p + 20, 1);
    put32(p + 24, 0x08804000u);
    put32(p + 28, 0x34u);
    put16(p + 40, 0x34u); put16(p + 42, 0x20u); put16(p + 44, 1u);
    put32(p + 0x34, 1u);
    put32(p + 0x38, 0x54u);
    put32(p + 0x3C, 0x08804000u);
    put32(p + 0x44, 16u);
    put32(p + 0x48, 16u);
    put32(p + 0x4C, 5u);
    put32(p + 0x50, 4u);
    /* LUI r1, 0x0880; ORI r1,r1,0x0200; ADDIU r2,r0,0x55; SW r2,0(r1). */
    put32(p + 0x54, I(0x0F, 0, 1, 0x0880));
    put32(p + 0x58, I(0x0D, 1, 1, 0x0200));
    put32(p + 0x5C, I(0x09, 0, 2, 0x0055));
    put32(p + 0x60, I(0x2B, 1, 2, 0));
}

int main(void)
{
    uint32_t value = 0u;
    uint32_t executed = 0u;
    uint32_t entry = 0u;
    uint8_t elf[0x64u];
    const uint32_t base = UINT32_C(0x08000000);

    assert(!psp_core_read32(base, &value));
    assert(psp_core_init() == PSP_CORE_OK);
    assert(psp_core_write32(UINT32_C(0x08800100), UINT32_C(0xA1B2C3D4)));
    assert(psp_core_read32(UINT32_C(0x48800100), &value));
    assert(value == UINT32_C(0xA1B2C3D4));

    /* ADDIU r1,r0,7; ADDIU r2,r1,5 through the public core API. */
    assert(psp_core_write32(base, I(0x09, 0, 1, 7)));
    assert(psp_core_write32(base + 4u, I(0x09, 1, 2, 5)));
    assert(psp_core_start(base) == PSP_CORE_OK);
    assert(psp_core_run_steps(2u, &executed) == PSP_CORE_OK);
    assert(executed == 2u);
    assert(psp_core_run_steps(0u, &executed) == PSP_CORE_OK && executed == 0u);
    assert(psp_core_run_frame() == PSP_CORE_NOT_IMPLEMENTED);
    assert(!psp_core_read32(base, NULL));

    /* Public load API loads segments, sets entry PC and runs the loaded code. */
    make_test_elf(elf, sizeof(elf));
    assert(psp_core_load_image(elf, sizeof(elf), &entry) == PSP_CORE_OK);
    assert(entry == UINT32_C(0x08804000));
    assert(psp_core_run_steps(4u, &executed) == PSP_CORE_OK && executed == 4u);
    assert(psp_core_read32(UINT32_C(0x08800200), &value));
    assert(value == UINT32_C(0x55));

    assert(psp_core_load_image(elf, sizeof(elf) - 1u, &entry) == PSP_CORE_LOAD_ERROR);
    psp_core_shutdown();
    assert(!psp_core_read32(base, &value));
    puts("PASS: core loads a bounded ELF and executes its entry-point code.");
    return 0;
}
