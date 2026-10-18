# Build audit — StonePSP360 v8.21b

- Added Allegrex SPECIAL2 MUL, MADD, MADDU, MSUB and MSUBU.
- MADD/MSUB use signed 32-bit operands and accumulate/subtract in the 64-bit HI:LO pair; unsigned variants use unsigned multiplication.
- MUL writes the low 32 bits of the signed product to rd.
- Added host tests for product and accumulator results; CI compiles with C99 -Wall -Wextra -Werror.
- Previous CPU, memory, ELF loader and PBP/ELF regression tests remain in the workflow.
- Not a complete Allegrex implementation: FPU/VFPU, full COP0 exceptions, PSP GPU/GE, audio, kernel/HLE, encrypted PRX, commercial-game compatibility and physical console execution remain unverified.
- Cloud build target is ELF32 PowerPC for Xbox 360; xenon.elf is a copy of that ELF and not default.xex.
