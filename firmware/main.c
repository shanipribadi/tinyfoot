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
/*#include <inttypes.h>*/
#include <avr/io.h>
#include <avr/interrupt.h>

#define F_CPU  8000000UL
#include <util/delay.h>

#define CHECK_BOUNCE 4

/*using PORTB as i/o where PINB0 .. PINB3 is LED and PINB4 .. PINB7 is PBTN*/
#define MASK_LED 0x0f
#define MASK_PBTN 0xf0

static uint8_t PortState;
static volatile uint8_t KeyPressed;

void ioinit(void)
{
    DDRB = MASK_LED;
    PortState = MASK_PBTN;
    PORTB = PortState;
}

// TODO move debounce to ISR
void debounce(void)
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
    _delay_ms(1);

    // TODO should this be moved?
    KeyPressed = ~KeyClean & up & MASK_PBTN; // MASK_PBTN is unneeded?
    KeyClean |= up;
    KeyClean &= dn;
}

void toggle_led(void)
{
    if (KeyPressed == 0x00)
    {
	// Do nothing
    }
    else
    {
	PortState ^= KeyPressed >> 4; // shift right from pbtn to led.
	PORTB = PortState;
	// TODO send PortState to serial port
	KeyPressed = 0x00;
    }
}

int main(void)
{
    ioinit();
    while (1)
    {
	debounce();
	toggle_led();
    }
    return (0);
}
