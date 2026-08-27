/*
 * main.c - Standalone demo for the baby warmer display.
 *
 * "All possibilities" demo: sweeps through every face, every heater
 * power tier (to show the bar's green->yellow->red + ray count/
 * intensity across its full range), both mode icons, and every
 * fault/warning/alarm combination, so every visual element can be
 * checked on the real PCB in one loop:
 *
 *    1. Comfortable, heater idle                          (4 s)
 *    2. Comfortable, heater gentle maintain (~35%)         (4 s)
 *    3. Cold, heater ramping (~70%)                        (4 s)
 *    4. Cold, heater at full power (100%)                  (4 s)
 *    5. Warming up, approaching target (~55%)               (4 s)
 *    6. Too hot, heater off, warning + alarm                (4 s)
 *    7. Sensor fallen off, safe mode (warning blinks)       (4 s)
 *    8. Heater failure (bar forced red, no rays), alarm     (4 s)
 *    9. Manual mode, comfortable                            (4 s)
 *   10. Manual mode, cold, heating hard                     (4 s)
 *
 * Then loops back. The demo also shows that the main loop runs
 * at >1 kHz - the print "tick" is just a counter to prove it.
 */
#include <stdio.h>
#include <string.h>

#include "pico/stdlib.h"

#include "display.h"

/* Each demo step lasts this long */
#define STEP_MS 4000

typedef struct {
    const char            *name;
    warmer_display_state_t state;
} demo_step_t;

/* Build the demo sequence */
static const demo_step_t demo[] = {
    {
        .name = "1. Comfortable, heater idle",
        .state = {
            .mode = MODE_AUTO,
            .heater_on = false,
            .heater_failed = false,
            .sensor_connected = true,
            .warning = false,
            .alarm = false,
            .heater_percent = 0,
            .baby = BABY_OK,
        },
    },
    {
        .name = "2. Comfortable, gentle maintain heat",
        .state = {
            .mode = MODE_AUTO,
            .heater_on = true,
            .heater_failed = false,
            .sensor_connected = true,
            .warning = false,
            .alarm = false,
            .heater_percent = 35,
            .baby = BABY_OK,
        },
    },
    {
        .name = "3. Cold, heater ramping",
        .state = {
            .mode = MODE_AUTO,
            .heater_on = true,
            .heater_failed = false,
            .sensor_connected = true,
            .warning = false,
            .alarm = false,
            .heater_percent = 70,
            .baby = BABY_COLD,
        },
    },
    {
        .name = "4. Cold, heater at full power",
        .state = {
            .mode = MODE_AUTO,
            .heater_on = true,
            .heater_failed = false,
            .sensor_connected = true,
            .warning = false,
            .alarm = false,
            .heater_percent = 100,
            .baby = BABY_COLD,
        },
    },
    {
        .name = "5. Warming up, approaching target",
        .state = {
            .mode = MODE_AUTO,
            .heater_on = true,
            .heater_failed = false,
            .sensor_connected = true,
            .warning = false,
            .alarm = false,
            .heater_percent = 55,
            .baby = BABY_WARMING,
        },
    },
    {
        .name = "6. Too hot, heater off (alarm)",
        .state = {
            .mode = MODE_AUTO,
            .heater_on = false,
            .heater_failed = false,
            .sensor_connected = true,
            .warning = true,
            .alarm = true,
            .heater_percent = 0,
            .baby = BABY_HOT,
        },
    },
    {
        .name = "7. Sensor fallen off, safe mode",
        .state = {
            .mode = MODE_AUTO,
            .heater_on = true,
            .heater_failed = false,
            .sensor_connected = false,
            .warning = true,
            .alarm = false,
            .heater_percent = 50,
            .baby = BABY_WARMING,
        },
    },
    {
        .name = "8. Heater failure (alarm)",
        .state = {
            .mode = MODE_AUTO,
            .heater_on = false,
            .heater_failed = true,
            .sensor_connected = true,
            .warning = true,
            .alarm = true,
            .heater_percent = 0,
            .baby = BABY_COLD,
        },
    },
    {
        .name = "9. Manual mode, comfortable",
        .state = {
            .mode = MODE_MANUAL,
            .heater_on = true,
            .heater_failed = false,
            .sensor_connected = true,
            .warning = false,
            .alarm = false,
            .heater_percent = 40,
            .baby = BABY_OK,
        },
    },
    {
        .name = "10. Manual mode, cold, heating hard",
        .state = {
            .mode = MODE_MANUAL,
            .heater_on = true,
            .heater_failed = false,
            .sensor_connected = true,
            .warning = false,
            .alarm = false,
            .heater_percent = 90,
            .baby = BABY_COLD,
        },
    },
};

#define NUM_STEPS (sizeof(demo) / sizeof(demo[0]))

int main(void) {
    stdio_init_all();

    /* Tiny delay so a serial terminal can attach if attached */
    sleep_ms(500);
    printf("\n--- baby warmer display demo starting ---\n");

    display_init();

    uint32_t step_index = 0;
    uint32_t step_started_ms = to_ms_since_boot(get_absolute_time());
    uint32_t last_print_ms = step_started_ms;
    uint32_t loop_count = 0;

    /* simulate a push-button pulse on apgar_start: fires once at boot
       (birth) only, then the clock runs untouched - so the real 1/5/10
       minute checkpoints can be watched without the 10-step demo loop
       (40 s) restarting it along the way */
    bool apgar_pulse = true;

    printf("step %lu: %s\n", (unsigned long)step_index, demo[step_index].name);

    while (true) {
        uint32_t now = to_ms_since_boot(get_absolute_time());

        /* advance demo step */
        if ((uint32_t)(now - step_started_ms) >= STEP_MS) {
            step_started_ms = now;
            step_index = (step_index + 1) % NUM_STEPS;
            printf("step %lu: %s\n", (unsigned long)step_index, demo[step_index].name);
        }

        /* render current step; apgar_pulse is a one-shot trigger, the
           display owns the actual elapsed-time clock itself */
        warmer_display_state_t state = demo[step_index].state;
        state.apgar_start = apgar_pulse;
        display_update(&state, now);
        apgar_pulse = false;

        loop_count++;

        /* print loop rate every 2 seconds, useful to confirm
           the main loop is fast and not stalled by the display */
        if ((uint32_t)(now - last_print_ms) >= 2000) {
            printf("loop rate: %lu Hz\n",
                   (unsigned long)(loop_count / 2));
            loop_count = 0;
            last_print_ms = now;
        }

        /* a small sleep keeps things easy to read on a logic analyzer
           but is NOT required functionally - display_update is dirty-
           tracked and idempotent */
        sleep_ms(2);
    }
}
