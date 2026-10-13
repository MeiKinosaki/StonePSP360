#include "psp_memory.h"

#include <stdlib.h>

static bool psp_memory_offset(const PspMemory *memory, uint32_t address,
                              size_t access_size, size_t *offset)
{
    uint32_t segment;
    uint32_t physical;
    size_t candidate;

    if (memory == NULL || memory->ram == NULL || offset == NULL ||
        memory->ram_size == 0u) {
        return false;
    }

    segment = address & UINT32_C(0xE0000000);
    if (segment != 0u && segment != UINT32_C(0x40000000) &&
        segment != UINT32_C(0x80000000) &&
        segment != UINT32_C(0xA0000000) &&
        segment != UINT32_C(0xC0000000)) {
        return false;
    }

    physical = address & UINT32_C(0x1FFFFFFF);
    if (physical < PSP_RAM_BASE) {
        return false;
    }
    candidate = (size_t)(physical - PSP_RAM_BASE);
    if (candidate > memory->ram_size ||
        access_size > memory->ram_size - candidate) {
        return false;
    }

    *offset = candidate;
    return true;
}

bool psp_memory_init(PspMemory *memory)
{
    if (memory == NULL) {
        return false;
    }
    memory->ram = NULL;
    memory->ram_size = 0u;
    memory->ram = (uint8_t *)calloc(PSP_RAM_SIZE, sizeof(uint8_t));
    if (memory->ram == NULL) {
        return false;
    }
    memory->ram_size = PSP_RAM_SIZE;
    return true;
}

void psp_memory_destroy(PspMemory *memory)
{
    if (memory == NULL) {
        return;
    }
    free(memory->ram);
    memory->ram = NULL;
    memory->ram_size = 0u;
}

bool psp_memory_read8(const PspMemory *memory, uint32_t address, uint8_t *value)
{
    size_t offset;
    if (value == NULL || !psp_memory_offset(memory, address, 1u, &offset)) {
        return false;
    }
    *value = memory->ram[offset];
    return true;
}

bool psp_memory_read16(const PspMemory *memory, uint32_t address, uint16_t *value)
{
    size_t offset;
    if (value == NULL || !psp_memory_offset(memory, address, 2u, &offset)) {
        return false;
    }
    *value = (uint16_t)memory->ram[offset] |
             (uint16_t)((uint16_t)memory->ram[offset + 1u] << 8);
    return true;
}

bool psp_memory_read32(const PspMemory *memory, uint32_t address, uint32_t *value)
{
    size_t offset;
    if (value == NULL || !psp_memory_offset(memory, address, 4u, &offset)) {
        return false;
    }
    *value = (uint32_t)memory->ram[offset] |
             ((uint32_t)memory->ram[offset + 1u] << 8) |
             ((uint32_t)memory->ram[offset + 2u] << 16) |
             ((uint32_t)memory->ram[offset + 3u] << 24);
    return true;
}

bool psp_memory_write8(PspMemory *memory, uint32_t address, uint8_t value)
{
    size_t offset;
    if (!psp_memory_offset(memory, address, 1u, &offset)) {
        return false;
    }
    memory->ram[offset] = value;
    return true;
}

bool psp_memory_write16(PspMemory *memory, uint32_t address, uint16_t value)
{
    size_t offset;
    if (!psp_memory_offset(memory, address, 2u, &offset)) {
        return false;
    }
    memory->ram[offset] = (uint8_t)(value & UINT16_C(0x00FF));
    memory->ram[offset + 1u] = (uint8_t)(value >> 8);
    return true;
}

bool psp_memory_write32(PspMemory *memory, uint32_t address, uint32_t value)
{
    size_t offset;
    if (!psp_memory_offset(memory, address, 4u, &offset)) {
        return false;
    }
    memory->ram[offset] = (uint8_t)(value & UINT32_C(0x000000FF));
    memory->ram[offset + 1u] = (uint8_t)((value >> 8) & UINT32_C(0x000000FF));
    memory->ram[offset + 2u] = (uint8_t)((value >> 16) & UINT32_C(0x000000FF));
    memory->ram[offset + 3u] = (uint8_t)((value >> 24) & UINT32_C(0x000000FF));
    return true;
}
