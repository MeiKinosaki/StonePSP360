# Build audit — StonePSP360 v8.12b

- Host tests: Allegrex reset, PSP RAM/aliases, core-memory lifecycle, CPU arithmetic/memory/HI-LO/branch delay slot.
- Additional SPECIAL instructions: MFHI/MTHI/MFLO/MTLO, MULT/MULTU, DIV/DIVU.
- Additional REGIMM branches: BLTZ, BGEZ, BLTZAL, BGEZAL.
- Division by zero is explicitly simplified: HI/LO are left unchanged; hardware-accurate behavior remains future work.
- Exceptions, COP0/VFPU, GPU, audio, loader, scheduler, and full game execution remain unimplemented.
- CI verifies host tests and an ELF32 PowerPC big-endian build. No physical Xbox 360 or game compatibility claim.
