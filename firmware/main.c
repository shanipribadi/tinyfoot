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

uint8_t KeyRaw[CHECK_BOUNCE];
uint8_t KeyClean;
uint8_t KeyPressed;
uint8_t Index;

void ioinit(void)
{
    DDRD = _BV(5);
    PORTD = _BV(4);
}

void debounce(void)
{
    uint8_t i, up, dn;
    KeyRaw[Index] = (~PIND & _BV(4));
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

    KeyPressed = ~KeyClean & up;
    KeyClean |= up;
    KeyClean &= dn;
}

void toggle_led(void)
{
    if ((KeyPressed & _BV(4)) == _BV(4))
    {
	PORTD ^= _BV(5);
	KeyPressed ^= KeyPressed;
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
