#include "psp_ge.h"

#include <string.h>

void psp_ge_init(PspGeState *state)
{
    if (state != NULL) memset(state, 0, sizeof(*state));
}

bool psp_ge_process_word(PspGeState *state, uint32_t word,
                         PspGeCommandResult *result)
{
    uint8_t opcode;
    uint32_t parameter;
    bool recognized = true;
    if (state == NULL) return false;
    opcode = (uint8_t)((word >> 24) & 0xFFu);
    parameter = word & 0x00FFFFFFu;
    switch (opcode) {
    case PSP_GE_COMMAND_BASE:
        state->base = parameter & 0x00FF0000u;
        break;
    case PSP_GE_COMMAND_OFFSET:
        state->offset = (parameter << 8);
        break;
    case PSP_GE_COMMAND_VADDR:
        state->vertex_address = (state->base | parameter) + state->offset;
        break;
    case PSP_GE_COMMAND_IADDR:
        state->index_address = (state->base | parameter) + state->offset;
        break;
    case PSP_GE_COMMAND_VTYPE:
        state->vertex_type = parameter;
        break;
    case PSP_GE_COMMAND_PRIM:
        state->last_primitive = parameter;
        break;
    case PSP_GE_COMMAND_FINISH:
        ++state->finish_count;
        break;
    case PSP_GE_COMMAND_SIGNAL:
        ++state->signal_count;
        break;
    default:
        recognized = false;
        ++state->unknown_commands;
        break;
    }
    if (result != NULL) {
        result->opcode = opcode;
        result->parameter = parameter;
        result->recognized = recognized ? 1u : 0u;
    }
    return recognized;
}
