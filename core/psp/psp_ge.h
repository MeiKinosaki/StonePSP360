#ifndef STONEPSP360_PSP_GE_H
#define STONEPSP360_PSP_GE_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define PSP_GE_COMMAND_BASE 0x10u
#define PSP_GE_COMMAND_OFFSET 0x13u
#define PSP_GE_COMMAND_VADDR 0x01u
#define PSP_GE_COMMAND_IADDR 0x02u
#define PSP_GE_COMMAND_PRIM 0x04u
#define PSP_GE_COMMAND_VTYPE 0x12u
#define PSP_GE_COMMAND_FINISH 0x0Fu
#define PSP_GE_COMMAND_SIGNAL 0x0Eu

typedef struct PspGeState {
    uint32_t base;
    uint32_t offset;
    uint32_t vertex_address;
    uint32_t index_address;
    uint32_t vertex_type;
    uint32_t last_primitive;
    uint32_t finish_count;
    uint32_t signal_count;
    uint32_t unknown_commands;
} PspGeState;

typedef struct PspGeCommandResult {
    uint8_t opcode;
    uint32_t parameter;
    uint8_t recognized;
} PspGeCommandResult;

void psp_ge_init(PspGeState *state);
bool psp_ge_process_word(PspGeState *state, uint32_t word,
                         PspGeCommandResult *result);

#endif
