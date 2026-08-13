/**
 * @file    alert_contact.h
 * @brief   Owns the phone number the system calls when it raises an alert.
 *
 * The number is deployment data rather than source code, so it is stored in
 * one place, normalised in one place, and set at runtime over either of the
 * two command transports (debug console or SMS).
 */

#ifndef ALERT_CONTACT_H
#define ALERT_CONTACT_H

#include <stdbool.h>

/** Load the compile-time default from app_config.h. */
void AlertContact_init(void);

/**
 * Store a new destination number, normalising it to international form.
 *
 * A leading '+' is added when the caller omits it, so "972500000000" and
 * "+972500000000" are accepted interchangeably.
 *
 * @param number NUL-terminated digits, with or without a leading '+'.
 * @return false if the number is empty or too long to store; the previous
 *         number is left untouched in that case.
 */
bool AlertContact_set(const char *number);

/**
 * @return The current number, or an empty string if none is configured.
 */
const char *AlertContact_get(void);

/**
 * @return true if a number has been configured and a call can be placed.
 */
bool AlertContact_isConfigured(void);

#endif /* ALERT_CONTACT_H */
