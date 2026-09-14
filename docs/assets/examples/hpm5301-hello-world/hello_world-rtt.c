/*
 * Copyright (c) 2021 HPMicro
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include <math.h>
#ifdef HPM_DOC_RTT
#include "SEGGER_RTT.h"
#endif
#include <stdint.h>
#include "board.h"
#include "hpm_clock_drv.h"

#define WAVE_CHANNELS 16U
#define WAVE_UPDATE_MS 1U
#define WAVE_FREQUENCY_HZ 1.0f
#define WAVE_AMPLITUDE 100.0f
#define TWO_PI 6.28318530718f

/* One contiguous 64-byte region; each element is a float32 channel.
 * Noncacheable placement makes writes visible to the running JTAG reader.
 * Multiword debug reads are sequential, not atomic snapshots. */
ATTR_PLACE_AT_NONCACHEABLE_BSS_WITH_ALIGNMENT(4) volatile float superwatch_wave[WAVE_CHANNELS];
ATTR_PLACE_AT_NONCACHEABLE_BSS_WITH_ALIGNMENT(4) volatile uint32_t superwatch_tick;
ATTR_PLACE_AT_NONCACHEABLE_BSS_WITH_ALIGNMENT(4) volatile uint32_t superwatch_build_id;
ATTR_PLACE_AT_NONCACHEABLE_BSS_WITH_ALIGNMENT(4) volatile uint32_t superwatch_cpu_hz;
ATTR_PLACE_AT_NONCACHEABLE_BSS_WITH_ALIGNMENT(4) volatile uint32_t benchmark_words[1024];

static void update_waveforms(void)
{
    uint32_t tick = superwatch_tick;
    float phase = TWO_PI * WAVE_FREQUENCY_HZ * (float)(tick % 1000U) / 1000.0f;
    for (uint32_t channel = 0; channel < WAVE_CHANNELS; ++channel) {
        float offset = TWO_PI * (float)channel / (float)WAVE_CHANNELS;
        superwatch_wave[channel] = WAVE_AMPLITUDE * sinf(phase + offset);
    }
    superwatch_tick = tick + 1U;
    if (superwatch_tick % 300U == 0U) {
        board_led_toggle();
    }
}

int main(void)
{
    board_init();
    board_init_led_pins();
    superwatch_build_id = 0x20260912U;
#ifdef HPM_DOC_RTT
    superwatch_build_id = 0x20260913U;
    SEGGER_RTT_Init();
    SEGGER_RTT_WriteString(0, "HPM5301 hello_world RTT ready\r\n");
    uint32_t last_report = 0;
#endif
    superwatch_cpu_hz = clock_get_frequency(clock_cpu0);
    /* Deterministic data for the 4 KiB throughput/control test. */
    for (uint32_t i = 0; i < 1024U; ++i) {
        benchmark_words[i] = 0x48504D00U ^ i;
    }
    update_waveforms();
    /* Hardware timer: 1 kHz updates, 1 Hz wave. Probe rate is independent. */
    board_timer_create(WAVE_UPDATE_MS, update_waveforms);
    while (1) {
#ifdef HPM_DOC_RTT
        char input[16];
        unsigned count = SEGGER_RTT_Read(0, input, sizeof(input));
        if (count != 0U) {
            SEGGER_RTT_WriteString(0, "ECHO:");
            SEGGER_RTT_Write(0, input, count);
            SEGGER_RTT_WriteString(0, "\r\n");
        }
        if ((uint32_t)(superwatch_tick - last_report) >= 1000U) {
            last_report = superwatch_tick;
            SEGGER_RTT_printf(0, "HPM5301 tick=%u cpu=%u build=%x\r\n",
                (unsigned)superwatch_tick, (unsigned)superwatch_cpu_hz,
                (unsigned)superwatch_build_id);
        }
#endif
        __asm volatile ("wfi");
    }
}
