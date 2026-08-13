/**
 * @file    alert_contact.c
 * @brief   Owns the phone number the system calls when it raises an alert.
 */

#include <stdio.h>
#include <string.h>

#include "alert_contact.h"
#include "app_config.h"

static char phoneNumber[PHONE_NUMBER_MAX_LEN];

void AlertContact_init(void)
{
    (void)AlertContact_set(DEFAULT_ALERT_PHONE_NUMBER);
}

bool AlertContact_set(const char *number)
{
    if ((number == NULL) || (number[0] == '\0'))
    {
        return false;
    }

    char normalised[PHONE_NUMBER_MAX_LEN];
    const char *prefix = (number[0] == '+') ? "" : "+";
    const int length = snprintf(normalised, sizeof(normalised), "%s%s", prefix, number);

    /* snprintf truncates silently, so reject anything that did not fit whole:
     * a half-copied phone number would dial the wrong person. */
    if ((length <= 0) || (length >= (int)sizeof(normalised)))
    {
        return false;
    }

    (void)strcpy(phoneNumber, normalised);
    return true;
}

const char *AlertContact_get(void)
{
    return phoneNumber;
}

bool AlertContact_isConfigured(void)
{
    return phoneNumber[0] != '\0';
}
