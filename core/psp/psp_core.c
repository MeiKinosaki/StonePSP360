#include "psp_core.h"

#include "allegrex_state.h"
#include "psp_memory.h"

static AllegrexState g_cpu_state;
static PspMemory g_memory;
static int g_initialized;
static int g_memory_initialized;

PspCoreStatus psp_core_init(void)
{
    if (!g_memory_initialized) {
        if (!psp_memory_init(&g_memory)) {
            return PSP_CORE_OUT_OF_MEMORY;
        }
        g_memory_initialized = 1;
    }

    allegrex_state_reset(&g_cpu_state);
    g_initialized = 1;

    /* RAM is now owned by the core; CPU instruction execution is still a stub. */
    return PSP_CORE_OK;
}

void psp_core_shutdown(void)
{
    if (g_memory_initialized) {
        psp_memory_destroy(&g_memory);
        g_memory_initialized = 0;
    }
    g_initialized = 0;
}

PspCoreStatus psp_core_run_frame(void)
{
    if (!g_initialized) {
        return PSP_CORE_INVALID_ARGUMENT;
    }

    /* No guest instructions are executed in v8.10b. */
    return PSP_CORE_NOT_IMPLEMENTED;
}

bool psp_core_read8(uint32_t address, uint8_t *value)
{
    return g_initialized && psp_memory_read8(&g_memory, address, value);
}

bool psp_core_read16(uint32_t address, uint16_t *value)
{
    return g_initialized && psp_memory_read16(&g_memory, address, value);
}

bool psp_core_read32(uint32_t address, uint32_t *value)
{
    return g_initialized && psp_memory_read32(&g_memory, address, value);
}

bool psp_core_write8(uint32_t address, uint8_t value)
{
    return g_initialized && psp_memory_write8(&g_memory, address, value);
}

bool psp_core_write16(uint32_t address, uint16_t value)
{
    return g_initialized && psp_memory_write16(&g_memory, address, value);
}

bool psp_core_write32(uint32_t address, uint32_t value)
{
    return g_initialized && psp_memory_write32(&g_memory, address, value);
}

const char *psp_core_status_string(PspCoreStatus status)
{
    switch (status) {
    case PSP_CORE_OK:
        return "initialized; CPU execution not implemented";
    case PSP_CORE_NOT_IMPLEMENTED:
        return "CPU instruction execution not implemented";
    case PSP_CORE_INVALID_ARGUMENT:
        return "not initialized";
    case PSP_CORE_OUT_OF_MEMORY:
        return "could not allocate PSP RAM";
    default:
        return "unknown status";
    }
}
