#include "psp_services.h"

#include <string.h>

void psp_services_init(PspServices *s)
{
    if (s != NULL) {
        memset(s, 0, sizeof(*s));
        s->next_thread_id = 1u;
    }
}

bool psp_services_register_syscall(PspServices *s, uint32_t nid,
                                   PspSyscallHandler handler, void *user)
{
    size_t i;
    if (s == NULL || handler == NULL) return false;
    for (i = 0; i < s->syscall_count; ++i) {
        if (s->syscalls[i].nid == nid) {
            s->syscalls[i].handler = handler;
            s->syscalls[i].user = user;
            return true;
        }
    }
    if (s->syscall_count >= PSP_SERVICE_MAX_SYSCALLS) return false;
    s->syscalls[s->syscall_count].nid = nid;
    s->syscalls[s->syscall_count].handler = handler;
    s->syscalls[s->syscall_count].user = user;
    ++s->syscall_count;
    return true;
}

bool psp_services_dispatch_syscall(PspServices *s, uint32_t nid,
                                   uint32_t a0, uint32_t a1,
                                   uint32_t a2, uint32_t a3, int32_t *result)
{
    size_t i;
    if (s == NULL || result == NULL) return false;
    for (i = 0; i < s->syscall_count; ++i) {
        if (s->syscalls[i].nid == nid) {
            *result = s->syscalls[i].handler(a0, a1, a2, a3,
                                             s->syscalls[i].user);
            return true;
        }
    }
    return false;
}

bool psp_services_add_thread(PspServices *s, PspThreadEntry entry,
                             void *user, uint32_t *thread_id)
{
    PspThread *t;
    if (s == NULL || entry == NULL ||
        s->thread_count >= PSP_SERVICE_MAX_THREADS) return false;
    t = &s->threads[s->thread_count++];
    t->id = s->next_thread_id++;
    t->entry = entry;
    t->user = user;
    t->active = 1u;
    t->ran = 0u;
    if (thread_id != NULL) *thread_id = t->id;
    return true;
}

bool psp_services_schedule_one(PspServices *s)
{
    size_t checked;
    if (s == NULL || s->thread_count == 0u) return false;
    for (checked = 0; checked < s->thread_count; ++checked) {
        size_t index = s->next_thread % s->thread_count;
        PspThread *t = &s->threads[index];
        s->next_thread = (index + 1u) % s->thread_count;
        if (t->active && !t->ran) {
            t->ran = 1u;
            t->entry(t->user);
            return true;
        }
    }
    return false;
}

void psp_services_tick(PspServices *s, uint32_t elapsed_ticks)
{
    if (s != NULL) s->ticks += elapsed_ticks;
}

void psp_services_raise_irq(PspServices *s, uint32_t mask)
{
    if (s != NULL) s->irq_pending |= mask;
}

uint32_t psp_services_take_irqs(PspServices *s, uint32_t mask)
{
    uint32_t result;
    if (s == NULL) return 0u;
    result = s->irq_pending & mask;
    s->irq_pending &= ~result;
    return result;
}

void psp_services_set_input(PspServices *s, const PspInputState *input)
{
    if (s != NULL && input != NULL) s->input = *input;
}

bool psp_services_ge_push(PspServices *s, uint32_t command)
{
    size_t tail;
    if (s == NULL || s->ge_count >= PSP_SERVICE_MAX_GE_COMMANDS) return false;
    tail = (s->ge_read + s->ge_count) % PSP_SERVICE_MAX_GE_COMMANDS;
    s->ge_commands[tail] = command;
    ++s->ge_count;
    return true;
}

bool psp_services_ge_pop(PspServices *s, uint32_t *command)
{
    if (s == NULL || command == NULL || s->ge_count == 0u) return false;
    *command = s->ge_commands[s->ge_read];
    s->ge_read = (s->ge_read + 1u) % PSP_SERVICE_MAX_GE_COMMANDS;
    --s->ge_count;
    return true;
}

bool psp_services_audio_push(PspServices *s, int16_t sample)
{
    size_t tail;
    if (s == NULL || s->audio_count >= PSP_SERVICE_AUDIO_CAPACITY) return false;
    tail = (s->audio_read + s->audio_count) % PSP_SERVICE_AUDIO_CAPACITY;
    s->audio[tail] = sample;
    ++s->audio_count;
    return true;
}

bool psp_services_audio_pop(PspServices *s, int16_t *sample)
{
    if (s == NULL || sample == NULL || s->audio_count == 0u) return false;
    *sample = s->audio[s->audio_read];
    s->audio_read = (s->audio_read + 1u) % PSP_SERVICE_AUDIO_CAPACITY;
    --s->audio_count;
    return true;
}

bool psp_services_save(const PspServices *s, void *buffer,
                       size_t capacity, size_t *written)
{
    PspServicesSnapshot snapshot;
    if (written != NULL) *written = 0u;
    if (s == NULL || buffer == NULL || capacity < sizeof(snapshot)) return false;
    memset(&snapshot, 0, sizeof(snapshot));
    snapshot.magic = PSP_SERVICE_STATE_MAGIC;
    snapshot.version = 1u;
    snapshot.ticks = s->ticks;
    snapshot.irq_pending = s->irq_pending;
    snapshot.input = s->input;
    snapshot.next_thread_id = s->next_thread_id;
    snapshot.thread_count = s->thread_count;
    snapshot.next_thread = s->next_thread;
    memcpy(snapshot.threads, s->threads, sizeof(snapshot.threads));
    memcpy(buffer, &snapshot, sizeof(snapshot));
    if (written != NULL) *written = sizeof(snapshot);
    return true;
}

bool psp_services_load(PspServices *s, const void *buffer, size_t size)
{
    PspServicesSnapshot snapshot;
    size_t i;
    if (s == NULL || buffer == NULL || size < sizeof(snapshot)) return false;
    memcpy(&snapshot, buffer, sizeof(snapshot));
    if (snapshot.magic != PSP_SERVICE_STATE_MAGIC || snapshot.version != 1u ||
        snapshot.thread_count > PSP_SERVICE_MAX_THREADS ||
        (snapshot.thread_count != 0u &&
         snapshot.next_thread >= snapshot.thread_count)) return false;
    for (i = 0; i < snapshot.thread_count; ++i) {
        if (snapshot.threads[i].active && snapshot.threads[i].entry == NULL)
            return false;
    }
    s->ticks = snapshot.ticks;
    s->irq_pending = snapshot.irq_pending;
    s->input = snapshot.input;
    s->next_thread_id = snapshot.next_thread_id;
    s->thread_count = snapshot.thread_count;
    s->next_thread = snapshot.next_thread;
    memcpy(s->threads, snapshot.threads, sizeof(s->threads));
    return true;
}
