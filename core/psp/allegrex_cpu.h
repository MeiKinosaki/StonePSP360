#ifndef STONEPSP360_ALLEGREX_CPU_H
#define STONEPSP360_ALLEGREX_CPU_H

#include <stdint.h>
#include "allegrex_state.h"
#include "psp_memory.h"

typedef enum AllegrexStepResult {
    ALLEGREX_STEP_OK = 0,
    ALLEGREX_STEP_ILLEGAL_INSTRUCTION = 1,
    ALLEGREX_STEP_MEMORY_FAULT = 2,
    ALLEGREX_STEP_INVALID_ARGUMENT = 3
} AllegrexStepResult;

/* Execute one Allegrex/MIPS instruction, including its delay-slot PC model. */
AllegrexStepResult allegrex_step(AllegrexState *state, PspMemory *memory);

#endif
