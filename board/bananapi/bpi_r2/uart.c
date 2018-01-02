/*
 * Copyright (c) 2008 Travis Geiselbrecht
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files
 * (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
#include <serial.h>
#include <config.h>

#include <asm/arch/mt_typedefs.h>
#include <asm/arch/mt_reg_base.h>
#include <asm/arch/mt_uart.h>
//#include <asm/arch/mt_gpio.h>
#include <asm/arch/sync_write.h>
//#include "cust_gpio_usage.h"


#define UART_SET_BITS(BS,REG)       mt65xx_reg_sync_writel(DRV_Reg32(REG) | (u32)(BS), REG)
#define UART_CLR_BITS(BS,REG)       mt65xx_reg_sync_writel(DRV_Reg32(REG) & ~((u32)(BS)), REG)

#define UART_BASE(uart)					  (uart)

#define UART_RBR(uart)                    (UART_BASE(uart)+0x0)  /* Read only */
#define UART_THR(uart)                    (UART_BASE(uart)+0x0)  /* Write only */
#define UART_IER(uart)                    (UART_BASE(uart)+0x4)
#define UART_IIR(uart)                    (UART_BASE(uart)+0x8)  /* Read only */
#define UART_FCR(uart)                    (UART_BASE(uart)+0x8)  /* Write only */
#define UART_LCR(uart)                    (UART_BASE(uart)+0xc)
#define UART_MCR(uart)                    (UART_BASE(uart)+0x10)
#define UART_LSR(uart)                    (UART_BASE(uart)+0x14)
#define UART_MSR(uart)                    (UART_BASE(uart)+0x18)
#define UART_SCR(uart)                    (UART_BASE(uart)+0x1c)
#define UART_DLL(uart)                    (UART_BASE(uart)+0x0)  /* Only when LCR.DLAB = 1 */
#define UART_DLH(uart)                    (UART_BASE(uart)+0x4)  /* Only when LCR.DLAB = 1 */
#define UART_EFR(uart)                    (UART_BASE(uart)+0x8)  /* Only when LCR = 0xbf */
#define UART_XON1(uart)                   (UART_BASE(uart)+0x10) /* Only when LCR = 0xbf */
#define UART_XON2(uart)                   (UART_BASE(uart)+0x14) /* Only when LCR = 0xbf */
#define UART_XOFF1(uart)                  (UART_BASE(uart)+0x18) /* Only when LCR = 0xbf */
#define UART_XOFF2(uart)                  (UART_BASE(uart)+0x1c) /* Only when LCR = 0xbf */
#define UART_AUTOBAUD_EN(uart)            (UART_BASE(uart)+0x20)
#define UART_HIGHSPEED(uart)              (UART_BASE(uart)+0x24)
#define UART_SAMPLE_COUNT(uart)           (UART_BASE(uart)+0x28)
#define UART_SAMPLE_POINT(uart)           (UART_BASE(uart)+0x2c)
#define UART_AUTOBAUD_REG(uart)           (UART_BASE(uart)+0x30)
#define UART_RATE_FIX_AD(uart)            (UART_BASE(uart)+0x34)
#define UART_AUTOBAUD_SAMPLE(uart)        (UART_BASE(uart)+0x38)
#define UART_GUARD(uart)                  (UART_BASE(uart)+0x3c)
#define UART_ESCAPE_DAT(uart)             (UART_BASE(uart)+0x40)
#define UART_ESCAPE_EN(uart)              (UART_BASE(uart)+0x44)
#define UART_SLEEP_EN(uart)               (UART_BASE(uart)+0x48)
#define UART_VFIFO_EN(uart)               (UART_BASE(uart)+0x4c)
#define UART_RXTRI_AD(uart)               (UART_BASE(uart)+0x50)

// output uart port
static volatile unsigned int const g_uart = UART3;

// output uart baudrate
unsigned int g_brg = CONFIG_BAUDRATE;

#define UART_SRC_CLK 26000000

