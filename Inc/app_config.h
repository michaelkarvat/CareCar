/**
 * @file    app_config.h
 * @brief   Single place for every board-, timing- and policy-level constant.
 *
 * Nothing in this header depends on application code, so tuning the system
 * (thresholds, baud rates, pin assignment) never requires touching logic.
 */

#ifndef APP_CONFIG_H
#define APP_CONFIG_H

/* ------------------------------------------------------------------------
 * Clock tree
 * --------------------------------------------------------------------- */

/**
 * Frequency of the peripheral buses.
 *
 * The firmware deliberately runs on the STM32F303RE's default HSI clock
 * without touching the PLL, so SYSCLK, PCLK1 and PCLK2 are all 8 MHz.
 * UART baud dividers and the tick timer reload are derived from this value.
 */
#define APP_PCLK_HZ                     8000000UL

/* ------------------------------------------------------------------------
 * Serial links
 * --------------------------------------------------------------------- */

/** Debug console to the host PC: USART2, PA2 (TX) / PA3 (RX). */
#define DEBUG_UART_BAUD                 9600UL

/** SIMCom GSM modem: USART3, PB10 (TX) / PB11 (RX). */
#define GSM_UART_BAUD                   115200UL

/** MH-Z19 CO2 sensor: USART1, PA9 (TX) / PA10 (RX). Fixed by the sensor. */
#define CO2_UART_BAUD                   9600UL

/* ------------------------------------------------------------------------
 * Load cell wiring (both HX711 amplifiers live on GPIOB)
 * --------------------------------------------------------------------- */

#define CHILD_SEAT_HX711_SCK_PIN        0U   /**< PB0  -> PD_SCK */
#define CHILD_SEAT_HX711_DOUT_PIN       1U   /**< PB1  <- DOUT   */
#define DRIVER_SEAT_HX711_SCK_PIN       2U   /**< PB2  -> PD_SCK */
#define DRIVER_SEAT_HX711_DOUT_PIN      3U   /**< PB3  <- DOUT   */

/* ------------------------------------------------------------------------
 * Scheduling
 * --------------------------------------------------------------------- */

/** Rate at which the sensors are sampled and the safety policy re-evaluated. */
#define APP_TICK_HZ                     1UL

/* ------------------------------------------------------------------------
 * Safety policy
 * --------------------------------------------------------------------- */

/** Raw HX711 counts above which the child seat counts as occupied. */
#define CHILD_PRESENT_WEIGHT_RAW        5UL

/** Cabin temperature (deg C) above which the environment is unsafe. */
#define CABIN_TEMPERATURE_LIMIT_C       30

/** Cabin CO2 (ppm) above which the environment is unsafe. */
#define CABIN_CO2_LIMIT_PPM             600

/**
 * Consecutive ticks the danger condition must hold before alerting.
 *
 * Debounces load-cell noise and single bad sensor frames; at APP_TICK_HZ = 1
 * this is a three second confirmation window.
 */
#define ALERT_DEBOUNCE_TICKS            3U

/** How long the alert call is left ringing before the modem hangs up. */
#define ALERT_CALL_DURATION_TICKS       20U

/* ------------------------------------------------------------------------
 * Alert destination
 * --------------------------------------------------------------------- */

/** Buffer size for the alert phone number, including the '+' and terminator. */
#define PHONE_NUMBER_MAX_LEN            20

/**
 * Number dialled when no other number has been configured.
 *
 * Intentionally empty: a phone number is deployment data, not source code, and
 * must never be committed. Set it at runtime with the "setnum" command over the
 * debug console or by SMS. While it is empty the system logs the alert but does
 * not place a call.
 */
#define DEFAULT_ALERT_PHONE_NUMBER      ""

/* ------------------------------------------------------------------------
 * Simulation
 * --------------------------------------------------------------------- */

/**
 * Substitute fixed weights for the HX711 readings.
 *
 * The load cells are not fitted on the current prototype, so the default build
 * feeds constants into the safety policy in order to exercise the full
 * sample -> decide -> alert path from the CO2 and temperature sensors alone.
 * Set to 0 once the load cells are wired to read them for real.
 */
#define USE_SIMULATED_SEAT_WEIGHTS      1

#if USE_SIMULATED_SEAT_WEIGHTS
#define SIMULATED_CHILD_WEIGHT_RAW      20UL
#define SIMULATED_DRIVER_WEIGHT_RAW     20UL
#endif

#endif /* APP_CONFIG_H */
