/*
 * (C) Copyright 2000
 * Paolo Scaffardi, AIRVENT SAM s.p.a - RIMINI(ITALY), arsenio@tin.it
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <config.h>
#include <common.h>
#include <stdarg.h>
#include <malloc.h>
#include <devices.h>
#ifdef CONFIG_LOGBUFFER
#include <logbuff.h>
#endif
#if defined(CONFIG_HARD_I2C) || defined(CONFIG_SOFT_I2C)
#include <i2c.h>
#endif


list_t devlist = 0;
device_t *stdio_devices[] = { NULL, NULL, NULL };
char *stdio_names[MAX_FILES] = { "stdin", "stdout", "stderr" };



#if defined(CONFIG_SPLASH_SCREEN) && !defined(CFG_DEVICE_NULLDEV)
#define	CFG_DEVICE_NULLDEV	1
#endif


#ifdef CFG_DEVICE_NULLDEV
void nulldev_putc(const char c)
{
  /* nulldev is empty! */
}

void nulldev_puts(const char *s)
{
  /* nulldev is empty! */
}

int nulldev_input(void)
{
  /* nulldev is empty! */
  return 0;
}
#endif


#define outb(v,p) *(volatile u8 *) (p) = v
#define outw(v,p) *(volatile u16 *) (p) = v
#define outl(v,p) *(volatile u32 *) (p) = v

#define inb(p)	*(volatile u8 *) (p)
#define inw(p)	*(volatile u16 *) (p)
#define inl(p)	*(volatile u32 *) (p)

/**************************************************************************
 * SYSTEM DRIVERS
 **************************************************************************
 */

static void drv_system_init (void)
{
	device_t dev;

	memset (&dev, 0, sizeof (dev));

	strcpy (dev.name, "serial");
	dev.flags = DEV_FLAGS_OUTPUT | DEV_FLAGS_INPUT | DEV_FLAGS_SYSTEM;
#ifdef CONFIG_SERIAL_SOFTWARE_FIFO
	dev.putc = serial_buffered_putc;
	dev.puts = serial_buffered_puts;
	dev.getc = serial_buffered_getc;
	dev.tstc = serial_buffered_tstc;
#else
	dev.putc = serial_putc;
	dev.puts = serial_puts;
	dev.getc = serial_getc;
	dev.tstc = serial_tstc;
    serial_puts("dev.puts -> serial\n\r");
#endif

	device_register (&dev);
    //serial_puts("device_registered\n\r");

#ifdef CFG_DEVICE_NULLDEV
	memset (&dev, 0, sizeof (dev));

	strcpy (dev.name, "nulldev");
	dev.flags = DEV_FLAGS_OUTPUT | DEV_FLAGS_INPUT | DEV_FLAGS_SYSTEM;
	dev.putc = nulldev_putc;
	dev.puts = nulldev_puts;
	dev.getc = nulldev_input;
	dev.tstc = nulldev_input;

	device_register (&dev);
#endif
}

/**************************************************************************
 * DEVICES
 **************************************************************************
 */

int device_register (device_t * dev)
{
	ListInsertItem (devlist, dev, LIST_END);
	return 0;
}

/* deregister the device "devname".
 * returns 0 if success, -1 if device is assigned and 1 if devname not found
 */
#ifdef	CFG_DEVICE_DEREGISTER
int device_deregister(char *devname)
{
	int i,l,dev_index;
	device_t *dev = NULL;
	char temp_names[3][8];

	dev_index=-1;
	for (i=1; i<=ListNumItems(devlist); i++) {
		dev = ListGetPtrToItem (devlist, i);
		if(strcmp(dev->name,devname)==0) {
			dev_index=i;
			break;
		}
	}
	if(dev_index<0) /* device not found */
		return 0;
	/* get stdio devices (ListRemoveItem changes the dev list) */
	for (l=0 ; l< MAX_FILES; l++) {
		if (stdio_devices[l] == dev) {
			/* Device is assigned -> report error */
			return -1;
		}
		memcpy (&temp_names[l][0],
			stdio_devices[l]->name,
			sizeof(stdio_devices[l]->name));
	}
	ListRemoveItem(devlist,NULL,dev_index);
	/* reassign Device list */
	for (i=1; i<=ListNumItems(devlist); i++) {
		dev = ListGetPtrToItem (devlist, i);
		for (l=0 ; l< MAX_FILES; l++) {
			if(strcmp(dev->name,temp_names[l])==0) {
				stdio_devices[l] = dev;
			}
		}
	}
	return 0;
}
#endif	/* CFG_DEVICE_DEREGISTER */

