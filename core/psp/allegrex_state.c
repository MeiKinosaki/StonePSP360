#include "allegrex_state.h"

#include <stddef.h>

void allegrex_state_reset(AllegrexState *state)
{
    size_t i;

    if (state == NULL) {
        return;
    }

    for (i = 0; i < 32; ++i) {
        state->gpr[i] = 0;
        state->fpr_bits[i] = 0;
    }

    state->hi = 0;
    state->lo = 0;
    state->pc = 0;
    state->next_pc = 4;
    state->fcr31 = 0;
    state->cop0_status = 0;
    state->cop0_cause = 0;
    state->cop0_epc = 0;

    /* MIPS register zero is hard-wired to zero. */
    state->gpr[0] = 0;
}
