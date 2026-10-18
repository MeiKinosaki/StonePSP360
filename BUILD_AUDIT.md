# Build audit — StonePSP360 v8.20b

- Retained the v8.19b implementation of Allegrex SPECIAL2 CLZ/CLO and SPECIAL MOVZ/MOVN.
- Strengthened MOVZ/MOVN regression tests so false conditions prove that a nonmatching destination sentinel is preserved.
- Expanded integer ALU tests for AND, OR, XOR, NOR, signed/unsigned less-than, arithmetic right shift, ADDIU negative immediate, and ANDI zero extension.
- GitHub Actions compiles and runs the arithmetic regression suite under C99 with -Wall -Wextra -Werror.
- Scope remains limited: not a complete Allegrex ISA; no complete FPU/VFPU, COP0 exception model, PSP GPU/GE, audio, kernel/HLE, PRX decryption, commercial-game validation, or confirmed physical-console test.
- Cloud output remains ELF32 PowerPC for Xbox 360; xenon.elf is a copy of the ELF32 and is not default.xex.
