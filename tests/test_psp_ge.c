#include "../core/psp/psp_ge.h"

#include <assert.h>
#include <stdio.h>

int main(void)
{
    PspGeState s;
    PspGeCommandResult r;
    psp_ge_init(&s);
    assert(psp_ge_process_word(&s, (PSP_GE_COMMAND_BASE << 24) | 0x00120000u, &r));
    assert(r.opcode == PSP_GE_COMMAND_BASE && r.parameter == 0x00120000u);
    assert(s.base == 0x00120000u);
    assert(psp_ge_process_word(&s, (PSP_GE_COMMAND_OFFSET << 24) | 0x00000003u, &r));
    assert(s.offset == 0x300u);
    assert(psp_ge_process_word(&s, (PSP_GE_COMMAND_VADDR << 24) | 0x00000100u, &r));
    assert(s.vertex_address == 0x00120300u + 0x100u);
    assert(psp_ge_process_word(&s, (PSP_GE_COMMAND_IADDR << 24) | 0x00000080u, &r));
    assert(s.index_address == 0x00120380u);
    assert(psp_ge_process_word(&s, (PSP_GE_COMMAND_VTYPE << 24) | 0x123u, &r));
    assert(s.vertex_type == 0x123u);
    assert(psp_ge_process_word(&s, (PSP_GE_COMMAND_PRIM << 24) | 0x456u, &r));
    assert(s.last_primitive == 0x456u);
    assert(psp_ge_process_word(&s, (PSP_GE_COMMAND_FINISH << 24), &r));
    assert(s.finish_count == 1u);
    assert(psp_ge_process_word(&s, (PSP_GE_COMMAND_SIGNAL << 24), &r));
    assert(s.signal_count == 1u);
    assert(!psp_ge_process_word(&s, 0xFE000001u, &r));
    assert(r.recognized == 0u && s.unknown_commands == 1u);
    assert(!psp_ge_process_word(NULL, 0u, &r));
    puts("PSP GE command decoding tests passed");
    return 0;
}
