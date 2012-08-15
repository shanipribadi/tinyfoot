/*
 * =====================================================================================
 *
 *       Filename:  main.c
 *
 *    Description:  TinyFoot main.c
 *
 *        Version:  1.0
 *        Created:  08/12/2012 12:21:38 AM
 *       Revision:  none
 *       Compiler:  avr-gcc -mccu=attiny2313
 *
 *         Author:  Shani Hadiyanto
 *   Organization:  
 *
 * =====================================================================================
 */
#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#define F_CPU  8000000
#define BAUD 38400
#include <util/setbaud.h>

#define CHECK_BOUNCE 4

/*using PORTB as i/o where PINB0 .. PINB3 is LED and PINB4 .. PINB7 is PBTN*/
#define MASK_LED 0x0f
#define MASK_PBTN 0xf0

static uint8_t PortState;
static volatile uint8_t KeyPressed;

/*Debouncing routine using timer overflow interrupt*/
ISR(TIMER0_OVF_vect)
{
    static uint8_t KeyRaw[CHECK_BOUNCE];
    static uint8_t KeyClean;
    static uint8_t up;
    static uint8_t dn;
    static uint8_t Index;
    static uint8_t i;

    KeyRaw[Index] = (~PINB & MASK_PBTN);
    Index++;
    if (Index >= CHECK_BOUNCE)
    {
	Index = 0;
    }

    up = 0xff;
    dn = 0x00;
    for (i=0; i<CHECK_BOUNCE; i++)
    {
	up &= KeyRaw[i];
	dn |= KeyRaw[i];
    }

    // TODO should this be moved outside the ISR?
    KeyPressed = ~KeyClean & up & MASK_PBTN; // MASK_PBTN is unneeded?
    KeyClean |= up;
    KeyClean &= dn;
}

static void
init(void)
{
    /*Set IO*/
    DDRB = MASK_LED;
    PortState = MASK_PBTN;
    PORTB = PortState;

    /*Set USART*/
    UBRRH = UBRRH_VALUE;
    UBRRL = UBRRL_VALUE;
    #if USE_2X
    UCSRA |= _BV(U2X);
    #else
    UCSRA &= ~_BV(U2X);
    #endif
    UCSRC = _BV(UCSZ1) | _BV(UCSZ0);
    UCSRB = _BV(TXEN);

    /*Set Overflow Interrupt*/
    /*clock prescaler 64*/
    TCCR0B |= _BV(CS01) | _BV(CS00);
    /*Timer/Counter0 Overflow Interrupt Enable*/
    TIMSK |= _BV(TOIE0);

    sei();
}

static int
uart_putchar(char c)
{
    loop_until_bit_is_set(UCSRA, UDRE);
    UDR = c;
    return 0;
}

static void
key_check(void)
{
    if (KeyPressed == 0x00)
    {
	// Do nothing
    }
    else
    {
	cli();
	PortState ^= KeyPressed >> 4; // shift right from pbtn to led.
	PORTB = PortState;
	// TODO send PortState to serial port. Verify if this is working.
	uart_putchar(PortState);
	KeyPressed = 0x00;
	sei();
    }
}

int
main(void)
{
    init();
    while (1)
    {
	key_check();
    }
    return (0);
}
