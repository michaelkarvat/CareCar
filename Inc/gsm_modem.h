/**
 * @file    gsm_modem.h
 * @brief   AT command layer for the SIMCom GSM modem.
 *
 * Wraps the two things the application needs from the modem — placing the
 * alert call, and receiving remote configuration by SMS — behind an interface
 * that contains no AT syntax.
 */

#ifndef GSM_MODEM_H
#define GSM_MODEM_H

/**
 * Bring up the modem link and put it into text-mode SMS with unsolicited
 * new-message notifications enabled.
 */
void GsmModem_init(void);

/**
 * Dial a number in voice mode. The call is not answered by this system; the
 * ring itself is the alert.
 *
 * @param number Destination in international form, e.g. "+972500000000".
 */
void GsmModem_placeCall(const char *number);

/** Terminate any call in progress. */
void GsmModem_hangUp(void);

/** Echo any pending modem output to the debug console. Call from the main loop. */
void GsmModem_pollLog(void);

#endif /* GSM_MODEM_H */
