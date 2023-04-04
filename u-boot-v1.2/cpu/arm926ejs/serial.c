	/*
	 * (C) Copyright 2002
	 * Gary Jennejohn, DENX Software Engineering, <gj@denx.de>
	 *
	 * This program is free software; you can redistribute it and/or modify
	 * it under the terms of the GNU General Public License as published by
	 * the Free Software Foundation; either version 2 of the License, or
	 * (at your option) any later version.
	 *
	 * This program is distributed in the hope that it will be useful,
	 * but WITHOUT ANY WARRANTY; without even the implied warranty of
	 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	 * GNU General Public License for more details.
	 *
	 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#include <common.h>
#if defined(CONFIG_S3C2400) || defined(CONFIG_TRAB)
#include <s3c2400.h>
#elif defined(CONFIG_S3C2410)
#include <s3c2410.h>
#endif

#ifdef CONFIG_SERIAL1
#define UART_NR	S3C24X0_UART0

#elif defined(CONFIG_SERIAL2)
# if defined(CONFIG_TRAB)
#  error "TRAB supports only CONFIG_SERIAL1"
# endif
#define UART_NR	S3C24X0_UART1

#elif defined(CONFIG_SERIAL3)
# if defined(CONFIG_TRAB)
#  #error "TRAB supports only CONFIG_SERIAL1"
# endif
#define UART_NR	S3C24X0_UART2

#else
#error "Bad: you didn't configure serial ..."
#endif


#define inl(x) (*((volatile unsigned long *)x))


void serial_setbrg (void)
{

}



/*
 * Initialise the serial port with the given baudrate. The settings
 * are always 8 data bits, no parity, 1 stop bit, no start bits.
 *
 */
int serial_init (void)
{
	serial_setbrg ();

	return (0);
}

/*
 * Read a single byte from the serial port. Returns 1 on success, 0
 * otherwise. When the function is succesfull, the character read is
 * written into its argument c.
 */
int serial_getc (void)
{
	unsigned long status;	
	
	if (((status = inl(HW_UART4_STAT)) & 0x01000000) ==0 ){	
	   /* *data =	inl(UART1_DATA);*/
		return inl(HW_UART4_DATA);
		}
	else return 0;
/*
 unsigned long status;
while (((status = inl(UART1_SATA)) & 0x01000000) !=0 );
return (inl(UART1_DATA)&0xff);*/

}


/*
 * Read a single byte from the serial port. Returns 1 on success, 0
 * otherwise. When the function is succesfull, the character read is
 * written into its argument c.
 */
int getc_r (uchar *data)
{
	unsigned long status;	
	
	if (((status = *((volatile unsigned long *)HW_UART4_STAT)) & 0x01000000) ==0 ){	
	    *data =	*((volatile unsigned long *)HW_UART4_DATA);
		return 1;
		}
	else return 0;
}


uchar getc_w()
{
	unsigned long status;	
	uchar data;
	while (((status = *((volatile unsigned long *)HW_UART4_STAT)) & 0x01000000) !=0 );    	
	 data =	*((volatile unsigned long *)HW_UART4_DATA);
     return data;

}

#ifdef CONFIG_HWFLOW
static int hwflow = 0; /* turned off by default */
int hwflow_onoff(int on)
{
	switch(on) {
	case 0:
	default:
		break; /* return current */
	case 1:
		hwflow = 1; /* turn on */
		break;
	case -1:
		hwflow = 0; /* turn off */
		break;
	}
	return hwflow;
}
#endif

#ifdef CONFIG_MODEM_SUPPORT
static int be_quiet = 0;
void disable_putc(void)
{
	be_quiet = 1;
}

void enable_putc(void)
{
	be_quiet = 0;
}
#endif


/*
 * Output a single byte to the serial port.
 */
void serial_putc (const char c)
{
	unsigned long status;	
	while (((status = inl(HW_UART4_STAT)) & 0x08000000) ==0)	;
		outl(c,HW_UART4_DATA);	

    	/* If \n, also do \r */
	if (c == '\n')
		serial_putc ('\r');
}

/*
 * Test whether a character is in the RX buffer
 */
int serial_tstc (void)
{

	return ((inl(HW_UART4_STAT) & 0x01000000) ==0 );
}

void
serial_puts (const char *s)
{
	while (*s) {
		serial_putc (*s++);
	}
}





