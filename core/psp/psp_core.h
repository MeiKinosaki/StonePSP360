#ifndef STONEPSP360_PSP_CORE_H
#define STONEPSP360_PSP_CORE_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef enum PspCoreStatus {
    PSP_CORE_OK = 0,
    PSP_CORE_NOT_IMPLEMENTED = 1,
    PSP_CORE_INVALID_ARGUMENT = 2,
    PSP_CORE_OUT_OF_MEMORY = 3,
    PSP_CORE_CPU_ILLEGAL_INSTRUCTION = 4,
    PSP_CORE_CPU_MEMORY_FAULT = 5,
    PSP_CORE_LOAD_ERROR = 6
} PspCoreStatus;

PspCoreStatus psp_core_init(void);
void psp_core_shutdown(void);
/* Load a supported raw ELF/PBP buffer into guest RAM and start at its entry point.
   Caller retains ownership of the input buffer. */
PspCoreStatus psp_core_load_image(const uint8_t *data, size_t size,
                                  uint32_t *entry_point);
/* Set the guest entry point after memory/program setup. */
PspCoreStatus psp_core_start(uint32_t entry_pc);
/* Run a bounded number of guest CPU instructions. */
PspCoreStatus psp_core_run_steps(uint32_t max_steps, uint32_t *executed_steps);
/* Frame execution remains unavailable until scheduler, GPU and timing exist. */
PspCoreStatus psp_core_run_frame(void);
bool psp_core_read8(uint32_t address, uint8_t *value);
bool psp_core_read16(uint32_t address, uint16_t *value);
bool psp_core_read32(uint32_t address, uint32_t *value);
bool psp_core_write8(uint32_t address, uint8_t value);
bool psp_core_write16(uint32_t address, uint16_t value);
bool psp_core_write32(uint32_t address, uint32_t value);
const char *psp_core_status_string(PspCoreStatus status);

#endif