void uart_setbrg(void)
{
	unsigned int byte, speed;
	unsigned int highspeed;
	unsigned int quot, divisor, remainder;
	unsigned int uartclk;
	unsigned short data, high_speed_div, sample_count, sample_point;
	unsigned int tmp_div;

	speed = g_brg;
	////FIXME Disable for MT6582 LK Porting
	uartclk = UART_SRC_CLK;
	//uartclk = (unsigned int)(mtk_get_bus_freq()*1000/4);
	if (speed <= 115200) {
		highspeed = 0;
		quot = 16;
	} else {
		highspeed = 3;
		quot = 1;
	}

	if (highspeed < 3) { /*0~2*/
		/* Set divisor DLL and DLH	*/
		divisor   =  uartclk / (quot * speed);
		remainder =  uartclk % (quot * speed);

		if (remainder >= (quot / 2) * speed)
			divisor += 1;

		mt65xx_reg_sync_writew(highspeed, UART_HIGHSPEED(g_uart));
		byte = DRV_Reg32(UART_LCR(g_uart));	  /* DLAB start */
		mt65xx_reg_sync_writel((byte | UART_LCR_DLAB), UART_LCR(g_uart));
		mt65xx_reg_sync_writel((divisor & 0x00ff), UART_DLL(g_uart));
		mt65xx_reg_sync_writel(((divisor >> 8)&0x00ff), UART_DLH(g_uart));
		mt65xx_reg_sync_writel(byte, UART_LCR(g_uart));	  /* DLAB end */
	} else {
		data = (unsigned short)(uartclk / speed);
		high_speed_div = (data >> 8) + 1; // divided by 256

		tmp_div = uartclk / (speed * high_speed_div);
		divisor = (unsigned short)tmp_div;

		remainder = (uartclk) % (high_speed_div * speed);
		/*get (sample_count+1)*/
		if (remainder >= ((speed)*(high_speed_div)) >> 1)
			divisor = (unsigned short)(tmp_div + 1);
		else
			divisor = (unsigned short)tmp_div;

		sample_count = divisor - 1;

		/*get the sample point*/
		sample_point = (sample_count - 1) >> 1;

		/*configure register*/
		mt65xx_reg_sync_writel(highspeed, UART_HIGHSPEED(g_uart));

		byte = DRV_Reg32(UART_LCR(g_uart));	   /* DLAB start */
		mt65xx_reg_sync_writel((byte | UART_LCR_DLAB), UART_LCR(g_uart));
		mt65xx_reg_sync_writel((high_speed_div & 0x00ff), UART_DLL(g_uart));
		mt65xx_reg_sync_writel(((high_speed_div >> 8)&0x00ff), UART_DLH(g_uart));
		mt65xx_reg_sync_writel(sample_count, UART_SAMPLE_COUNT(g_uart));
		mt65xx_reg_sync_writel(sample_point, UART_SAMPLE_POINT(g_uart));
		mt65xx_reg_sync_writel(byte, UART_LCR(g_uart));	  /* DLAB end */
	}
}

int uart_init_early(void)
{
	DRV_SetReg32(UART_FCR(g_uart), UART_FCR_FIFO_INIT); /* clear fifo */
	mt65xx_reg_sync_writew(UART_NONE_PARITY | UART_WLS_8 | UART_1_STOP, UART_LCR(g_uart));

	uart_setbrg();

	return 0;
}


void uart_putc(const char c)
{
	while (!(DRV_Reg32(UART_LSR(g_uart)) & UART_LSR_THRE));

	if (c == '\n')
		mt65xx_reg_sync_writel((unsigned int)'\r', UART_THR(g_uart));

	mt65xx_reg_sync_writel((unsigned int)c, UART_THR(g_uart));
}

int uart_getc(void)  /* returns -1 if no data available */
{
	while (!(DRV_Reg32(UART_LSR(g_uart)) & UART_LSR_DR));
	return (int)DRV_Reg32(UART_RBR(g_uart));
}

void uart_puts(const char *s)
{
	while (*s)
		uart_putc(*s++);
}

int uart_tstc(void)
{
	return (DRV_Reg32(UART_LSR(g_uart)) & UART_LSR_DR);
}

static struct serial_device mt7623_uart_drv = {
	.name	= "mt7623_uart",
	.start	= uart_init_early,
	.stop	= NULL,
	.setbrg	= uart_setbrg,
	.putc	= uart_putc,
	.puts	= uart_puts,
	.getc	= uart_getc,
	.tstc	= uart_tstc,
};

void pl01x_serial_initialize(void)
{
	serial_register(&mt7623_uart_drv);
}

struct serial_device *default_serial_console(void) {
	return &mt7623_uart_drv;
}

