#include "psp_core.h"

#include "allegrex_cpu.h"
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

PspCoreStatus psp_core_start(uint32_t entry_pc)
{
    if (!g_initialized) return PSP_CORE_INVALID_ARGUMENT;
    if ((entry_pc & 3u) != 0u) return PSP_CORE_INVALID_ARGUMENT;
    allegrex_state_reset(&g_cpu_state);
    g_cpu_state.pc = entry_pc;
    g_cpu_state.next_pc = entry_pc + 4u;
    return PSP_CORE_OK;
}

PspCoreStatus psp_core_run_steps(uint32_t max_steps, uint32_t *executed_steps)
{
    AllegrexStepResult result;
    if (executed_steps != NULL) *executed_steps = 0u;
    if (!g_initialized) return PSP_CORE_INVALID_ARGUMENT;

    result = allegrex_run(&g_cpu_state, &g_memory, max_steps, executed_steps);
    switch (result) {
    case ALLEGREX_STEP_OK: return PSP_CORE_OK;
    case ALLEGREX_STEP_ILLEGAL_INSTRUCTION: return PSP_CORE_CPU_ILLEGAL_INSTRUCTION;
    case ALLEGREX_STEP_MEMORY_FAULT: return PSP_CORE_CPU_MEMORY_FAULT;
    case ALLEGREX_STEP_INVALID_ARGUMENT: return PSP_CORE_INVALID_ARGUMENT;
    default: return PSP_CORE_NOT_IMPLEMENTED;
    }
}

PspCoreStatus psp_core_run_frame(void)
{
    if (!g_initialized) return PSP_CORE_INVALID_ARGUMENT;
    /* A frame needs the PSP scheduler, display list/GPU and timing model. */
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
    case PSP_CORE_OK: return "operation completed";
    case PSP_CORE_NOT_IMPLEMENTED: return "frame execution requires scheduler, GPU and timing";
    case PSP_CORE_INVALID_ARGUMENT: return "invalid argument or core not initialized";
    case PSP_CORE_OUT_OF_MEMORY: return "could not allocate PSP RAM";
    case PSP_CORE_CPU_ILLEGAL_INSTRUCTION: return "guest CPU encountered an unsupported instruction";
    case PSP_CORE_CPU_MEMORY_FAULT: return "guest CPU memory access failed";
    default: return "unknown status";
    }
}
