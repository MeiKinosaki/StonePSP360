#include "../core/psp/psp_services.h"

#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

static int thread_calls;
static void thread_entry(void *user) { thread_calls += *(int *)user; }
static int32_t syscall_add(uint32_t a0, uint32_t a1, uint32_t a2,
                           uint32_t a3, void *user)
{
    return (int32_t)(a0 + a1 + a2 + a3 + (uint32_t)(uintptr_t)user);
}

int main(void)
{
    PspServices s, restored;
    PspInputState input = { 0x1234u, -1, 2, 3, -4 };
    uint32_t id, ge;
    int32_t result = 0;
    int value = 2;
    int16_t sample = 0;
    uint8_t snapshot[sizeof(PspServicesSnapshot)];
    size_t written = 0;

    psp_services_init(&s);
    assert(psp_services_register_syscall(&s, 0xABCDu, syscall_add,
                                         (void *)(uintptr_t)5u));
    assert(psp_services_dispatch_syscall(&s, 0xABCDu, 1, 2, 3, 4, &result));
    assert(result == 15);
    assert(!psp_services_dispatch_syscall(&s, 0xBADu, 0, 0, 0, 0, &result));

    thread_calls = 0;
    assert(psp_services_add_thread(&s, thread_entry, &value, &id));
    assert(id == 1u);
    assert(psp_services_schedule_one(&s));
    assert(thread_calls == 2);
    assert(!psp_services_schedule_one(&s));

    psp_services_tick(&s, 7u);
    assert(s.ticks == 7u);
    psp_services_raise_irq(&s, 0x5u);
    assert(psp_services_take_irqs(&s, 0x1u) == 0x1u);
    assert(s.irq_pending == 0x4u);
    psp_services_set_input(&s, &input);
    assert(s.input.buttons == 0x1234u && s.input.lx == -1);

    assert(psp_services_ge_push(&s, 0xDEADBEEFu));
    assert(psp_services_ge_pop(&s, &ge) && ge == 0xDEADBEEFu);
    assert(!psp_services_ge_pop(&s, &ge));
    {
        size_t i;
        for (i = 0; i < PSP_SERVICE_MAX_GE_COMMANDS; ++i)
            assert(psp_services_ge_push(&s, (uint32_t)i));
        assert(!psp_services_ge_push(&s, 999u));
        for (i = 0; i < PSP_SERVICE_MAX_GE_COMMANDS / 2u; ++i) {
            assert(psp_services_ge_pop(&s, &ge) && ge == (uint32_t)i);
        }
        for (i = 0; i < PSP_SERVICE_MAX_GE_COMMANDS / 2u; ++i)
            assert(psp_services_ge_push(&s, 1000u + (uint32_t)i));
        for (i = PSP_SERVICE_MAX_GE_COMMANDS / 2u;
             i < PSP_SERVICE_MAX_GE_COMMANDS; ++i)
            assert(psp_services_ge_pop(&s, &ge) && ge == (uint32_t)i);
        for (i = 0; i < PSP_SERVICE_MAX_GE_COMMANDS / 2u; ++i)
            assert(psp_services_ge_pop(&s, &ge) &&
                   ge == 1000u + (uint32_t)i);
        assert(!psp_services_ge_pop(&s, &ge));
    }
    assert(psp_services_audio_push(&s, -123));
    assert(psp_services_audio_pop(&s, &sample) && sample == -123);
    {
        size_t i;
        for (i = 0; i < PSP_SERVICE_AUDIO_CAPACITY; ++i)
            assert(psp_services_audio_push(&s, (int16_t)i));
        assert(!psp_services_audio_push(&s, 1));
        for (i = 0; i < PSP_SERVICE_AUDIO_CAPACITY / 2u; ++i)
            assert(psp_services_audio_pop(&s, &sample) && sample == (int16_t)i);
        for (i = 0; i < PSP_SERVICE_AUDIO_CAPACITY / 2u; ++i)
            assert(psp_services_audio_push(&s, (int16_t)(2000u + i)));
        for (i = PSP_SERVICE_AUDIO_CAPACITY / 2u;
             i < PSP_SERVICE_AUDIO_CAPACITY; ++i)
            assert(psp_services_audio_pop(&s, &sample) && sample == (int16_t)i);
        for (i = 0; i < PSP_SERVICE_AUDIO_CAPACITY / 2u; ++i)
            assert(psp_services_audio_pop(&s, &sample) &&
                   sample == (int16_t)(2000u + i));
        assert(!psp_services_audio_pop(&s, &sample));
    }

    assert(psp_services_save(&s, snapshot, sizeof(snapshot), &written));
    assert(written == sizeof(snapshot));
    psp_services_init(&restored);
    assert(psp_services_load(&restored, snapshot, written));
    assert(restored.ticks == 7u && restored.irq_pending == 0x4u);
    assert(restored.input.buttons == 0x1234u);
    snapshot[0] = 0u;
    assert(!psp_services_load(&restored, snapshot, written));
    assert(!psp_services_save(&s, snapshot, 1u, &written));

    puts("PSP services foundation tests passed");
    return 0;
}
