#include "psp_loader.h"

#include <stdbool.h>
#include <string.h>

#define PBP_HEADER_SIZE 0x28u
#define ELF32_HEADER_SIZE 0x34u
#define ELF32_PROGRAM_HEADER_SIZE 0x20u
#define ELF_ET_EXEC 2u
#define ELF_EM_MIPS 8u
#define PT_LOAD 1u
#define PF_X 1u

static uint16_t read_le16(const uint8_t *p)
{
    return (uint16_t)((uint16_t)p[0] | ((uint16_t)p[1] << 8));
}

static uint32_t read_le32(const uint8_t *p)
{
    return (uint32_t)p[0] | ((uint32_t)p[1] << 8) |
           ((uint32_t)p[2] << 16) | ((uint32_t)p[3] << 24);
}

static bool range_fits(size_t total, size_t offset, size_t length)
{
    return offset <= total && length <= total - offset;
}

static PspLoaderStatus inspect_elf(const uint8_t *data, size_t size,
                                   PspImageInfo *info)
{
    uint32_t phoff;
    uint16_t ehsize, phentsize, phnum;
    size_t phbytes;

    if (data == NULL || info == NULL) return PSP_LOADER_INVALID_ARGUMENT;
    if (size < ELF32_HEADER_SIZE || data[0] != 0x7Fu ||
        data[1] != 'E' || data[2] != 'L' || data[3] != 'F') {
        return PSP_LOADER_INVALID_ELF;
    }
    if (data[4] != 1u || data[5] != 1u || data[6] != 1u) {
        return PSP_LOADER_UNSUPPORTED_IMAGE;
    }
    if (read_le16(data + 16) != ELF_ET_EXEC ||
        read_le16(data + 18) != ELF_EM_MIPS ||
        read_le32(data + 20) != 1u) {
        return PSP_LOADER_UNSUPPORTED_IMAGE;
    }

    ehsize = read_le16(data + 40);
    phoff = read_le32(data + 28);
    phentsize = read_le16(data + 42);
    phnum = read_le16(data + 44);
    if (ehsize < ELF32_HEADER_SIZE ||
        (phnum != 0u && phentsize < ELF32_PROGRAM_HEADER_SIZE)) {
        return PSP_LOADER_INVALID_ELF;
    }
    if (phnum != 0u) {
        if ((size_t)phnum > ((size_t)-1) / (size_t)phentsize) {
            return PSP_LOADER_INVALID_ELF;
        }
        phbytes = (size_t)phnum * (size_t)phentsize;
        if (!range_fits(size, (size_t)phoff, phbytes)) {
            return PSP_LOADER_INVALID_ELF;
        }
    } else if (phoff > size) {
        return PSP_LOADER_INVALID_ELF;
    }

    info->kind = PSP_IMAGE_ELF32_MIPS_LE;
    info->entry_point = read_le32(data + 24);
    info->program_header_offset = phoff;
    info->program_header_entry_size = phentsize;
    info->program_header_count = phnum;
    info->payload_offset = 0u;
    info->payload_size = size;
    return PSP_LOADER_OK;
}

PspLoaderStatus psp_loader_inspect(const uint8_t *data, size_t size,
                                   PspImageInfo *info)
{
    uint32_t offsets[8];
    size_t i;
    PspLoaderStatus status;

    if (data == NULL || info == NULL) return PSP_LOADER_INVALID_ARGUMENT;
    memset(info, 0, sizeof(*info));
    if (size >= 4u && data[0] == 0x7Fu && data[1] == 'E' &&
        data[2] == 'L' && data[3] == 'F') {
        return inspect_elf(data, size, info);
    }
    if (size < PBP_HEADER_SIZE || memcmp(data, "\0PBP", 4u) != 0) {
        return PSP_LOADER_UNSUPPORTED_IMAGE;
    }
    for (i = 0u; i < 8u; ++i) {
        offsets[i] = read_le32(data + 8u + i * 4u);
        if (offsets[i] < PBP_HEADER_SIZE || (size_t)offsets[i] > size) {
            return PSP_LOADER_INVALID_PBP;
        }
        if (i != 0u && offsets[i] < offsets[i - 1u]) {
            return PSP_LOADER_INVALID_PBP;
        }
    }
    info->kind = PSP_IMAGE_PBP;
    info->payload_offset = offsets[6];
    info->payload_size = (size_t)offsets[7] - (size_t)offsets[6];
    if (info->payload_size >= 4u &&
        range_fits(size, info->payload_offset, info->payload_size)) {
        PspImageInfo elf_info;
        status = inspect_elf(data + info->payload_offset,
                             info->payload_size, &elf_info);
        if (status == PSP_LOADER_OK) {
            *info = elf_info;
            info->kind = PSP_IMAGE_PBP_WITH_ELF32_MIPS_LE;
            info->payload_offset = offsets[6];
            info->payload_size = (size_t)offsets[7] - (size_t)offsets[6];
            return PSP_LOADER_OK;
        }
    }
    return PSP_LOADER_OK;
}

