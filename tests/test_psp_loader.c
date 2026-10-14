#include "../core/psp/psp_loader.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>

static void put16(uint8_t *p, uint16_t v)
{
    p[0] = (uint8_t)v; p[1] = (uint8_t)(v >> 8);
}
static void put32(uint8_t *p, uint32_t v)
{
    p[0] = (uint8_t)v; p[1] = (uint8_t)(v >> 8);
    p[2] = (uint8_t)(v >> 16); p[3] = (uint8_t)(v >> 24);
}
static void make_elf(uint8_t *p, size_t size)
{
    memset(p, 0, size);
    p[0] = 0x7F; p[1] = 'E'; p[2] = 'L'; p[3] = 'F';
    p[4] = 1; p[5] = 1; p[6] = 1;
    put16(p + 16, 2); put16(p + 18, 8); put32(p + 20, 1);
    put32(p + 24, 0x08804000u);
    put32(p + 28, 0x34u);
    put16(p + 40, 0x34u); put16(p + 42, 0x20u); put16(p + 44, 1u);
    put32(p + 0x34, 1u); /* PT_LOAD */
    put32(p + 0x38, 0x54u); /* p_offset */
    put32(p + 0x3C, 0x08804000u); /* p_vaddr */
    put32(p + 0x44, 4u); /* p_filesz */
    put32(p + 0x48, 8u); /* p_memsz */
    put32(p + 0x4C, 5u); /* PF_R | PF_X */
    put32(p + 0x50, 4u); /* p_align */
    put32(p + 0x54, 0x24020001u);
}
int main(void)
{
    uint8_t elf[0x5Cu];
    uint8_t pbp[0x90u];
    PspImageInfo info;
    PspMemory memory;
    uint32_t entry = 0u, word = 0u;
    size_t i;

    make_elf(elf, sizeof(elf));
    assert(psp_loader_inspect(elf, sizeof(elf), &info) == PSP_LOADER_OK);
    assert(info.kind == PSP_IMAGE_ELF32_MIPS_LE);
    assert(info.entry_point == 0x08804000u && info.program_header_count == 1u);

    assert(psp_memory_init(&memory));
    assert(psp_memory_write32(&memory, 0x08804000u, 0xDEADBEEFu));
    assert(psp_loader_load(elf, sizeof(elf), &memory, &entry) == PSP_LOADER_OK);
    assert(entry == 0x08804000u);
    assert(psp_memory_read32(&memory, entry, &word) && word == 0x24020001u);
    assert(psp_memory_read8(&memory, entry + 4u, (uint8_t *)&i));
    {
        uint8_t bss = 0xFFu;
        assert(psp_memory_read8(&memory, entry + 4u, &bss) && bss == 0u);
    }

    /* Invalid destination must fail without overwriting an earlier marker. */
    put32(elf + 0x3C, 0x0A000000u);
    assert(psp_memory_write32(&memory, 0x08804000u, 0xDEADBEEFu));
    assert(psp_loader_load(elf, sizeof(elf), &memory, &entry) == PSP_LOADER_SEGMENT_OUT_OF_RANGE);
    assert(psp_memory_read32(&memory, 0x08804000u, &word) && word == 0xDEADBEEFu);
    psp_memory_destroy(&memory);

    memset(pbp, 0, sizeof(pbp));
    memcpy(pbp, "\0PBP", 4u);
    for (i = 0; i < 8u; ++i) put32(pbp + 8u + i * 4u, 0x28u);
    put32(pbp + 8u + 6u * 4u, 0x28u);
    put32(pbp + 8u + 7u * 4u, 0x28u);
    assert(psp_loader_inspect(pbp, sizeof(pbp), &info) == PSP_LOADER_OK);
    assert(info.kind == PSP_IMAGE_PBP && info.payload_size == 0u);

    memset(pbp, 0, sizeof(pbp));
    memcpy(pbp, "\0PBP", 4u);
    for (i = 0; i < 6u; ++i) put32(pbp + 8u + i * 4u, 0x28u);
    put32(pbp + 8u + 6u * 4u, 0x28u);
    put32(pbp + 8u + 7u * 4u, 0x84u);
    make_elf(pbp + 0x28u, 0x5Cu);
    assert(psp_loader_inspect(pbp, sizeof(pbp), &info) == PSP_LOADER_OK);
    assert(info.kind == PSP_IMAGE_PBP_WITH_ELF32_MIPS_LE);
    assert(info.entry_point == 0x08804000u);
    assert(info.payload_offset == 0x28u && info.payload_size == 0x5Cu);

    assert(psp_memory_init(&memory));
    assert(psp_loader_load(pbp, sizeof(pbp), &memory, &entry) == PSP_LOADER_OK);
    assert(entry == 0x08804000u);
    assert(psp_memory_read32(&memory, entry, &word) && word == 0x24020001u);
    psp_memory_destroy(&memory);

    put32(pbp + 8u + 6u * 4u, 0xA0u);
    assert(psp_loader_inspect(pbp, sizeof(pbp), &info) == PSP_LOADER_INVALID_PBP);
    assert(psp_loader_inspect(NULL, sizeof(elf), &info) == PSP_LOADER_INVALID_ARGUMENT);
    assert(psp_loader_inspect(elf, sizeof(elf) - 1u, &info) == PSP_LOADER_OK);
    puts("PASS: PBP/ELF inspection, bounded PT_LOAD copy and BSS zeroing.");
    return 0;
}
