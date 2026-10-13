# Build audit — StonePSP360 v8.13b

- Host tests: Allegrex reset, PSP RAM/aliases, core-memory lifecycle, CPU arithmetic/memory/HI-LO/branch delay slot and bounded run loop.
- New API: allegrex_run(state, memory, max_steps, executed_steps).
- Run loop stops on the first instruction error, reports successful instruction count, and supports a zero-step no-op.
- CPU still lacks full exception/COP0/VFPU behavior, PSP loader, scheduler integration, graphics/audio, and game compatibility validation.
- CI verifies host tests and an ELF32 PowerPC big-endian cloud build. No physical Xbox 360 test claim.
