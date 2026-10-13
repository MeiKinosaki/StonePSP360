#ifndef STONEPSP360_ALLEGREX_STATE_H
#define STONEPSP360_ALLEGREX_STATE_H

#include <stdint.h>

/*
 * Initial PSP Allegrex architectural-state scaffold.
 * This defines storage only; instruction execution, exceptions, COP0,
 * VFPU, caches and timing are not implemented in v8.6b.
 */
typedef struct AllegrexState {
    uint32_t gpr[32];
    uint32_t hi;
    uint32_t lo;
    uint32_t pc;
    uint32_t next_pc;
    uint32_t fpr_bits[32];
    uint32_t fcr31;
    uint32_t cop0_status;
    uint32_t cop0_cause;
    uint32_t cop0_epc;
} AllegrexState;

void allegrex_state_reset(AllegrexState *state);

#endif
