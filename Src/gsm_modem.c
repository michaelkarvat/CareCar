/**
 * @file    gsm_modem.c
 * @brief   AT command layer for the SIMCom GSM modem.
 */

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "debug_uart.h"
#include "gsm_modem.h"
#include "gsm_uart.h"
#include "terminal.h"

/** Unsolicited result code: a new SMS was stored, giving its memory slot. */
#define URC_SMS_STORED          "+CMTI:"

/** Unsolicited result code: an SMS is being delivered inline, header first. */
#define URC_SMS_DELIVERED       "+CMT:"

/**
 * True while the next line received is expected to be an SMS body.
 *
 * The modem sends the message header and the message text as two separate
 * lines, so recognising the body requires remembering that a header just
 * arrived. Written from interrupt context only.
 */
static volatile bool smsBodyExpected = false;

static bool startsWith(const char *line, const char *prefix)
{
    return strncmp(line, prefix, strlen(prefix)) == 0;
}

/**
 * Handle one line of modem output. Runs in interrupt context.
 *
 * Only SMS-related lines are acted on; everything else is left to the main
 * loop's log echo.
 */
static void onModemLine(const char *line)
{
    if (startsWith(line, URC_SMS_STORED))
    {
        /* +CMTI: "SM",<slot> — the body has to be fetched explicitly. */
        const char *slotField = strchr(line, ',');
        if (slotField != NULL)
        {
            GsmUart_printf("AT+CMGR=%d\r\n", atoi(slotField + 1));
            smsBodyExpected = true;
        }
        return;
    }

    if (startsWith(line, URC_SMS_DELIVERED))
    {
        /* +CMT: <sender>,... — the body follows immediately, no fetch needed. */
        smsBodyExpected = true;
        return;
    }

    if (smsBodyExpected)
    {
        smsBodyExpected = false;
        (void)Terminal_executeCommand(line);
    }
}

void GsmModem_init(void)
{
    GsmUart_init(onModemLine);

    GsmUart_printf("AT+CMGF=1\r\n");            /* SMS in text mode, not PDU. */
    GsmUart_printf("AT+CNMI=2,1,0,0,0\r\n");    /* Notify on new SMS arrival. */
    GsmUart_printf("AT\r\n");                   /* Liveness check. */
    GsmUart_printf("AT+COPS=?\r\n");            /* Log visible networks. */
}

void GsmModem_placeCall(const char *number)
{
    /* A bare "AT" first: the modem may have dropped into low power since the
     * last command, and the first byte after that is used only to wake it. */
    GsmUart_printf("AT\r\n");
    GsmUart_printf("ATD%s;\r\n", number);
}

void GsmModem_hangUp(void)
{
    GsmUart_printf("AT+CHUP\r\n");
}

void GsmModem_pollLog(void)
{
    char line[GSM_UART_RX_BUFFER_SIZE];

    if (GsmUart_takeLine(line))
    {
        DebugUart_printf("modem => %s\n", line);
    }
}
