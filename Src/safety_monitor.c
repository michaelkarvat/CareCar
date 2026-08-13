/**
 * @file    safety_monitor.c
 * @brief   Decides whether a child has been left alone in an unsafe cabin.
 */

#include "app_config.h"
#include "debug_uart.h"
#include "hx711.h"
#include "mhz19.h"
#include "safety_monitor.h"

/** One complete observation of the cabin. */
typedef struct
{
    bool childPresent;      /**< Weight detected on the child seat. */
    bool driverPresent;     /**< Weight detected on the driver seat. */
    bool environmentUnsafe; /**< Cabin temperature or CO2 above its limit. */
} CabinState;

static const Hx711 childSeatSensor = {
    .clockPin = CHILD_SEAT_HX711_SCK_PIN,
    .dataPin = CHILD_SEAT_HX711_DOUT_PIN,
};

static const Hx711 driverSeatSensor = {
    .clockPin = DRIVER_SEAT_HX711_SCK_PIN,
    .dataPin = DRIVER_SEAT_HX711_DOUT_PIN,
};

/** Consecutive ticks the danger condition has held. */
static uint8_t dangerTicks = 0U;
static bool alertActive = false;

static uint32_t readChildSeatWeight(void)
{
#if USE_SIMULATED_SEAT_WEIGHTS
    return SIMULATED_CHILD_WEIGHT_RAW;
#else
    return Hx711_read(&childSeatSensor);
#endif
}

static uint32_t readDriverSeatWeight(void)
{
#if USE_SIMULATED_SEAT_WEIGHTS
    return SIMULATED_DRIVER_WEIGHT_RAW;
#else
    return Hx711_read(&driverSeatSensor);
#endif
}

static CabinState sampleCabin(void)
{
    CabinState state;

    const uint32_t childWeight = readChildSeatWeight();
    const uint32_t driverWeight = readDriverSeatWeight();

    state.childPresent = childWeight >= CHILD_PRESENT_WEIGHT_RAW;
    state.driverPresent = driverWeight > 0UL;

    Mhz19_Reading air;
    if (Mhz19_read(&air))
    {
        state.environmentUnsafe = (air.temperatureC > CABIN_TEMPERATURE_LIMIT_C) ||
                                  (air.co2Ppm > CABIN_CO2_LIMIT_PPM);
        DebugUart_printf("weight child=%lu driver=%lu | CO2=%d ppm temp=%d C\n",
                         childWeight, driverWeight, air.co2Ppm, air.temperatureC);
    }
    else
    {
        /* A failed read tells us nothing about the cabin, so it must not be
         * allowed to raise an alarm on its own. The child-alone condition
         * below can still trigger without it. */
        state.environmentUnsafe = false;
        DebugUart_printf("weight child=%lu driver=%lu | CO2 read failed\n",
                         childWeight, driverWeight);
    }

    return state;
}

/**
 * A child is in danger if the seat is occupied and either the driver has left
 * the vehicle or the cabin itself has become unsafe.
 */
static bool isDangerous(const CabinState *state)
{
    return state->childPresent &&
           (!state->driverPresent || state->environmentUnsafe);
}

void SafetyMonitor_init(void)
{
    Hx711_init(&childSeatSensor);
    Hx711_init(&driverSeatSensor);
    Mhz19_init();
}

void SafetyMonitor_update(void)
{
    const CabinState state = sampleCabin();

    if (!isDangerous(&state))
    {
        if (dangerTicks > 0U)
        {
            DebugUart_printf("Danger condition cleared\n");
        }
        dangerTicks = 0U;
        alertActive = false;
        return;
    }

    if (dangerTicks < ALERT_DEBOUNCE_TICKS)
    {
        dangerTicks++;
        DebugUart_printf("Danger condition held for %u/%u ticks\n",
                         (unsigned)dangerTicks, (unsigned)ALERT_DEBOUNCE_TICKS);
    }

    if ((dangerTicks >= ALERT_DEBOUNCE_TICKS) && !alertActive)
    {
        alertActive = true;
        DebugUart_printf("** Alert condition confirmed **\n");
    }
}

bool SafetyMonitor_isAlertActive(void)
{
    return alertActive;
}

void SafetyMonitor_reset(void)
{
    dangerTicks = 0U;
    alertActive = false;
}
