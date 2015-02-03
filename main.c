#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <avr/cpufunc.h>
#include <stdlib.h>
#include <util/delay.h>
#include <string.h>
#include "usb_serial.h"
#include "light_ws2812.h"


#define LED_CONFIG	(DDRD |= (1<<6))
#define CPU_PRESCALE(n)	(CLKPR = 0x80, CLKPR = (n))
#define CPU_16MHz       0x00
#define CPU_8MHz        0x01
#define CPU_4MHz        0x02
#define CPU_2MHz        0x03
#define CPU_1MHz        0x04
#define CPU_500kHz      0x05
#define CPU_250kHz      0x06
#define CPU_125kHz      0x07
#define CPU_62kHz       0x08

#define USB_SERIAL_PRINT(a) usb_serial_write((uint8_t *)a, sizeof(a)-1)


void
usb_serial_write_u16(uint16_t i)
{
	char result[20];

	utoa(i, (char *) result, 10);
	usb_serial_write((uint8_t *)result, strlen((char *)result));
}

void
usb_serial_write_u16_with_scale(uint32_t i, char scale)
{
	uint8_t result[20];

	utoa(i, (char *) result, 10);
	size_t len = strlen((char *) result);

	if (len <= scale) {
		usb_serial_putchar('0');
		usb_serial_putchar('.');
		for (int i=0; i < scale - len; i++) {
			usb_serial_putchar('0');
		}
		usb_serial_write(result, len);
	} else {
		usb_serial_write(result, len - scale);
		usb_serial_putchar('.');
		usb_serial_write(result + len - scale, scale);
	}
}

//static volatile char hit = 0;

//ISR(TIMER1_COMPA_vect)
//{
//	hit = 1;
//}

//init_blink()
//{
//	// Set 1024x prescaler
//	TCCR1B |= (_BV(CS12) | _BV(CS10));
//	// Set compare value: 0xffff
//	OCR1A = 0xffff;
//	// Enable interrupt
//	TIMSK1 |= _BV(OCIE1A);
//}

#define INTENSITY 0x20

const struct cRGB COLOR_BLACK = { r: 0, g: 0, b: 0 };
const struct cRGB COLOR_RED = { r: INTENSITY, g: 0, b: 0 };
const struct cRGB COLOR_GREEN = { r: 0, g: INTENSITY, b: 0 };
const struct cRGB COLOR_YELLOW = { r: INTENSITY, g: INTENSITY, b: 0 };

struct cRGB current_color = { r: 0, g: 0, b: 0 };

int main(void)
{
	CPU_PRESCALE(CPU_16MHz);

	usb_init();

	ws2812_setleds(&current_color, 1);

	for (;;) {
		//if (hit == 1) {
		//	struct cRGB tmp_color = COLOR_BLACK;
		//	ws2812_setleds(&tmp_color, 1);
		//	_delay_ms(100);
		//	ws2812_setleds(&current_color, 1);
		//	hit = 0;
		//}

		uint16_t c = usb_serial_getchar();

		if (c == '0') {
			current_color = COLOR_BLACK;
			ws2812_setleds(&current_color, 1);
		} else if (c == '1') {
			current_color = COLOR_RED;
			ws2812_setleds(&current_color, 1);
		} else if (c == '2') {
			current_color = COLOR_GREEN;
			ws2812_setleds(&current_color, 1);
		} else if (c == '3') {
			current_color = COLOR_YELLOW;
			ws2812_setleds(&current_color, 1);
		}

		_delay_ms(100);

	}

	return 0;
}

