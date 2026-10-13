#include "../core/psp/psp_core.h"

#include <assert.h>
#include <stdio.h>

int main(void)
{
    uint32_t value;

    assert(!psp_core_read32(UINT32_C(0x08000000), &value));
    assert(psp_core_init() == PSP_CORE_OK);
    assert(psp_core_write32(UINT32_C(0x08800100), UINT32_C(0xA1B2C3D4)));
    assert(psp_core_read32(UINT32_C(0x48800100), &value));
    assert(value == UINT32_C(0xA1B2C3D4));
    assert(psp_core_run_frame() == PSP_CORE_NOT_IMPLEMENTED);
    assert(psp_core_read32(UINT32_C(0x08800100), NULL) == false);
    psp_core_shutdown();
    assert(!psp_core_read32(UINT32_C(0x08800100), &value));
    puts("PASS: core owns PSP RAM, exposes accessors, and reports CPU stub honestly.");
    return 0;
}
