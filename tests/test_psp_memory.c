#include "../core/psp/psp_memory.h"

#include <assert.h>
#include <stdio.h>

int main(void)
{
    PspMemory memory;
    uint8_t value8;
    uint16_t value16;
    uint32_t value32;

    assert(psp_memory_init(&memory));
    assert(memory.ram_size == PSP_RAM_SIZE);

    assert(psp_memory_write32(&memory, UINT32_C(0x08000100), UINT32_C(0x12345678)));
    assert(psp_memory_read32(&memory, UINT32_C(0x08000100), &value32));
    assert(value32 == UINT32_C(0x12345678));

    /* Cached, uncached, kernel, and uncached-kernel aliases share storage. */
    assert(psp_memory_read32(&memory, UINT32_C(0x48000100), &value32));
    assert(value32 == UINT32_C(0x12345678));
    assert(psp_memory_read32(&memory, UINT32_C(0x88000100), &value32));
    assert(value32 == UINT32_C(0x12345678));
    assert(psp_memory_read32(&memory, UINT32_C(0xC8000100), &value32));
    assert(value32 == UINT32_C(0x12345678));

    assert(psp_memory_write16(&memory, UINT32_C(0x08000200), UINT16_C(0xABCD)));
    assert(psp_memory_read16(&memory, UINT32_C(0x08000200), &value16));
    assert(value16 == UINT16_C(0xABCD));
    assert(psp_memory_read8(&memory, UINT32_C(0x08000201), &value8));
    assert(value8 == UINT8_C(0xAB));

    assert(!psp_memory_read8(&memory, UINT32_C(0x00000100), &value8));
    assert(!psp_memory_read32(&memory, UINT32_C(0x09FFFFFE), &value32));
    assert(!psp_memory_write8(&memory, UINT32_C(0x0A000000), 1u));
    assert(!psp_memory_read8(&memory, UINT32_C(0x08000000), NULL));

    psp_memory_destroy(&memory);
    assert(memory.ram == NULL);
    assert(memory.ram_size == 0u);
    puts("PASS: PSP RAM reads/writes, little-endian values, aliases, and bounds.");
    return 0;
}