int devices_init (void)
{
#ifndef CONFIG_ARM     /* already relocated for current ARM implementation */
	DECLARE_GLOBAL_DATA_PTR;

	ulong relocation_offset = gd->reloc_off;
	int i;

	/* relocate device name pointers */
	for (i = 0; i < (sizeof (stdio_names) / sizeof (char *)); ++i) {
		stdio_names[i] = (char *) (((ulong) stdio_names[i]) +
						relocation_offset);
	}
#endif

	/* Initialize the list */
	devlist = ListCreate (sizeof (device_t));

	if (devlist == NULL) {
		eputs ("Cannot initialize the list of devices!\n");
		return -1;
	}

//serial_puts("List Created\n");


#if defined(CONFIG_HARD_I2C) || defined(CONFIG_SOFT_I2C)
	i2c_init (CFG_I2C_SPEED, CFG_I2C_SLAVE);
#endif
/*#ifdef CONFIG_LCD
	drv_lcd_init ();
#endif*/

//as3310_lcd_init();

#ifdef CONFIG_NAND_FLASH
    Ecc_Init();
    NAND_init();
#endif

#if defined(CONFIG_VIDEO) || defined(CONFIG_CFB_CONSOLE)
	drv_video_init ();
#endif

#ifdef CONFIG_KEYBOARD
	drv_keyboard_init ();
#endif
#ifdef CONFIG_LOGBUFFER
	drv_logbuff_init ();
#endif

    drv_system_init ();
    
#ifdef CONFIG_USB_TTY
	drv_usbtty_init ();
#endif

    //serial_puts("system_inited\n");
	return (0);
}

int devices_done (void)
{
	ListDispose (devlist);

	return 0;
}



void set_gpio(unsigned short iodata){
    outw(iodata,0x800000a0);
}


