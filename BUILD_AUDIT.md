# Build audit — StonePSP360 v8.14b

- CPU execution is now reachable through the core API: psp_core_start(entry_pc), psp_core_run_steps(max_steps, executed_steps).
- Core integration test writes guest instructions into PSP RAM and executes them through the public core interface.
- CPU errors map to explicit core statuses for illegal instruction and memory fault.
- psp_core_run_frame intentionally remains NOT_IMPLEMENTED: scheduler, frame timing and graphics are absent.
- Host tests cover CPU reset, PSP RAM aliases, core CPU integration, and bounded instruction execution.
- Cloud CI verifies ELF32 PowerPC big-endian build. No physical Xbox 360 test or PSP game compatibility is claimed.
