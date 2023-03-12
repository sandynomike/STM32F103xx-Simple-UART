//  Minimalist UART
//  Mike Shegedin, 03/2023
//
//  Set up the USART for UART (RS-232) IO with character RxTx capability.
//  Initiall output "Hello World" with first 'H' output as a character and
//  'ello World!" output as a string. Then echo any terminal input back to
//  the terminal. Indicate <Return> by displaying '<RETURN>'.
//
//  Since possibly using USART1 for programming purposes, this implementation uses USART2.
//
//  USART2_Tx = PA2, Alternate function output, push-pull, 2 MHz
//  USART2_Rx = PA3, Input, floating
//
//  f(CK) = 8 MHz
//  Mantissa = whole part of f(CK) / (16 * Baud)
//  Fraction = remainder of above * 16
//      f(CK)    Baud     Mantissa   Fraction
//      -----   -------   --------   --------
//      8 MHz     9,600      52          1
//              115,200       4          5
//              460,800       1          1
//              500,000       1          0
//
//  If calculating in program:
//      uartDiv  = freqClk / baud
//      mantissa = uartDiv / 16
//      fraction = uartDiv % 16


//  Steps to Set Up UART:
//  1. Enable GPIO Port A
//  2. Enable USART2 perpheral
//  3. Set Baudrate
//  4. Enable (turn on) Tx, Rx, and USART
//  5. Set CNF[1:0] and MODE[1:0] bits for Pin A2 to function as Alt. output w/push-pull, 2 MHz.
//     Note that the Rx pin, A3 is set to the desired state of floating input by default.
#include "stm32f103xb.h"

int
putchar(int c)
{
    while( !(USART2->SR & USART_SR_TXE ) )
    {   // Wait until the transmit data register is empty
    }
    USART2->DR = c; // Put character into the data register.
    return c;
}

int
puts( const char *s )
{
    while( *s )
        putchar( *s++ );
     putchar('\n');
     return 1;
}

int
getchar()
{
    while( !( USART2->SR & USART_SR_RXNE ) )
    {
    }
    return USART2->DR;
}

int
main( void )
{
    int c; // Will be character to echo

    // Enable GPIO Port A
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;

    // Enable USART2 peripheral
    RCC->APB1ENR |= RCC_APB1ENR_USART2EN;

    // Set Baudrate by loading the baudrate Mantissa and Fractional part as described above
    USART2->BRR = (4<<USART_BRR_DIV_Mantissa_Pos) | (5<<USART_BRR_DIV_Fraction_Pos);

    // Enable (turn on) Tx, Rx, and USART
    USART2->CR1 = (USART_CR1_TE | USART_CR1_RE | USART_CR1_UE) ;

    // Clear CNF2[1:0]
    GPIOA->CRL &= ~GPIO_CRL_CNF2_Msk;

    // Set  CNF2[1:0] to 0b10 (Alt output w/push-pull) and
    //     MODE2[1:0] to 0b01 (Output, 2 MHz)
    GPIOA->CRL |=  GPIO_CRL_CNF2_1 | GPIO_CRL_MODE2_0;


    putchar('H');           // Output one character 'H' to terminal
    puts("ello World!");    // Output rest of 'ellow World!'

    puts("Now type stuff on the terminal to be echoed...");

    while( 1 )                // Endless looop to echo anything typed into the termainal
    {                         // back out to the terminal. Echo '<RETURN>' at each CR character.
        c = getchar();
        putchar(c);
        if( c == '\r' )
            puts("<RETURN>");
    }

} // End main.c
