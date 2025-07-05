// usart1.c  ——————— SMS “setnum” over +CMTI or +CMT ———————

#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "stm32f303xe.h"
#include "types.h"
#include "usart1.h"
#include "scheduler.h"    // for extern char g_phoneNumber[]

static char A_Rx_Buffer[USART1_SIZE_OF_RX_BUFFER];
static char A_Tx_Buffer[USART1_SIZE_OF_PRINT_BUFFER];
static BOOL Command_Received = FALSE;
static char *P_Loc_In_Rx_Buffer = A_Rx_Buffer;
static char * const P_End_Of_Rx_Buffer = A_Rx_Buffer + USART1_SIZE_OF_RX_BUFFER - 1;

// true when the next line is the SMS body
static BOOL smsBodyExpected = FALSE;

static void processSmsLine(const char *line)
{
    // 1) summary notification: +CMTI: "SM",<slot>
    if (strncmp(line, "+CMTI:", 6) == 0)
    {
        char *comma = strchr((char*)line, ',');
        if (comma) {
            int slot = atoi(comma + 1);
            char cmd[16];
            snprintf(cmd, sizeof(cmd), "AT+CMGR=%d\r\n", slot);
            USART1_print("%s", cmd);
            smsBodyExpected = TRUE;
        }
    }
    // 2) direct‐deliver header: +CMT: "<sender>",...
    else if (strncmp(line, "+CMT:", 5) == 0)
    {
        // no need to send AT+CMGR — the module will immediately follow
        smsBodyExpected = TRUE;
    }
    // 3) the next line is the SMS text itself
    else if (smsBodyExpected)
    {
        const char *prefix = "setnum ";
        if (strncmp(line, prefix, strlen(prefix)) == 0)
        {
            const char *digits = line + strlen(prefix);
            char formatted[PHONE_NUMBER_MAX_LEN];

            // prepend '+' if missing
            if (digits[0] == '+')
                snprintf(formatted, sizeof(formatted), "%s", digits);
            else
                snprintf(formatted, sizeof(formatted), "+%s", digits);

            // store into the global dial buffer
            strncpy(g_phoneNumber, formatted, PHONE_NUMBER_MAX_LEN - 1);
            g_phoneNumber[PHONE_NUMBER_MAX_LEN - 1] = '\0';
            print("Phone number set via SMS to %s\n", g_phoneNumber);
        }
        smsBodyExpected = FALSE;
    }
}

void USART1_init(void)
{
    // … your existing pin/baud/interrupt setup …

    // put SIM in text‐mode SMS, route new SMS directly or as index
    USART1_print("AT+CMGF=1\r\n");
    USART1_print("AT+CNMI=2,1,0,0,0\r\n");
}

BOOL USART1_commandReceived(void)
{
    if (Command_Received) {
        Command_Received = FALSE;
        return TRUE;
    }
    return FALSE;
}

void USART1_getCommand(char *p_command)
{
    strcpy(p_command, A_Rx_Buffer);
}

void USART1_print(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    int len = vsnprintf(A_Tx_Buffer, sizeof(A_Tx_Buffer), fmt, args);
    va_end(args);

    for (int i = 0; i < len; i++) {
        while (!(USART3->ISR & USART_ISR_TXE));
        USART3->TDR = A_Tx_Buffer[i];
    }
}

void USART3_EXTI28_IRQHandler(void)
{
    if (USART3->ISR & USART_ISR_RXNE)
    {
        char rx = (char)(USART3->RDR & 0xFF);
        if (rx == '\r') return;    // drop stray CR

        if (rx == '\n') {
            *P_Loc_In_Rx_Buffer = '\0';
            processSmsLine(A_Rx_Buffer);
            Command_Received = TRUE;
            P_Loc_In_Rx_Buffer = A_Rx_Buffer;
        } else {
            *P_Loc_In_Rx_Buffer++ = rx;
            if (P_Loc_In_Rx_Buffer >= P_End_Of_Rx_Buffer)
                P_Loc_In_Rx_Buffer = P_End_Of_Rx_Buffer;
        }
    }
}


//USART3_EXTI28_IRQHandler
