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
    // 1) Enable GPIOB clock (bit18)
    RCC->AHBENR  |= 0x00040000;      // RCC_AHBENR_GPIOBEN

    // 2) Enable USART3 clock on APB1 (bit18)
    RCC->APB1ENR |= 0x00040000;      // RCC_APB1ENR_USART3EN

    // 3) Configure PB10 (TX) & PB11 (RX) as AF7
    GPIOB->MODER |= (0x2 << (10 * 2)) | (0x2 << (11 * 2));
    GPIOB->AFR[1] |= (0x7 << ((10 - 8) * 4)) | (0x7 << ((11 - 8) * 4));

    // 4) Set baud rate to 115200 @ 8 MHz PCLK1 → BRR = 69
    USART3->BRR = 69;

    // 5) Enable USART3, Rx, Tx, and Rx interrupt
    //    UE=bit0, RE=bit2, TE=bit3, RXNEIE=bit5 → 0x2D
    USART3->CR1 = 0x0000002D;

    // 6) Enable NVIC interrupt for USART3 (shared with UART4)
    NVIC_EnableIRQ(USART3_IRQn);

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
