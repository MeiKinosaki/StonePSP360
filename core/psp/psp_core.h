#ifndef STONEPSP360_PSP_CORE_H
#define STONEPSP360_PSP_CORE_H

typedef enum PspCoreStatus {
    PSP_CORE_OK = 0,
    PSP_CORE_NOT_IMPLEMENTED = 1,
    PSP_CORE_INVALID_ARGUMENT = 2
} PspCoreStatus;

PspCoreStatus psp_core_init(void);
PspCoreStatus psp_core_run_frame(void);
const char *psp_core_status_string(PspCoreStatus status);

#endif
