#include "../core/psp/allegrex_state.h"
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

int main(void)
{
    AllegrexState state;
    size_t i;
    for (i = 0; i < 32; ++i) {
        state.gpr[i] = UINT32_C(0xA5A5A5A5);
        state.fpr_bits[i] = UINT32_C(0x5A5A5A5A);
    }
    state.hi = UINT32_C(0x11111111);
    state.lo = UINT32_C(0x22222222);
    state.pc = UINT32_C(0x33333333);
    state.next_pc = UINT32_C(0x44444444);
    state.fcr31 = UINT32_C(0x55555555);
    state.cop0_status = UINT32_C(0x66666666);
    state.cop0_cause = UINT32_C(0x77777777);
    state.cop0_epc = UINT32_C(0x88888888);
    allegrex_state_reset(&state);
    for (i = 0; i < 32; ++i) {
        assert(state.gpr[i] == 0);
        assert(state.fpr_bits[i] == 0);
    }
    assert(state.gpr[0] == 0);
    assert(state.hi == 0);
    assert(state.lo == 0);
    assert(state.pc == 0);
    assert(state.next_pc == 4);
    assert(state.fcr31 == 0);
    assert(state.cop0_status == 0);
    assert(state.cop0_cause == 0);
    assert(state.cop0_epc == 0);
    allegrex_state_reset(NULL);
    puts("PASS: Allegrex reset initializes all scaffold registers.");
    return 0;
}
