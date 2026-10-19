# Build audit — StonePSP360 v8.25b

- Added Allegrex SPECIAL2 MUL, MADD, MADDU, MSUB and MSUBU.
- MADD/MSUB use signed 32-bit operands and accumulate/subtract in the 64-bit HI:LO pair; unsigned variants use unsigned multiplication.
- MUL writes the low 32 bits of the signed product to rd.
- Added host tests for product and accumulator results; CI compiles with C99 -Wall -Wextra -Werror.
- Previous CPU, memory, ELF loader and PBP/ELF regression tests remain in the workflow.
- Not a complete Allegrex implementation: FPU/VFPU, full COP0 exceptions, PSP GPU/GE, audio, kernel/HLE, encrypted PRX, commercial-game compatibility and physical console execution remain unverified.
- Cloud build target is ELF32 PowerPC for Xbox 360; xenon.elf is a copy of that ELF and not default.xex.

- v8.22b adds a tested initial PSP services module: NID syscall registry/dispatch, one-pass cooperative thread scheduler, virtual ticks, pending IRQ flags, abstract input state, GE command queue, audio sample queue and a versioned services-only snapshot.
- The snapshot is not a full emulator save state: it excludes CPU and guest RAM, and must not be treated as portable across builds because thread callbacks/user pointers are process-local.
- GE/audio queues are infrastructure only; no actual PSP rendering/audio backend, complete kernel/HLE, PRX loader, encrypted PRX handling, or commercial-game compatibility is claimed.

- v8.24b adds an in-memory virtual filesystem with bounded file slots and file sizes, relative-path validation (rejecting traversal and absolute paths), partial reads, overwrite, existence/size checks and deletion.
- This VFS is volatile process memory only; it is not persistent storage, is not connected to PSP kernel file syscalls, and does not access Xbox 360 disk/USB.

- v8.24b adds a versioned little-endian VFS export/import image format. Import validates header, bounds, relative paths, duplicate names and trailing bytes before replacing the in-memory filesystem. This is not automatic disk persistence; callers must store/retrieve the exported bytes.

- v8.25b fixes a real CI regression in VFS path validation: NUL terminators must delimit path segments and not be rejected as control characters. Adds an empty-file regression test. The v8.23b workflow failure is explicitly acknowledged; the v8.25b run must pass before calling the fix verified.