static bool guest_ram_range_valid(const PspMemory *memory, uint32_t address,
                                  uint32_t length)
{
    uint32_t segment = address & UINT32_C(0xE0000000);
    uint32_t physical = address & UINT32_C(0x1FFFFFFF);
    size_t offset;
    if (memory == NULL || memory->ram == NULL || memory->ram_size == 0u)
        return false;
    if (segment != 0u && segment != UINT32_C(0x40000000) &&
        segment != UINT32_C(0x80000000) && segment != UINT32_C(0xA0000000) &&
        segment != UINT32_C(0xC0000000)) return false;
    if (physical < PSP_RAM_BASE) return false;
    offset = (size_t)(physical - PSP_RAM_BASE);
    return offset <= memory->ram_size &&
           (size_t)length <= memory->ram_size - offset;
}

static bool is_power_of_two(uint32_t value)
{
    return value != 0u && (value & (value - 1u)) == 0u;
}

PspLoaderStatus psp_loader_load(const uint8_t *data, size_t size,
                                PspMemory *memory, uint32_t *entry_point)
{
    PspImageInfo info;
    PspLoaderStatus status;
    const uint8_t *elf;
    size_t elf_size;
    uint16_t i;
    unsigned loadable = 0u;
    bool entry_executable = false;

    if (data == NULL || memory == NULL || memory->ram == NULL ||
        entry_point == NULL) return PSP_LOADER_INVALID_ARGUMENT;
    status = psp_loader_inspect(data, size, &info);
    if (status != PSP_LOADER_OK) return status;
    if (info.kind == PSP_IMAGE_ELF32_MIPS_LE) {
        elf = data;
        elf_size = size;
    } else if (info.kind == PSP_IMAGE_PBP_WITH_ELF32_MIPS_LE) {
        elf = data + info.payload_offset;
        elf_size = info.payload_size;
    } else {
        return PSP_LOADER_UNSUPPORTED_IMAGE;
    }

    /* Pass 1: validate all PT_LOAD ranges before touching guest memory. */
    for (i = 0u; i < info.program_header_count; ++i) {
        const uint8_t *ph = elf + info.program_header_offset +
                            (size_t)i * info.program_header_entry_size;
        uint32_t type = read_le32(ph);
        uint32_t offset, vaddr, filesz, memsz, flags, align;
        if (type != PT_LOAD) continue;
        offset = read_le32(ph + 4);
        vaddr = read_le32(ph + 8);
        filesz = read_le32(ph + 16);
        memsz = read_le32(ph + 20);
        flags = read_le32(ph + 24);
        align = read_le32(ph + 28);
        if (filesz > memsz || !range_fits(elf_size, (size_t)offset, (size_t)filesz) ||
            !guest_ram_range_valid(memory, vaddr, memsz)) {
            return PSP_LOADER_SEGMENT_OUT_OF_RANGE;
        }
        if (align > 1u && (!is_power_of_two(align) ||
                           ((vaddr - offset) & (align - 1u)) != 0u)) {
            return PSP_LOADER_INVALID_ELF;
        }
        if (memsz != 0u) ++loadable;
        if ((flags & PF_X) != 0u && memsz != 0u &&
            info.entry_point >= vaddr &&
            (uint64_t)info.entry_point < (uint64_t)vaddr + (uint64_t)memsz) {
            entry_executable = true;
        }
    }
    if (loadable == 0u) return PSP_LOADER_NO_LOADABLE_SEGMENTS;
    if (!entry_executable) return PSP_LOADER_ENTRY_NOT_EXECUTABLE;

    /* Pass 2: copy file bytes and zero the BSS tail. */
    for (i = 0u; i < info.program_header_count; ++i) {
        const uint8_t *ph = elf + info.program_header_offset +
                            (size_t)i * info.program_header_entry_size;
        uint32_t type = read_le32(ph);
        uint32_t offset, vaddr, filesz, memsz, j;
        if (type != PT_LOAD) continue;
        offset = read_le32(ph + 4);
        vaddr = read_le32(ph + 8);
        filesz = read_le32(ph + 16);
        memsz = read_le32(ph + 20);
        for (j = 0u; j < filesz; ++j) {
            if (!psp_memory_write8(memory, vaddr + j, elf[offset + j]))
                return PSP_LOADER_SEGMENT_OUT_OF_RANGE;
        }
        for (j = filesz; j < memsz; ++j) {
            if (!psp_memory_write8(memory, vaddr + j, 0u))
                return PSP_LOADER_SEGMENT_OUT_OF_RANGE;
        }
    }
    *entry_point = info.entry_point;
    return PSP_LOADER_OK;
}

const char *psp_loader_status_string(PspLoaderStatus status)
{
    switch (status) {
    case PSP_LOADER_OK: return "image inspected or ELF segments loaded";
    case PSP_LOADER_INVALID_ARGUMENT: return "invalid argument";
    case PSP_LOADER_INVALID_PBP: return "invalid PBP header or section offsets";
    case PSP_LOADER_INVALID_ELF: return "invalid ELF header, table or alignment";
    case PSP_LOADER_UNSUPPORTED_IMAGE: return "unsupported image format or PRX payload";
    case PSP_LOADER_SEGMENT_OUT_OF_RANGE: return "loadable segment exceeds file or PSP RAM bounds";
    case PSP_LOADER_NO_LOADABLE_SEGMENTS: return "ELF has no non-empty PT_LOAD segments";
    case PSP_LOADER_ENTRY_NOT_EXECUTABLE: return "ELF entry point is not inside an executable PT_LOAD segment";
    default: return "unknown status";
    }
}
