#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include "stm32f303xe.h"
#include "types.h"
#include "usart2.h"




// This buffer is used to receive commands.
static char A_Rx_Buffer[USART2_SIZE_OF_RX_BUFFER];
// This buffer is used by the printf-like print function.
static char A_Tx_Buffer[USART2_SIZE_OF_PRINT_BUFFER];
static BOOL Command_Received;
static char *P_Loc_In_Rx_Buffer = A_Rx_Buffer;
// End of RX buffer, to make sure there's no overflow.
// This is set to point at the last character of the buffer,
// so that eventually, when a \n is entered by the user,
// it goes into the last character of the buffer.
static char * const P_End_Of_Rx_Buffer = A_Tx_Buffer + USART2_SIZE_OF_RX_BUFFER - 1;




void print(char *p_format, ...)
{
	va_list p_variables;
	va_start(p_variables, p_format);
	(void)vsprintf(A_Tx_Buffer, p_format, p_variables);
    USART2_print(A_Tx_Buffer);
}




BOOL USART2_commandReceived(void)
{
    if(Command_Received)
    {
    	Command_Received = FALSE;
    	return TRUE;
    }
    else
    {
    	return FALSE;
    }
}




void USART2_getCommand(char *p_command)
{
	(void)strcpy(p_command, A_Rx_Buffer);
}




void USART2_init(void)
{
    // Enable GPIOA clock (p. 148 in the datasheet).
    RCC->AHBENR |= 0x00020000;
    // Enable USART2 clock.
    RCC->APB1ENR |= 0x00020000;
    // Configure GPIOA pins 2 and 3 as alternate function 7, which is USART2.
    GPIOA->MODER |= 0x000000A0;
    // AFR[0] is the same as AFRL in the reference manual (p. 241),
    // and AFR[1] is the same as AFRH in the reference manual (p. 242).
    GPIOA->AFR[0] |= 0x00007700;
    USART2->BRR = 833;  // 8 MHz / 9600 baud rate.
    // Enable USART2 and its RX and TX functionality.
    // Also enable the RX interrupt.
    USART2->CR1 = 0x0000002D;
    NVIC_EnableIRQ(USART2_IRQn);
}




void USART2_EXTI26_IRQHandler(void)
{
	char rx_byte;

	// Read the received byte into the buffer.
	// This also clears the interrupt request flag.
    rx_byte = USART2->RDR;

    // If the user entered '\n', a whole command has been received.
    if(rx_byte == '\n')
    {
    	Command_Received = TRUE;
    	*P_Loc_In_Rx_Buffer = '\0';
    	P_Loc_In_Rx_Buffer = A_Rx_Buffer;
    	return;
    }

    *P_Loc_In_Rx_Buffer = rx_byte;
    // If the buffer is full, we don't advance the pointer.
    if(P_Loc_In_Rx_Buffer >= P_End_Of_Rx_Buffer)
    {
    	return;
    }
    P_Loc_In_Rx_Buffer++;
}




void USART2_printCharacter(char c)
{
    USART2->TDR = c;
    while(!(USART2->ISR & 0x00000080)); // The TXE bit is bit 7 in the register ISR.
}




void USART2_print(const char *p_data)
{
	while(*p_data != '\0')
	{
		USART2->TDR = *p_data;
        p_data++;
        while(!(USART2->ISR & 0x00000080));
	}
}
