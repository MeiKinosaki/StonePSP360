#ifndef STONEPSP360_PSP_CORE_H
#define STONEPSP360_PSP_CORE_H

#include <stdbool.h>
#include <stdint.h>

typedef enum PspCoreStatus {
    PSP_CORE_OK = 0,
    PSP_CORE_NOT_IMPLEMENTED = 1,
    PSP_CORE_INVALID_ARGUMENT = 2,
    PSP_CORE_OUT_OF_MEMORY = 3
} PspCoreStatus;

PspCoreStatus psp_core_init(void);
void psp_core_shutdown(void);
PspCoreStatus psp_core_run_frame(void);
bool psp_core_read8(uint32_t address, uint8_t *value);
bool psp_core_read16(uint32_t address, uint16_t *value);
bool psp_core_read32(uint32_t address, uint32_t *value);
bool psp_core_write8(uint32_t address, uint8_t value);
bool psp_core_write16(uint32_t address, uint16_t value);
bool psp_core_write32(uint32_t address, uint32_t value);
const char *psp_core_status_string(PspCoreStatus status);

#endif
