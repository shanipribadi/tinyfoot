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
#include <inttypes.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#define F_CPU  1000000UL
#include <util/delay.h>

#define CHECK_BOUNCE 4

uint8_t KeyRaw[CHECK_BOUNCE];
uint8_t Index;
uint8_t KeyClean;
uint8_t KeyPressed;

void ioinit(void)
{
    DDRD = _BV(5);
    PORTD = _BV(4);
}

void debounce(void)
{
    static uint8_t i, up, dn;
    KeyRaw[Index] = PIND & 0x0f;
    Index++;

    up = 0xff;
    dn = 0x00;
    for (i=0; i<CHECK_BOUNCE; i++)
    {
	up &= KeyRaw[i];
	dn |= KeyRaw[i];
    }
    if (Index >= CHECK_BOUNCE);
    {
	Index = 0;
    }

    KeyPressed = ~KeyClean & up;
    KeyClean |= up;
    KeyClean &= dn;
    _delay_ms(5);
}

void toggle_led(void)
{
    if ((KeyPressed & _BV(4)) == _BV(4))
    {
	PORTD ^= _BV(5);
	KeyPressed ^= _BV(4);
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
