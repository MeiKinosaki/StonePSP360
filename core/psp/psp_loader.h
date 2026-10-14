#ifndef STONEPSP360_PSP_LOADER_H
#define STONEPSP360_PSP_LOADER_H

#include <stddef.h>
#include <stdint.h>
#include "psp_memory.h"

typedef enum PspImageKind {
    PSP_IMAGE_UNKNOWN = 0,
    PSP_IMAGE_PBP = 1,
    PSP_IMAGE_ELF32_MIPS_LE = 2,
    PSP_IMAGE_PBP_WITH_ELF32_MIPS_LE = 3
} PspImageKind;

typedef enum PspLoaderStatus {
    PSP_LOADER_OK = 0,
    PSP_LOADER_INVALID_ARGUMENT = 1,
    PSP_LOADER_INVALID_PBP = 2,
    PSP_LOADER_INVALID_ELF = 3,
    PSP_LOADER_UNSUPPORTED_IMAGE = 4,
    PSP_LOADER_SEGMENT_OUT_OF_RANGE = 5,
    PSP_LOADER_NO_LOADABLE_SEGMENTS = 6,
    PSP_LOADER_ENTRY_NOT_EXECUTABLE = 7
} PspLoaderStatus;

typedef struct PspImageInfo {
    PspImageKind kind;
    uint32_t entry_point;
    uint32_t program_header_offset;
    uint16_t program_header_entry_size;
    uint16_t program_header_count;
    size_t payload_offset;
    size_t payload_size;
} PspImageInfo;

/* Inspection does not decrypt/decompress PRX. */
PspLoaderStatus psp_loader_inspect(const uint8_t *data, size_t size,
                                   PspImageInfo *info);
/* Loads validated PT_LOAD segments from raw ELF or PBP with direct ELF DATA.PSP.
   Validates every segment before modifying guest RAM. */
PspLoaderStatus psp_loader_load(const uint8_t *data, size_t size,
                                PspMemory *memory, uint32_t *entry_point);
const char *psp_loader_status_string(PspLoaderStatus status);

#endif
