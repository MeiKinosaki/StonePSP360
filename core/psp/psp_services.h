#ifndef STONEPSP360_PSP_SERVICES_H
#define STONEPSP360_PSP_SERVICES_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define PSP_SERVICE_MAX_SYSCALLS 64u
#define PSP_SERVICE_MAX_THREADS 16u
#define PSP_SERVICE_MAX_GE_COMMANDS 256u
#define PSP_SERVICE_AUDIO_CAPACITY 1024u
#define PSP_SERVICE_STATE_MAGIC 0x53505331u

typedef int32_t (*PspSyscallHandler)(uint32_t a0, uint32_t a1,
                                     uint32_t a2, uint32_t a3, void *user);
typedef void (*PspThreadEntry)(void *user);

typedef struct PspSyscallSlot {
    uint32_t nid;
    PspSyscallHandler handler;
    void *user;
} PspSyscallSlot;

typedef struct PspThread {
    uint32_t id;
    PspThreadEntry entry;
    void *user;
    uint8_t active;
    uint8_t ran;
} PspThread;

typedef struct PspInputState {
    uint32_t buttons;
    int8_t lx, ly, rx, ry;
} PspInputState;

typedef struct PspServices {
    PspSyscallSlot syscalls[PSP_SERVICE_MAX_SYSCALLS];
    size_t syscall_count;
    PspThread threads[PSP_SERVICE_MAX_THREADS];
    size_t thread_count;
    size_t next_thread;
    uint32_t next_thread_id;
    uint64_t ticks;
    uint32_t irq_pending;
    PspInputState input;
    uint32_t ge_commands[PSP_SERVICE_MAX_GE_COMMANDS];
    size_t ge_read, ge_count;
    int16_t audio[PSP_SERVICE_AUDIO_CAPACITY];
    size_t audio_read, audio_count;
} PspServices;

typedef struct PspServicesSnapshot {
    uint32_t magic;
    uint32_t version;
    uint64_t ticks;
    uint32_t irq_pending;
    PspInputState input;
    uint32_t next_thread_id;
    size_t thread_count;
    size_t next_thread;
    PspThread threads[PSP_SERVICE_MAX_THREADS];
} PspServicesSnapshot;

void psp_services_init(PspServices *services);
bool psp_services_register_syscall(PspServices *services, uint32_t nid,
                                   PspSyscallHandler handler, void *user);
bool psp_services_dispatch_syscall(PspServices *services, uint32_t nid,
                                   uint32_t a0, uint32_t a1,
                                   uint32_t a2, uint32_t a3, int32_t *result);
bool psp_services_add_thread(PspServices *services, PspThreadEntry entry,
                             void *user, uint32_t *thread_id);
bool psp_services_schedule_one(PspServices *services);
void psp_services_tick(PspServices *services, uint32_t elapsed_ticks);
void psp_services_raise_irq(PspServices *services, uint32_t mask);
uint32_t psp_services_take_irqs(PspServices *services, uint32_t mask);
void psp_services_set_input(PspServices *services, const PspInputState *input);
bool psp_services_ge_push(PspServices *services, uint32_t command);
bool psp_services_ge_pop(PspServices *services, uint32_t *command);
bool psp_services_audio_push(PspServices *services, int16_t sample);
bool psp_services_audio_pop(PspServices *services, int16_t *sample);
bool psp_services_save(const PspServices *services, void *buffer,
                       size_t capacity, size_t *written);
bool psp_services_load(PspServices *services, const void *buffer, size_t size);

#endif
