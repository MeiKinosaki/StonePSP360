#ifndef STONEPSP360_PSP_MEMORY_H
#define STONEPSP360_PSP_MEMORY_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define PSP_RAM_BASE UINT32_C(0x08000000)
#define PSP_RAM_SIZE ((size_t)32u * 1024u * 1024u)

typedef struct PspMemory {
    uint8_t *ram;
    size_t ram_size;
} PspMemory;

bool psp_memory_init(PspMemory *memory);
void psp_memory_destroy(PspMemory *memory);
bool psp_memory_read8(const PspMemory *memory, uint32_t address, uint8_t *value);
bool psp_memory_read16(const PspMemory *memory, uint32_t address, uint16_t *value);
bool psp_memory_read32(const PspMemory *memory, uint32_t address, uint32_t *value);
bool psp_memory_write8(PspMemory *memory, uint32_t address, uint8_t value);
bool psp_memory_write16(PspMemory *memory, uint32_t address, uint16_t value);
bool psp_memory_write32(PspMemory *memory, uint32_t address, uint32_t value);

#endif
