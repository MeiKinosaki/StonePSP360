#include "../core/psp/psp_core.h"

#include <assert.h>
#include <stdio.h>

#define I(op, rs, rt, imm) (((uint32_t)(op) << 26) | ((uint32_t)(rs) << 21) | ((uint32_t)(rt) << 16) | ((uint32_t)(imm) & UINT32_C(0xFFFF)))

int main(void)
{
    uint32_t value = 0u;
    uint32_t executed = 0u;
    const uint32_t base = UINT32_C(0x08000000);

    assert(!psp_core_read32(base, &value));
    assert(psp_core_init() == PSP_CORE_OK);
    assert(psp_core_write32(UINT32_C(0x08800100), UINT32_C(0xA1B2C3D4)));
    assert(psp_core_read32(UINT32_C(0x48800100), &value));
    assert(value == UINT32_C(0xA1B2C3D4));

    /* ADDIU r1, r0, 7; ADDIU r2, r1, 5; run through the real core API. */
    assert(psp_core_write32(base, I(0x09, 0, 1, 7)));
    assert(psp_core_write32(base + 4u, I(0x09, 1, 2, 5)));
    assert(psp_core_start(base) == PSP_CORE_OK);
    assert(psp_core_run_steps(2u, &executed) == PSP_CORE_OK);
    assert(executed == 2u);
    assert(psp_core_run_steps(0u, &executed) == PSP_CORE_OK && executed == 0u);

    /* Core returns a defined status rather than claiming frame execution exists. */
    assert(psp_core_run_frame() == PSP_CORE_NOT_IMPLEMENTED);
    assert(!psp_core_read32(base, NULL));
    psp_core_shutdown();
    assert(!psp_core_read32(base, &value));
    puts("PASS: core executes bounded Allegrex instructions and reports frame stub honestly.");
    return 0;
}
