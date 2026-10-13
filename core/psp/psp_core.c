#include "psp_core.h"
#include "allegrex_state.h"

static AllegrexState g_cpu_state;
static int g_initialized;

PspCoreStatus psp_core_init(void)
{
    allegrex_state_reset(&g_cpu_state);
    g_initialized = 1;

    /*
     * This is deliberately a scaffold, not a CPU interpreter.
     * Return NOT_IMPLEMENTED until fetch/decode/execute, memory,
     * interrupts and timing have real implementations.
     */
    return PSP_CORE_NOT_IMPLEMENTED;
}

PspCoreStatus psp_core_run_frame(void)
{
    if (!g_initialized) {
        return PSP_CORE_INVALID_ARGUMENT;
    }

    /* No guest instructions are executed in this milestone. */
    return PSP_CORE_NOT_IMPLEMENTED;
}

const char *psp_core_status_string(PspCoreStatus status)
{
    switch (status) {
    case PSP_CORE_OK:
        return "ready";
    case PSP_CORE_NOT_IMPLEMENTED:
        return "scaffold only; emulation not implemented";
    case PSP_CORE_INVALID_ARGUMENT:
        return "not initialized";
    default:
        return "unknown status";
    }
}
