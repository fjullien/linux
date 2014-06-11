/*
 * 16550 compatible uart based serial debug support for zboot
 */

#include <linux/types.h>
#include <linux/serial_reg.h>

#ifndef IOTYPE
#define IOTYPE char
#endif

#define UART_BASE 0x90000000
#define PORT(offset) ((UART_BASE) + (offset))

#ifndef PORT
#error please define the serial port address for your own machine
#endif

static inline unsigned int serial_in(int offset)
{
	return *((volatile IOTYPE *)PORT(offset)) & 0xFF;
}

static inline void serial_out(int offset, int value)
{
	*((volatile IOTYPE *)PORT(offset)) = value & 0xFF;
}

void uart_putc(char c)
{
	int timeout = 1000000;

	while (((serial_in(UART_LSR) & UART_LSR_THRE) == 0) && (timeout-- > 0))
		;

	serial_out(UART_TX, c);
}

static int uart_putchar(int ch)
{
	uart_putc(ch);
	if (ch == '\n')
		uart_putc('\r');
	return ch;
}

static int uart_puts(const char *s)
{
	while (*s)
		uart_putchar(*s++);
	return 0;
}