#define LCD_COL 240
#define LCD_ROW 320
//
// void lcd_delay(int n){
//     int i;
//     while (n-->0) {
//         for(i=0;i<40000;i++);
//     }
// }
//
// int vector_n=0;
// unsigned long lcd_data_buffer;
//
// void lcd_write_reg(unsigned short n){
//     if (vector_n==0) {
//         lcd_data_buffer = n;
//         vector_n++;
//     }
//     else {
//         lcd_data_buffer = lcd_data_buffer + (((unsigned long)n)<<16);
//         while ((0x01000000 & inl(HW_LCDIF_CTRL)) ==0 ) { /*   printf("HW_LCDIF_CTRL = %x\n",inl(HW_LCDIF_CTRL));*/ }
//         lcd_delay(1);
//         outl(lcd_data_buffer,HW_LCDIF_DATA);
//  //       printf("%x\n",lcd_data_buffer);
//         vector_n=0;
//     }
// }
//
// unsigned char lcd_read_reg(unsigned char reg_n){
//
//     outw(reg_n<<8+0xff,0/*0x000fffff*//*HW_LCDIF_DATA*/);
//    // if ((0x01000000 & inl(HW_LCDIF_CTRL)) ==0 ) {    printf("HW_LCDIF_CTRL = %x\n",inl(HW_LCDIF_CTRL)); }
//     lcd_delay(10);
//     return (inw(0)/*&0xff*/);
//     lcd_delay(10);
// }
//
//
// int lcdif_init(){
//     int i;
//     ulong temp;
//
//     //outl(0x00000000,VGA_CTRL);
//     outl( 0xc0000000,HW_LCDIF_CTRL+8); // CLEAR bit 31,30
//     outl( 0x20000000+LCD_COL*LCD_ROW ,HW_LCDIF_CTRL);
//     outl( LCDIF_TIMING_VALUE ,HW_LCDIF_TIMING); // Timing
//     lcd_delay(10);
//     outl( 0x00100000 ,HW_LCDIF_CTRL+4); // SET RESET = 1
//     lcd_delay(2);
//     outl( 0x10000 ,HW_LCDIF_CTRL+8); // STOP LCD
//     outl( 0x40000 ,HW_LCDIF_CTRL+8); // CLEAR DATA MODE
//     outl( LCD_COL*LCD_ROW ,HW_LCDIF_CTRL+4); // SET Count
//     outl( 0x10000 ,HW_LCDIF_CTRL+4); // RUN LCD
//
//          lcd_write_reg(0x0142);  // Start oscillation
//          lcd_write_reg(0x01c2);  // Start oscillation
//          lcd_write_reg(0x01c1);  // Start oscillation
//          lcd_write_reg(0x00A0);  // Standby mode cancel
//          lcd_write_reg(0x0301);  // Software reset operation
//          lcd_delay(20);
//          lcd_write_reg(0x0300);  // Software reset operation cancel
//          lcd_write_reg(0x0600);
//          lcd_write_reg(0x2B04);  // Oscillator frequency adjust setting
//
//          lcd_write_reg(0x281B);  // DC/DC clock frequency adjust setting
//          lcd_write_reg(0x1A05);  // Step up circuit frequency adjust setting
//          lcd_write_reg(0x2505);  // Step up factor in step up circuit 2 setting
//          //lcd_write_reg(0x1C73);  // Step up circuit operating current setting
//          //lcd_write_reg(0x3708);  //// Step up circuit operating current setting 2
//
//          //############# void Power_on_Set(void) ################//
//
//          lcd_write_reg(0x1e01);  // Extra step up circuit1 operation
//          lcd_write_reg(0x1900);  // VR1 and VR2 regulator setting
//          lcd_write_reg(0x18f5);  // VR1 and VR2 regulator on
//          lcd_delay(20);
//
//          lcd_write_reg(0x18f5);  // VCL turn on
//          lcd_write_reg(0x18F5);  // VGH and VGL turn on
//          lcd_delay(80);
//
//          lcd_write_reg(0x18F5);  // DDVDH turn on
//          lcd_delay(80);
//
//          lcd_write_reg(0x1b09);  // VS/VDH turn on and set
//          lcd_delay(20);
//
//          lcd_write_reg(0x1f11);  // VCOMH voltage setting
//          lcd_write_reg(0x2010);  // VCOM amplitude voltage setting
//          lcd_write_reg(0x1e81);  // VCOM operation start
//          lcd_delay(20);
//
//          //############# void Chip_Set (void) ################//
//
//          lcd_write_reg(0x9D00);
//
//
//          lcd_write_reg(0xC000);
//          lcd_write_reg(0xC100);
//          lcd_write_reg(0x0E00);
//          lcd_write_reg(0x0F00);
//          lcd_write_reg(0x1000);
//          lcd_write_reg(0x1100);
//          lcd_write_reg(0x1200);
//          lcd_write_reg(0x1300);
//          lcd_write_reg(0x1400);
//          lcd_write_reg(0x1500);
//
//          lcd_write_reg(0x1600);
//          lcd_write_reg(0x1700);
//          lcd_write_reg(0x3401);
//          lcd_write_reg(0x3500);
//          lcd_write_reg(0x4B00);
//          lcd_write_reg(0x4C00);
//          lcd_write_reg(0x4E00);
//          lcd_write_reg(0x4F00);
//          lcd_write_reg(0x5000);
//          lcd_write_reg(0x3C00);
//
//          lcd_write_reg(0x3D00);
//          lcd_write_reg(0x3E01);
//          lcd_write_reg(0x3Fff);
//          lcd_write_reg(0x4002);
//          lcd_write_reg(0x4102);
//          lcd_write_reg(0x4202);
//          lcd_write_reg(0x4300);
//          lcd_write_reg(0x4400);
//          lcd_write_reg(0x4500);
//          lcd_write_reg(0x46EF);
//
//          lcd_write_reg(0x4700);
//          lcd_write_reg(0x4800);
//          lcd_write_reg(0x4901);
//          lcd_write_reg(0x4A3F);
//
//          //lcd_write_reg(0x0600);
//          lcd_write_reg(0x1de6);  // Gate scan direction setting
//          lcd_write_reg(0x8600);
//          lcd_write_reg(0x8730);
//          lcd_write_reg(0x8802);
//          lcd_write_reg(0x8905);
//          lcd_write_reg(0x8D01);  // Register set-up mode for one line clock number
//          lcd_write_reg(0x8B25);  // One line SYSCLK number in one-line scan
//          lcd_write_reg(0x3377);  // N line inversion setting
//          lcd_write_reg(0x370e);  // Scanning method setting
//          lcd_write_reg(0x7600);
//          //lcd_write_reg(0xc100);
//
//          //############# void Gamma_Set(void) ################//
//
//          lcd_write_reg(0x8F00);
//          lcd_write_reg(0x9000);
//          lcd_write_reg(0x9100);
//          lcd_write_reg(0x9200);
//          lcd_write_reg(0x9300);
//          lcd_write_reg(0x9400);
//          lcd_write_reg(0x9500);
//          lcd_write_reg(0x9600);
//          lcd_write_reg(0x9700);
//          lcd_write_reg(0x9800);
//          lcd_write_reg(0x9900);
//          lcd_write_reg(0x9A00);
//
//          //############# void Display_On(void) ################//
//
//          lcd_write_reg(0x3B01);
//          lcd_delay(40);
//          lcd_write_reg(0x0020);
//          lcd_write_reg(0x0504);
//          lcd_write_reg(0x0504);
//
//
//     outl( 0x10000 ,HW_LCDIF_CTRL+8); // STOP LCD
//     outl( 0x40000 ,HW_LCDIF_CTRL+4); // SET DATA MODE
//     outl(0xffff ,HW_LCDIF_CTRL+4); // SET Count
// //    outl( 0x10000 ,HW_LCDIF_CTRL+4); // RUN LCD
//
// }
//
// void WriteLCDIFData()
// {
//     int i,data;
//     int count=DMA_XFER_COUNT*4;
//     int *memaddr=(int*)LCDIF_DATA_BASE_ADDR;
//
//     for (i=0;i<(count>>2);i++) {
//         outl(0x001f001f,memaddr++);
//     }
// }
//
//
// void as3310_lcd_init()
// {
//
//
//
//
//   //  outl(0x00000000,VGA_CTRL);
//   //  lcdif_init();
//   //  WriteLCDIFData();
//   //  DMA_LCD_FRAME_FLUSH(CONFIG_AS3310_BOOT_LOGO);
// //  lcd_frame_flush(CONFIG_AS3310_BOOT_LOGO);   // show logo
// }
//
//

