#ifndef STONEPSP360_PSP_LOADER_H
#define STONEPSP360_PSP_LOADER_H

#include <stddef.h>
#include <stdint.h>

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
    PSP_LOADER_UNSUPPORTED_IMAGE = 4
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

/* Inspect only; this does not decrypt PRX or load program segments into RAM. */
PspLoaderStatus psp_loader_inspect(const uint8_t *data, size_t size,
                                   PspImageInfo *info);
const char *psp_loader_status_string(PspLoaderStatus status);

#endif
