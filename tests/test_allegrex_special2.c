#include "../core/psp/allegrex_cpu.h"
#include <assert.h>
#include <stdio.h>
#define SPECIAL2(rs, rt, rd, fn) ((UINT32_C(0x1C) << 26) | ((uint32_t)(rs) << 21) | ((uint32_t)(rt) << 16) | ((uint32_t)(rd) << 11) | (uint32_t)(fn))
static void put(PspMemory *m, uint32_t a, uint32_t op) { assert(psp_memory_write32(m,a,op)); }
static void reset(AllegrexState *c,uint32_t pc) { allegrex_state_reset(c); c->pc=pc; c->next_pc=pc+4u; }
static void step_ok(AllegrexState *c,PspMemory *m) { assert(allegrex_step(c,m)==ALLEGREX_STEP_OK); }
int main(void) {
 PspMemory m; AllegrexState c; const uint32_t b=UINT32_C(0x08003000); uint64_t acc;
 assert(psp_memory_init(&m));
 reset(&c,b); c.gpr[1]=7u; c.gpr[2]=9u; put(&m,b,SPECIAL2(1,2,3,2)); step_ok(&c,&m); assert(c.gpr[3]==63u);
 reset(&c,b+0x20u); c.hi=1u; c.lo=UINT32_C(0xFFFFFFF0); c.gpr[1]=4u; c.gpr[2]=8u; put(&m,b+0x20u,SPECIAL2(1,2,0,0)); step_ok(&c,&m); acc=((uint64_t)c.hi<<32)|c.lo; assert(acc==UINT64_C(0x100000010));
 reset(&c,b+0x40u); c.lo=100u; c.gpr[1]=7u; c.gpr[2]=9u; put(&m,b+0x40u,SPECIAL2(1,2,0,4)); step_ok(&c,&m); assert(c.hi==0u && c.lo==37u);
 reset(&c,b+0x60u); c.lo=100u; c.gpr[1]=UINT32_MAX; c.gpr[2]=2u; put(&m,b+0x60u,SPECIAL2(1,2,0,5)); step_ok(&c,&m); acc=((uint64_t)c.hi<<32)|c.lo; assert(acc==UINT64_C(0xFFFFFFFFFFFFFFFE));
 reset(&c,b+0x80u); c.gpr[1]=3u; c.gpr[2]=5u; put(&m,b+0x80u,SPECIAL2(1,2,0,1)); step_ok(&c,&m); assert(c.hi==0u && c.lo==15u);
 psp_memory_destroy(&m); puts("PASS: Allegrex SPECIAL2 MUL/MADD/MADDU/MSUB/MSUBU."); return 0;
}