# Build audit — StonePSP360 v8.19b

- Implemented SPECIAL2 CLZ and CLO with a portable leading-bit count helper.
- Implemented SPECIAL MOVZ and MOVN; destination is changed only when the condition is met.
- Added a dedicated host regression test covering CLZ/CLO values, zero input, conditional writes, and immutable register zero.
- Updated GitHub Actions to run the test with C99, -Wall, -Wextra, and -Werror and label the uploaded ELF artifact v8.19b.
- Scope remains limited: no full Allegrex ISA, FPU/VFPU, COP0 exception model, GPU/GE, audio, PSP kernel/HLE, PRX decryption, commercial-game validation, or confirmed physical-console test.
- The cloud output remains ELF32 PowerPC for Xbox 360; xenon.elf is a copy of the ELF32 and is not default.xex.
