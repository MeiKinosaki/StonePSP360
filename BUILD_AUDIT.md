# Build audit — StonePSP360 v8.17b

- Added psp_core_load_image(data, size, entry_point).
- The API requires an initialized core, calls the validated PBP/ELF loader, then resets Allegrex state to the loaded entry PC.
- Host integration test loads a synthetic ELF through the public API, executes four supported instructions and verifies a RAM store.
- CI adds loader source to the core integration test and compiles with -Wall -Wextra -Werror.
- No PRX decryption/decompression, relocations, import resolution, device file I/O, scheduler, GPU/audio, commercial game support or hardware validation is claimed.
