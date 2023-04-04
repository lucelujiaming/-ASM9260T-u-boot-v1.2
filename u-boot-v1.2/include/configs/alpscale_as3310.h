/*
    AlphaScale AS3310B Soc Header File
    Define all registers IO prots .. etc

    Last Modify:
    2006-11-22 Hoffer
        Switch to AS3310C

    2006-05-26 Hoffer
        Create File.
 */

#include <asm/arch/sizes.h>

#ifndef __ALPSCALE_AS3310B_H__
#define __ALPSCALE_AS3310B_H__


#ifndef __ASSEMBLY__
typedef volatile unsigned char AS3310_REG8;
typedef volatile unsigned short AS3310_REG16;
typedef volatile unsigned long AS3310_REG32;

#endif

#define REG_VAL 0
#define REG_SET 1
#define REG_CLR 2
#define REG_TOG 3

/*      LCD     */
#define HW_LCDIF_CTRL    0x80000200
#define HW_LCDIF_TIMING  0x80000210
#define HW_LCDIF_DATA    0x80000220
#define HW_LCDIF_DEBUG   0x80000230

/*      LCD CONTORL      */
#define LCDCTRL_BASE_ADDRESS 0x80028000

#define	HW_LCDCTRL_CTRL0_ADDR       LCDCTRL_BASE_ADDRESS
#define	HW_LCDCTRL_CTRL1_ADDR       LCDCTRL_BASE_ADDRESS + 0x10
#define	HW_LCDCTRL_DATA_ADDR        LCDCTRL_BASE_ADDRESS + 0x20
#define	HW_LCDCTRL_TIMING0_ADDR	    LCDCTRL_BASE_ADDRESS + 0x30
#define	HW_LCDCTRL_TIMING1_ADDR	    LCDCTRL_BASE_ADDRESS + 0x40
#define	HW_LCDCTRL_TIMING2_ADDR	    LCDCTRL_BASE_ADDRESS + 0x50
#define	HW_LCD_FIFO_STAT_ADDR	    LCDCTRL_BASE_ADDRESS + 0x60
#define	HW_LCD_STAT_ADDR	        LCDCTRL_BASE_ADDRESS + 0x70
#define	HW_LCD_SUBPANEL_ADDR	    LCDCTRL_BASE_ADDRESS + 0x80
#define	HW_LCD_LINEINT_ADDR	        LCDCTRL_BASE_ADDRESS + 0x90
#define	HW_LCD_DISPLAYSTATUS_ADDR   LCDCTRL_BASE_ADDRESS + 0xA0
#define	HW_LCD_MONITOR_CTRL_ADDR    LCDCTRL_BASE_ADDRESS + 0xB0
#define	HW_LCD_SWITCH_CTRL_ADDR	    LCDCTRL_BASE_ADDRESS + 0xC0
#define	HW_LCD_CHECK_CTRL_ADDR	    LCDCTRL_BASE_ADDRESS + 0xD0
#define	HW_LCD_SECURE_CTRL_ADDR	    LCDCTRL_BASE_ADDRESS + 0xE0


/*      DMA         */

#define LCDIF_CTRLC_VALUE       0xc0000000
#define LCDIF_CTRL_VALUE        (0x21170000)|((int)DMA_XFER_COUNT)
#define LCDIF_TIMING_VALUE      0x0a030a03



/*DMA_APBH*/
#define HW_APBH_BASE 			0x80004000

#define HW_APBH_CTRL0 			0x80004000
#define HW_APBH_CTRL0_SET 		0x80004004
#define HW_APBH_CTRL0_CLR 		0x80004008
#define HW_APBH_CTRL0_TOG 		0x8000400C

#define HW_APBH_CTRL1 			0x80004010
#define HW_APBH_CTRL1_SET 		0x80004014
#define HW_APBH_CTRL1_CLR 		0x80004018
#define HW_APBH_CTRL1_TOG 		0x8000401C

/* Channel 0 - 7*/

//  #define HW_APBH_CH0_NXTCMDAR 0x80004040
//  #define HW_APBH_CH0_SEMA 0x80004070
//  
//  #define HW_APBH_CH1_NXTCMDAR 0x800040B0
//  #define HW_APBH_CH1_SEMA 0x800040E0
//  
//  #define HW_APBH_CH2_NXTCMDAR 	0x80004120
//  #define HW_APBH_CH2_SEMA 		0x80004150
//  
//  #define HW_APBH_CH3_NXTCMDAR 	0x80004190
//  #define HW_APBH_CH3_SEMA 		0x800041C0
//  
//  #define HW_APBH_CH4_NXTCMDAR 	0x80004200
//  #define HW_APBH_CH4_SEMA 		0x80004230

#define HW_APBH_CH0_NXTCMDAR  (HW_APBH_BASE+0x40)
#define HW_APBH_CH0_SEMA (HW_APBH_CH0_NXTCMDAR+0x30)

#define HW_APBH_CH1_NXTCMDAR   (HW_APBH_BASE+0xB0)
#define HW_APBH_CH1_SEMA   (HW_APBH_CH1_NXTCMDAR+0x30)

#define HW_APBH_CH2_NXTCMDAR   (HW_APBH_BASE+0x120)
#define HW_APBH_CH2_SEMA   (HW_APBH_CH2_NXTCMDAR+0x30)

#define HW_APBH_CH3_NXTCMDAR   (HW_APBH_BASE+0x190)
#define HW_APBH_CH3_SEMA	(HW_APBH_CH3_NXTCMDAR+0x30)

#define HW_APBH_CH4_NXTCMDAR    (HW_APBH_BASE+0x200)
#define HW_APBH_CH4_SEMA   (HW_APBH_CH4_NXTCMDAR+0x30)


#define HW_APBH_DEVSEL 0x80004020
/*DMA_APBX*/
#define HW_APBX_CTRL0 			0x80024000
#define HW_APBX_CTRL0_SET 		0x80024004
#define HW_APBX_CTRL0_CLR 		0x80024008
#define HW_APBX_CTRL0_TOG 		0x8002400C

#define HW_APBX_DEVSEL 			0x80024020

#define HW_APBX_CH3_NXTCMDAR	0x80024190
#define HW_APBX_CH3_SEMA		0x800241C0

#define HW_APBX_CH4_NXTCMDAR	0x80024200
#define HW_APBX_CH4_SEMA		0x80024230


/*			NAND FLASH REG STRUCTURES			*/
#define AS3310_NAND_BASE	0x50000000

/*  MT29F2G16AABWP   2GB NAND FLASH CMD WORD--------*/
#define CONF_PAGE_BYTES 1024

#define AS3310_NAND_READ_CMD1 0x30
#define AS3310_NAND_READ_CMD 0x00
#define AS3310_NAND_WRITE_CMD1 0x10
#define AS3310_NAND_WRITE_CMD 0x80
#define AS3310_NAND_ERASE_CMD1 0xD0
#define AS3310_NAND_ERASE_CMD 0x60
#define AS3310_NAND_SATA_CMD 0x70


/*			NOR FLASH REG STRUCTURES			*/
#define AS3310_NRO_BASE	0x00000000

/*  NOR_MBM29LV160TE  2MB NOR FLASH CMD WORD--------*/

/* Flash Organizations */
//OrgDef OrgMem_29LV160TE[] = {
//	{1, 16 * 1024},				/* 1 * 16kBytes sectors */
//    {3, 8 * 1024},				/* 3 * 8kBytes sectors */
//    {31, 32 * 1024},				/* 31 * 32kBytes sectors */
//
//};


#define MT_XXXX 1					// select the nor flash

#ifdef MT_XXXX
#define NOR_MBM29LV160TE_ADDR_BYTEMODE_1 0x555
#define NOR_MBM29LV160TE_ADDR_BYTEMODE_2 0x2AA
#define NOR_MBM29LV160TE_ADDR_BYTEMODE_3 0x555
#define NOR_MBM29LV160TE_ADDR_BYTEMODE_4 0x555
#define NOR_MBM29LV160TE_ADDR_BYTEMODE_5 0x2AA
#define NOR_MBM29LV160TE_ADDR_BYTEMODE_6 0x555

#define NOR_MBM29LV160TE_ADDR_WORDMODE_1 0xAAA
#define NOR_MBM29LV160TE_ADDR_WORDMODE_2 0x554
#define NOR_MBM29LV160TE_ADDR_WORDMODE_3 0xAAA
#define NOR_MBM29LV160TE_ADDR_WORDMODE_4 0xAAA
#define NOR_MBM29LV160TE_ADDR_WORDMODE_5 0x554
#define NOR_MBM29LV160TE_ADDR_WORDMODE_6 0xAAA

#define NOR_MBM29LV160TE_COMMON_CMD_1 0xAA
#define NOR_MBM29LV160TE_COMMON_CMD_2 0x55
#define NOR_MBM29LV160TE_WRITE_CMD_3 0xA0
#define NOR_MBM29LV160TE_ERASE_CMD_3 0x80
#define NOR_MBM29LV160TE_ERASE_CMD_4 0xAA
#define NOR_MBM29LV160TE_ERASE_CMD_5 0x55
#define NOR_MBM29LV160TE_ERASE_CMD_6_SECTOR 0x30
#define NOR_MBM29LV160TE_ERASE_CMD_6_CHIP 0x10
#endif

#ifdef SST_39VF1601
#define NOR_MBM29LV160TE_ADDR_BYTEMODE_1 0x555
#define NOR_MBM29LV160TE_ADDR_BYTEMODE_2 0x2AA
#define NOR_MBM29LV160TE_ADDR_BYTEMODE_3 0x555
#define NOR_MBM29LV160TE_ADDR_BYTEMODE_4 0x555
#define NOR_MBM29LV160TE_ADDR_BYTEMODE_5 0x2AA
#define NOR_MBM29LV160TE_ADDR_BYTEMODE_6 0x555

#define NOR_MBM29LV160TE_ADDR_WORDMODE_1 0xAAAA
#define NOR_MBM29LV160TE_ADDR_WORDMODE_2 0x5554
#define NOR_MBM29LV160TE_ADDR_WORDMODE_3 0xAAAA
#define NOR_MBM29LV160TE_ADDR_WORDMODE_4 0xAAAA
#define NOR_MBM29LV160TE_ADDR_WORDMODE_5 0x5554
#define NOR_MBM29LV160TE_ADDR_WORDMODE_6 0xAAAA

#define NOR_MBM29LV160TE_COMMON_CMD_1 0xAA
#define NOR_MBM29LV160TE_COMMON_CMD_2 0x55
#define NOR_MBM29LV160TE_WRITE_CMD_3 0xA0
#define NOR_MBM29LV160TE_ERASE_CMD_3 0x80
#define NOR_MBM29LV160TE_ERASE_CMD_4 0xAA
#define NOR_MBM29LV160TE_ERASE_CMD_5 0x55
#define NOR_MBM29LV160TE_ERASE_CMD_6_SECTOR 0x50
#define NOR_MBM29LV160TE_ERASE_CMD_6_CHIP 0x10
#endif


/* PIN CONTROL AS3310C-Only*/
#define PIN_CTRL_BASE       0x80018000

#define PIN_FUNCTION_0      0
#define PIN_FUNCTION_1      1
#define PIN_FUNCTION_2      2
#define PIN_FUNCTION_GPIO   3

#define PINCTRL_CTRL       PIN_CTRL_BASE
#define PIN_CTRL_MUXSEL_0   PINCTRL_CTRL + 0x10
#define PIN_CTRL_MUXSEL_1   PINCTRL_CTRL + 0x20
#define PIN_CTRL_DRIVE_0    PINCTRL_CTRL + 0x30
#define PIN_CTRL_DOUT_0     PINCTRL_CTRL + 0x50
#define PIN_CTRL_DIN_0      PINCTRL_CTRL + 0x60
#define PIN_CTRL_DOE_0      PINCTRL_CTRL + 0x70
#define PIN_CTRL_PIN2IRQ_0  PINCTRL_CTRL + 0x80
#define PIN_CTRL_IRQEN_0    PINCTRL_CTRL + 0x90
#define PIN_CTRL_IRQLEVEL_0 PINCTRL_CTRL + 0xA0

#define HW_PINCTRL_CTRL				PIN_CTRL_BASE
#define HW_PINCTRL_CTRL_SET 		PIN_CTRL_BASE + 0x4
#define HW_PINCTRL_CTRL_CLR 		PIN_CTRL_BASE + 0x8
#define HW_PINCTRL_CTRL_TOG 		PIN_CTRL_BASE + 0xc

#define HW_PINCTRL_MUXSEL0			0x80018010
#define HW_PINCTRL_MUXSEL0_SET		0x80018014
#define HW_PINCTRL_MUXSEL0_CLR		0x80018018
#define HW_PINCTRL_MUXSEL0_TOG		0x8001801C

#define HW_PINCTRL_MUXSEL1			0x80018020
#define HW_PINCTRL_MUXSEL1_SET		0x80018024
#define HW_PINCTRL_MUXSEL1_CLR		0x80018028
#define HW_PINCTRL_MUXSEL1_TOG		0x8001802C

#define HW_PINCTRL_MUXSEL2			0x80018110
#define HW_PINCTRL_MUXSEL2_SET		0x80018114
#define HW_PINCTRL_MUXSEL2_CLR		0x80018118
#define HW_PINCTRL_MUXSEL2_TOG		0x8001811C

#define HW_PINCTRL_MUXSEL3			0x80018120
#define HW_PINCTRL_MUXSEL3_SET		0x80018124
#define HW_PINCTRL_MUXSEL3_CLR		0x80018128
#define HW_PINCTRL_MUXSEL3_TOG		0x8001812C

#define HW_PINCTRL_MUXSEL4			0x80018210
#define HW_PINCTRL_MUXSEL4_SET		0x80018214
#define HW_PINCTRL_MUXSEL4_CLR		0x80018218
#define HW_PINCTRL_MUXSEL4_TOG		0x8001821C

#define HW_PINCTRL_MUXSEL5			0x80018220
#define HW_PINCTRL_MUXSEL5_SET		0x80018224
#define HW_PINCTRL_MUXSEL5_CLR		0x80018228
#define HW_PINCTRL_MUXSEL5_TOG		0x8001822C

#define HW_PINCTRL_MUXSEL6			0x80018310
#define HW_PINCTRL_MUXSEL6_SET		0x80018314
#define HW_PINCTRL_MUXSEL6_CLR		0x80018318
#define HW_PINCTRL_MUXSEL6_TOG		0x8001831C

#define HW_PINCTRL_MUXSEL7			0x80018320
#define HW_PINCTRL_MUXSEL7_SET		0x80018324
#define HW_PINCTRL_MUXSEL7_CLR		0x80018328
#define HW_PINCTRL_MUXSEL7_TOG		0x8001832C

#define HW_PINCTRL_PU3_ADDR			0x800183D0
#define HW_PINCTRL_PU3_ADDR_SET			0x800183D4
#define HW_PINCTRL_PU3_ADDR_CLR			0x800183D8
#define HW_PINCTRL_PU3_ADDR_TOG			0x800183DC



#define AS3310_INT_BASE		0x80000000
#define INT_IVAR 			AS3310_INT_BASE	//	R/W	Interrupt Vector Address Register
#define INT_LEVACK 			AS3310_INT_BASE + 0x10	//	R/W	Level Acknowledge Register
#define INT_RWCTRL 			AS3310_INT_BASE + 0x20	//	R/W	Control Register
#define INT_STAT			AS3310_INT_BASE + 0x30	//	R	Status Register
#define INT_RAWINT0 		AS3310_INT_BASE + 0x40	//	R	Raw Interrupt Input Register 0
#define INT_RAWINT1 		AS3310_INT_BASE + 0x50	//	R	Raw Interrupt Input Register 1
#define INT_RWPRIORITY0  	AS3310_INT_BASE + 0x60	//	R/W	Priority Register 0
#define INT_RWPRIORITY1  	AS3310_INT_BASE + 0x70	//	R/W	Priority Register 1
#define INT_RWPRIORITY2  	AS3310_INT_BASE + 0x80	//	R/W	Priority Register 2
#define INT_RWPRIORITY3  	AS3310_INT_BASE + 0x90	//	R/W	Priority Register 3
#define INT_RWPRIORITY4  	AS3310_INT_BASE + 0xA0	//	R/W	Priority Register 4
#define INT_RWPRIORITY5  	AS3310_INT_BASE + 0xB0	//	R/W	Priority Register 5
#define INT_RWPRIORITY6  	AS3310_INT_BASE + 0xC0	//	R/W	Priority Register 6
#define INT_RWPRIORITY7  	AS3310_INT_BASE + 0xD0	//	R/W	Priority Register 7
#define INT_RWPRIORITY8  	AS3310_INT_BASE + 0xE0	//	R/W	Priority Register 8
#define INT_RWPRIORITY9  	AS3310_INT_BASE + 0xF0	//	R/W	Priority Register 9
#define INT_RWPRIORITY10 	AS3310_INT_BASE + 0x100	//	R/W	Priority Register 10
#define INT_RWPRIORITY11 	AS3310_INT_BASE + 0x110	//	R/W	Priority Register 11
#define INT_RWPRIORITY12 	AS3310_INT_BASE + 0x120	//	R/W	Priority Register 12
#define INT_RWPRIORITY13 	AS3310_INT_BASE + 0x130	//	R/W	Priority Register 13
#define INT_RWPRIORITY14 	AS3310_INT_BASE + 0x140	//	R/W	Priority Register 14
#define INT_RWPRIORITY15 	AS3310_INT_BASE + 0x150	//	R/W	Priority Register 15
#define INT_IVBA 			AS3310_INT_BASE + 0x160	//	R/W	Interrupt Vector Base Address Register
#define INT_DEBUG0 			AS3310_INT_BASE + 0x170	//	R	Debug Register 0
#define INT_DEBUGRD0 		AS3310_INT_BASE + 0x180	//	R	Debug Read Register 0
#define INT_DEBUGRD1 		AS3310_INT_BASE + 0x190	//	R	Debug Read Register 1
#define INT_DEBUGFLAG 		AS3310_INT_BASE + 0x1A0	//	R/W	Debug Flag Register
#define INT_DEBUGRDREQ0 	AS3310_INT_BASE + 0x1B0	//	R	Debug Read Request Register 0
#define INT_DEBUGRDREQ1 	AS3310_INT_BASE + 0x1C0	//	R	Debug Read Request Register 1
#define INT_IRQCLEAR0 	    AS3310_INT_BASE + 0x1D0	//	IRQ Clear 0
#define INT_IRQCLEAR1 	    AS3310_INT_BASE + 0x1E0	//	IRQ Clear 1
#define AS3310_INT_BASE_ADDR 	    AS3310_INT_BASE + 0x1F0	//	IRQ Clear 1


/* TIMER */
    #define ALPAS3310_TIMER_BASE		0x80068000
    #define TIMER_ROTCTRL	ALPAS3310_TIMER_BASE	//	Timer CTRL register
    #define TIMER_ROTCOUNT 	ALPAS3310_TIMER_BASE + 0x10	//	Timer COUNT register
    #define TIMER0_CTRL 	ALPAS3310_TIMER_BASE + 0x20	//	Timer 0 control register
    #define TIMER0_COUNTER	ALPAS3310_TIMER_BASE + 0x30	//	Timer 0 counter register
    #define TIMER1_CTRL 	ALPAS3310_TIMER_BASE + 0x40	//	Timer 1 control register
    #define TIMER1_COUNTER	ALPAS3310_TIMER_BASE + 0x50	//	Timer 1 counter register
    #define TIMER2_CTRL 	ALPAS3310_TIMER_BASE + 0x60	//	Timer 2 control register
    #define TIMER2_COUNTER	ALPAS3310_TIMER_BASE + 0x70	//	Timer 2 counter register
    #define TIMER3_CTRL 	ALPAS3310_TIMER_BASE + 0x80	//	Timer 3 control register
    #define TIMER3_COUNTER	ALPAS3310_TIMER_BASE + 0x90	//	Timer 3 counter register

#if 0
/* UART1 Registers  */
#define ALPAS3310_UART1_BASE	    0x8006C000	 /* UART 1 */
#define UART1_CTRL0 ALPAS3310_UART1_BASE
#define UART1_CTRL1 (ALPAS3310_UART1_BASE + 0x10)
#define UART1_CTRL2 (ALPAS3310_UART1_BASE + 0x20)
#define UART1_LINE_CTRL (ALPAS3310_UART1_BASE + 0x30)
#define UART1_INTR (ALPAS3310_UART1_BASE + 0x40)
#define UART1_INTR_CLEAR (ALPAS3310_UART1_BASE + 0x48)
#define UART1_DATA (ALPAS3310_UART1_BASE + 0x50)
#define UART1_SATA (ALPAS3310_UART1_BASE + 0x60)
#define UART1_DEBUG (ALPAS3310_UART1_BASE + 0x70)
#endif
/////////////////////////////////////////////////////////
//UART0
#define UART0_BASEESS  0x80000000

/////////////////////////////////////////////////////////
//UART0
#define UART0_BASEESS  0x80000000

#define HW_UART0_CTRL0          (UART0_BASEESS + 0x00)
#define HW_UART0_CTRL1          (UART0_BASEESS + 0x10)
#define HW_UART0_CTRL2          (UART0_BASEESS + 0x20)
#define HW_UART0_LINECTRL       (UART0_BASEESS + 0x30)
#define HW_UART0_INTR           (UART0_BASEESS + 0x40)
#define HW_UART0_DATA           (UART0_BASEESS + 0x50)
#define HW_UART0_STAT           (UART0_BASEESS + 0x60)
#define HW_UART0_DEBUG          (UART0_BASEESS + 0x70)
#define HW_UART0_ILPR           (UART0_BASEESS + 0x80)
#define HW_UART0_RS485CTRL      (UART0_BASEESS + 0x90)
#define HW_UART0_RS485ADRMATCH  (UART0_BASEESS + 0xa0)
#define HW_UART0_RS485DLY       (UART0_BASEESS + 0xb0)
#define HW_UART0_AUTOBAUD       (UART0_BASEESS + 0xc0)
#define HW_UART0_CTRL3          (UART0_BASEESS + 0xd0)


/////////////////////////////////////////////////////////
//UART1
#define UART1_BASEESS  0x80004000

#define HW_UART1_CTRL0          (UART1_BASEESS + 0x00)
#define HW_UART1_CTRL1          (UART1_BASEESS + 0x10)
#define HW_UART1_CTRL2          (UART1_BASEESS + 0x20)
#define HW_UART1_LINECTRL       (UART1_BASEESS + 0x30)
#define HW_UART1_INTR           (UART1_BASEESS + 0x40)
#define HW_UART1_DATA           (UART1_BASEESS + 0x50)
#define HW_UART1_STAT           (UART1_BASEESS + 0x60)
#define HW_UART1_DEBUG          (UART1_BASEESS + 0x70)
#define HW_UART1_ILPR           (UART1_BASEESS + 0x80)
#define HW_UART1_RS485CTRL      (UART1_BASEESS + 0x90)
#define HW_UART1_RS485ADRMATCH  (UART1_BASEESS + 0xa0)
#define HW_UART1_RS485DLY       (UART1_BASEESS + 0xb0)
#define HW_UART1_AUTOBAUD       (UART1_BASEESS + 0xc0)
#define HW_UART1_CTRL3          (UART1_BASEESS + 0xd0)

/////////////////////////////////////////////////////////
//UART2
#define UART2_BASEESS  0x80008000

#define HW_UART2_CTRL0          (UART2_BASEESS + 0x00)
#define HW_UART2_CTRL1          (UART2_BASEESS + 0x10)
#define HW_UART2_CTRL2          (UART2_BASEESS + 0x20)
#define HW_UART2_LINECTRL       (UART2_BASEESS + 0x30)
#define HW_UART2_INTR           (UART2_BASEESS + 0x40)
#define HW_UART2_DATA           (UART2_BASEESS + 0x50)
#define HW_UART2_STAT           (UART2_BASEESS + 0x60)
#define HW_UART2_DEBUG          (UART2_BASEESS + 0x70)
#define HW_UART2_ILPR           (UART2_BASEESS + 0x80)
#define HW_UART2_RS485CTRL      (UART2_BASEESS + 0x90)
#define HW_UART2_RS485ADRMATCH  (UART2_BASEESS + 0xa0)
#define HW_UART2_RS485DLY       (UART2_BASEESS + 0xb0)
#define HW_UART2_AUTOBAUD       (UART2_BASEESS + 0xc0)
#define HW_UART2_CTRL3          (UART2_BASEESS + 0xd0)


/////////////////////////////////////////////////////////
//UART3
#define UART3_BASEESS  0x8000c000

#define HW_UART3_CTRL0          (UART3_BASEESS + 0x00)
#define HW_UART3_CTRL1          (UART3_BASEESS + 0x10)
#define HW_UART3_CTRL2          (UART3_BASEESS + 0x20)
#define HW_UART3_LINECTRL       (UART3_BASEESS + 0x30)
#define HW_UART3_INTR           (UART3_BASEESS + 0x40)
#define HW_UART3_DATA           (UART3_BASEESS + 0x50)
#define HW_UART3_STAT           (UART3_BASEESS + 0x60)
#define HW_UART3_DEBUG          (UART3_BASEESS + 0x70)
#define HW_UART3_ILPR           (UART3_BASEESS + 0x80)
#define HW_UART3_RS485CTRL      (UART3_BASEESS + 0x90)
#define HW_UART3_RS485ADRMATCH  (UART3_BASEESS + 0xa0)
#define HW_UART3_RS485DLY       (UART3_BASEESS + 0xb0)
#define HW_UART3_AUTOBAUD       (UART3_BASEESS + 0xc0)
#define HW_UART3_CTRL3          (UART3_BASEESS + 0xd0)

/////////////////////////////////////////////////////////
//UART4
#define UART4_BASEESS  0x80010000

#define HW_UART4_CTRL0          (UART4_BASEESS + 0x00)
#define HW_UART4_CTRL1          (UART4_BASEESS + 0x10)
#define HW_UART4_CTRL2          (UART4_BASEESS + 0x20)
#define HW_UART4_LINECTRL       (UART4_BASEESS + 0x30)
#define HW_UART4_INTR           (UART4_BASEESS + 0x40)
#define HW_UART4_DATA           (UART4_BASEESS + 0x50)
#define HW_UART4_STAT           (UART4_BASEESS + 0x60)
#define HW_UART4_DEBUG          (UART4_BASEESS + 0x70)
#define HW_UART4_ILPR           (UART4_BASEESS + 0x80)
#define HW_UART4_RS485CTRL      (UART4_BASEESS + 0x90)
#define HW_UART4_RS485ADRMATCH  (UART4_BASEESS + 0xa0)
#define HW_UART4_RS485DLY       (UART4_BASEESS + 0xb0)
#define HW_UART4_AUTOBAUD       (UART4_BASEESS + 0xc0)
#define HW_UART4_CTRL3          (UART4_BASEESS + 0xd0)

/////////////////////////////////////////////////////////
//UART5
#define UART5_BASEESS  0x80014000

#define HW_UART5_CTRL0          (UART5_BASEESS + 0x00)
#define HW_UART5_CTRL1          (UART5_BASEESS + 0x10)
#define HW_UART5_CTRL2          (UART5_BASEESS + 0x20)
#define HW_UART5_LINECTRL       (UART5_BASEESS + 0x30)
#define HW_UART5_INTR           (UART5_BASEESS + 0x40)
#define HW_UART5_DATA           (UART5_BASEESS + 0x50)
#define HW_UART5_STAT           (UART5_BASEESS + 0x60)
#define HW_UART5_DEBUG          (UART5_BASEESS + 0x70)
#define HW_UART5_ILPR           (UART5_BASEESS + 0x80)
#define HW_UART5_RS485CTRL      (UART5_BASEESS + 0x90)
#define HW_UART5_RS485ADRMATCH  (UART5_BASEESS + 0xa0)
#define HW_UART5_RS485DLY       (UART5_BASEESS + 0xb0)
#define HW_UART5_AUTOBAUD       (UART5_BASEESS + 0xc0)
#define HW_UART5_CTRL3          (UART5_BASEESS + 0xd0)

/////////////////////////////////////////////////////////
//UART6
#define UART6_BASEESS  0x80018000

#define HW_UART6_CTRL0          (UART6_BASEESS + 0x00)
#define HW_UART6_CTRL1          (UART6_BASEESS + 0x10)
#define HW_UART6_CTRL2          (UART6_BASEESS + 0x20)
#define HW_UART6_LINECTRL       (UART6_BASEESS + 0x30)
#define HW_UART6_INTR           (UART6_BASEESS + 0x40)
#define HW_UART6_DATA           (UART6_BASEESS + 0x50)
#define HW_UART6_STAT           (UART6_BASEESS + 0x60)
#define HW_UART6_DEBUG          (UART6_BASEESS + 0x70)
#define HW_UART6_ILPR           (UART6_BASEESS + 0x80)
#define HW_UART6_RS485CTRL      (UART6_BASEESS + 0x90)
#define HW_UART6_RS485ADRMATCH  (UART6_BASEESS + 0xa0)
#define HW_UART6_RS485DLY       (UART6_BASEESS + 0xb0)
#define HW_UART6_AUTOBAUD       (UART6_BASEESS + 0xc0)
#define HW_UART6_CTRL3          (UART6_BASEESS + 0xd0)

/////////////////////////////////////////////////////////
//UART7
#define UART7_BASEESS  0x8001c000

#define HW_UART7_CTRL0          (UART7_BASEESS + 0x00)
#define HW_UART7_CTRL1          (UART7_BASEESS + 0x10)
#define HW_UART7_CTRL2          (UART7_BASEESS + 0x20)
#define HW_UART7_LINECTRL       (UART7_BASEESS + 0x30)
#define HW_UART7_INTR           (UART7_BASEESS + 0x40)
#define HW_UART7_DATA           (UART7_BASEESS + 0x50)
#define HW_UART7_STAT           (UART7_BASEESS + 0x60)
#define HW_UART7_DEBUG          (UART7_BASEESS + 0x70)
#define HW_UART7_ILPR           (UART7_BASEESS + 0x80)
#define HW_UART7_RS485CTRL      (UART7_BASEESS + 0x90)
#define HW_UART7_RS485ADRMATCH  (UART7_BASEESS + 0xa0)
#define HW_UART7_RS485DLY       (UART7_BASEESS + 0xb0)
#define HW_UART7_AUTOBAUD       (UART7_BASEESS + 0xc0)
#define HW_UART7_CTRL3          (UART7_BASEESS + 0xd0)

/////////////////////////////////////////////////////////
//UART8
#define UART8_BASEESS  0x80020000

#define HW_UART8_CTRL0          (UART8_BASEESS + 0x00)
#define HW_UART8_CTRL1          (UART8_BASEESS + 0x10)
#define HW_UART8_CTRL2          (UART8_BASEESS + 0x20)
#define HW_UART8_LINECTRL       (UART8_BASEESS + 0x30)
#define HW_UART8_INTR           (UART8_BASEESS + 0x40)
#define HW_UART8_DATA           (UART8_BASEESS + 0x50)
#define HW_UART8_STAT           (UART8_BASEESS + 0x60)
#define HW_UART8_DEBUG          (UART8_BASEESS + 0x70)
#define HW_UART8_ILPR           (UART8_BASEESS + 0x80)
#define HW_UART8_RS485CTRL      (UART8_BASEESS + 0x90)
#define HW_UART8_RS485ADRMATCH  (UART8_BASEESS + 0xa0)
#define HW_UART8_RS485DLY       (UART8_BASEESS + 0xb0)
#define HW_UART8_AUTOBAUD       (UART8_BASEESS + 0xc0)
#define HW_UART8_CTRL3          (UART8_BASEESS + 0xd0)

/////////////////////////////////////////////////////////
//UART9
#define UART9_BASEESS  0x80024000

#define HW_UART9_CTRL0          (UART9_BASEESS + 0x00)
#define HW_UART9_CTRL1          (UART9_BASEESS + 0x10)
#define HW_UART9_CTRL2          (UART9_BASEESS + 0x20)
#define HW_UART9_LINECTRL       (UART9_BASEESS + 0x30)
#define HW_UART9_INTR           (UART9_BASEESS + 0x40)
#define HW_UART9_DATA           (UART9_BASEESS + 0x50)
#define HW_UART9_STAT           (UART9_BASEESS + 0x60)
#define HW_UART9_DEBUG          (UART9_BASEESS + 0x70)
#define HW_UART9_ILPR           (UART9_BASEESS + 0x80)
#define HW_UART9_RS485CTRL      (UART9_BASEESS + 0x90)
#define HW_UART9_RS485ADRMATCH  (UART9_BASEESS + 0xa0)
#define HW_UART9_RS485DLY       (UART9_BASEESS + 0xb0)
#define HW_UART9_AUTOBAUD       (UART9_BASEESS + 0xc0)
#define HW_UART9_CTRL3          (UART9_BASEESS + 0xd0)




/*		GPIO		*/
#define ALPAS3310_GPIO_BASE	 0x80017000
#define ALPAS3310_GPIO_DATA	 ALPAS3310_GPIO_BASE
#define ALPAS3310_GPIO_DIRECTION	 ALPAS3310_GPIO_BASE +4
#define ALPAS3310_GPIO_INT	 ALPAS3310_GPIO_BASE +8

#if 0
/*
 * EMI Registers
 */
#define AS3310_EMI_BASE		0x80020000

#define HW_EMICTRL	        (AS3310_EMI_BASE  )  			/*control	write*/
#define HW_EMISTATE	        (AS3310_EMI_BASE + 0x10)				/*status	read */
#define HW_EMIDRAMSTATE	    (AS3310_EMI_BASE + 0x80	)	/*status	read*/
#define HW_EMIDRAMCTRL	    (AS3310_EMI_BASE + 0x90	)	/*control	write*/
#define HW_EMIDRAMADDR	    (AS3310_EMI_BASE + 0xA0	)	/*control	write*/
#define HW_EMIDRAMMODE	    (AS3310_EMI_BASE + 0xB0	)	/*control	write*/
#define HW_EMIDRAMTIME	    (AS3310_EMI_BASE + 0xC0	)	/*control	write*/
#define HW_EMIDRAMTIME2	    (AS3310_EMI_BASE + 0xD0	)	/*control	write*/
#define HW_EMISTATICCTRL	(AS3310_EMI_BASE + 0x100)	/*control	write*/
#define HW_EMISTATICTIIME	(AS3310_EMI_BASE + 0x110)	/*control	write*/
#endif

/////////////////////////////////////////////////////////////
//emi
#define	HW_EMI_SCONR            0x80700000    
#define	HW_EMI_STMG0R           0x80700004
#define	HW_EMI_STMG1R           0x80700008
#define	HW_EMI_SCTLR  	        0x8070000C
#define	HW_EMI_SREFR  	        0x80700010
#define	HW_EMI_SCSLR0_LOW       0x80700014
#define	HW_EMI_SCSLR1_LOW       0x80700018
#define	HW_EMI_SCSLR2_LOW       0x8070001c
#define	HW_EMI_SCSLR3_LOW       0x80700020
#define	HW_EMI_SCSLR4_LOW       0x80700024
#define	HW_EMI_SCSLR5_LOW       0x80700028
#define	HW_EMI_SCSLR6_LOW       0x8070002c
#define	HW_EMI_SCSLR7_LOW       0x80700030

#define	HW_EMI_SMSKR0           0x80700054
#define	HW_EMI_SMSKR1           0x80700058
#define	HW_EMI_SMSKR2           0x8070005c
#define	HW_EMI_SMSKR3           0x80700060
#define	HW_EMI_SMSKR4           0x80700064
#define	HW_EMI_SMSKR5           0x80700068
#define	HW_EMI_SMSKR6           0x8070006c
#define	HW_EMI_SMSKR7           0x80700070

#define	HW_EMI_CSALIAS0_LOW     0x80700074
#define HW_EMI_CSALIAS1_LOW     0x80700078

#define HW_EMI_CSREMAP0_LOW     0x80700084
#define HW_EMI_CSREMAP1_LOW     0x80700088

#define HW_EMI_SMTMGR_SET0      0x80700094
#define HW_EMI_SMTMGR_SET1      0x80700098
#define HW_EMI_SMTMGR_SET2      0x8070009c

#define HW_EMI_FLASH_TRPDR      0x807000a0
#define HW_EMI_SMCTLR           0x807000a4
#define	HW_EMI_EXN_MODE_REG     0x807000ac

/* audio */
#define HW_AUDIO_OUT_CTRL       0x80048000
#define HW_AUDIO_IN_CTRL        0x8004c000
#define HW_AUDIO_OUT_PWRD_CTRL  0x80048070
#define HW_AUDIO_OUT_REF_CTRL   0x80048080

/*
 * SRAM
 */

#define ALPAS3310_SRAM_BASE 0x40000000
#define ALPAS3310_SRAM_SIZE 0x00008000


/*
 * SDRAM
 */

#define ALPAS3310_SDRAM_BASE 0x20000000
#define ALPAS3310_SDRAM_SIZE 0x04000000

/*
 * Memory chunk set aside for the Framebuffer in SRAM
 */
#define SRAM_FRAMEBUFFER_MEMORY ALPAS3310_SRAM_BASE

/*
Enthernet LAN91C96
*/
#define ALPAS3310_LAN91C96_BASE 0x30000000


/* Macros to access registers */
#define outb(v,p) *(volatile u8 *) (p) = v
#define outw(v,p) *(volatile u16 *) (p) = v
#define outl(v,p) *(volatile u32 *) (p) = v

#define inb(p)	*(volatile u8 *) (p)
#define inw(p)	*(volatile u16 *) (p)
#define inl(p)	*(volatile u32 *) (p)

#define as_outb(v,p) *(volatile u8 *) (p) = v
#define as_outw(v,p) *(volatile u16 *) (p) = v
#define as_outl(v,p) *(volatile u32 *) (p) = v

#define as_inb(p)	*(volatile u8 *) (p)
#define as_inw(p)	*(volatile u16 *) (p)
#define as_inl(p)	*(volatile u32 *) (p)


/*			CLK BASE			*/
#define AS3310_CLK_BASE         0x80040000

/*			GPMI BASE			*/
#define AS3310_GPMI_BASE	0x8000C000


#ifndef __ASSEMBLER__

#define PCIO_BASE		0

/*
 * RAM definitions
 */
#define MAPTOPHYS(a)		((unsigned long)(a) - PAGE_OFFSET)
#define KERNTOPHYS(a)		((unsigned long)(&a))
#define KERNEL_BASE		(0x20008000)
#endif

/* macro to get at IO space when running virtually */
#define IO_ADDRESS(x) ((x))


/*
 * ALPAS3310 UART3 Registers
 */

#define ALPAS_MPU_UART3_BASE  ALPAS3310_UART1_BASE /* UART3 through MPU bus */

/*
 * MMC/SD Host Controller Registers
 */


/*
 * Memory chunk set aside for the Framebuffer in SRAM
 */
#define SRAM_FRAMEBUFFER_MEMORY ALPAS3310_SRAM_BASE



/* ---------------------------------------------------------------------------
 *  ALPAS3310 GPIO (SHARED)
 * ---------------------------------------------------------------------------
 *
 */
#define GPIO_DATA_INPUT_REG	(ALPAS3310_GPIO_BASE)
#define GPIO_DATA_OUTPUT_REG	(ALPAS3310_GPIO_BASE)
#define GPIO_DIR_CONTROL_REG	(ALPAS3310_GPIO_BASE + 0x8)
#define GPIO_INT_CONTROL_REG	(ALPAS3310_GPIO_BASE + 0xc)
#define GPIO_INT_MASK_REG	(ALPAS3310_GPIO_BASE + 0x10)
#define GPIO_INT_STATUS_REG	(ALPAS3310_GPIO_BASE + 0x14)
#define GPIO_PIN_CONTROL_REG	(ALPAS3310_GPIO_BASE + 0x18)



#ifndef __ASSEMBLY__


typedef struct {
		AS3310_REG32		CTRL0[4]	 ;	//	0x00	R/W		
		AS3310_REG32		COMPARE[4]   ;  //	0x10	R/W 
		AS3310_REG32		CTRL1[4]     ;  //	0x20	R/W 
		AS3310_REG32		TIMING0[4]	 ;  //	0x30	R/W 
		AS3310_REG32		TIMING1[4]   ;  //	0x40	R/W 
		AS3310_REG32		TIMING2[4]   ;  //	0x50	R/W 
		AS3310_REG32		DATA[4]      ;  //	0x60	R/W 
		AS3310_REG32		STAT[4]      ;  //	0x70	R	
		AS3310_REG32		DEBUG[4]     ;  //	0x80	R
} /*__attribute__((__packed__))*/ AS3310_GPMI;


static inline AS3310_GPMI * const AS3310_GetBase_GPMI(void)
{
	return (AS3310_GPMI * const)AS3310_GPMI_BASE;
}


/****************************************************************************************************************
* #define DMA_SET_PKG(dma_cmd_pkg_addr,next_pkg_addr,ctrl_dword,buffer_addr,cmd_dword) \                        *
* do {   volatile long * temp_mem;                                  \                                           *
*     temp_mem=(volatile long*)dma_cmd_pkg_addr;               \                                                *
*     *temp_mem=next_pkg_addr;                            \                                                     *
*     temp_mem+=1;                                    \                                                         *
*     *temp_mem=ctrl_dword;                               \                                                     *
*     temp_mem+=1;                                            \                                                 *
*     *temp_mem=buffer_addr;                              \                                                     *
*     temp_mem+=1;                                        \                                                     *
*     *temp_mem=cmd_dword;                                    \                                                 *
* }   while(0)                                                                                                  *
*                                                                                                               *
* #define DMAStart(dma_ch_sema_addr,sema_data,dma_ch_nxtcmd_addr,nextcmd_addr)    \                             *
* do {       \                                                                                                  *
*     outl(0,DMA_CTRL0_ADDR);\                                                                                  *
*     outl(sema_data,dma_ch_sema_addr);\                                                                        *
*     outl(nextcmd_addr,dma_ch_nxtcmd_addr);\                                                                   *
* }   while(0)                                                                                                  *
*                                                                                                               *
* #define DMA_LCD_FLUSH(buffer)  \                                                                              *
* do {\                                                                                                         *
*     DMA_SET_PKG(DMA_CMD_PKG_ADDR1,DMA_CMD_PKG_ADDR2,0x960010c6,(unsigned long)buffer,0x21154b00);\            *
*     DMA_SET_PKG(DMA_CMD_PKG_ADDR2,DMA_CMD_PKG_ADDR3,0x960010c6,((unsigned long)buffer)+0x9600,0x21154b00);\   *
*     DMA_SET_PKG(DMA_CMD_PKG_ADDR3,DMA_CMD_PKG_ADDR4,0x960010c6,((unsigned long)buffer)+0x9600*2,0x21154b00);\ *
*     DMA_SET_PKG(DMA_CMD_PKG_ADDR4,DMA_CMD_PKG_ADDR5,0x960010c2,((unsigned long)buffer)+0x9600*3,0x21154b00);\ *
*    DMAStart(DMA_CH3_SEMA_ADDR,0x00000004,DMA_CH3_NXTCMDAR_ADDR,DMA_CMD_PKG_ADDR1);\                           *
* } while ( 0 );                                                                                                *
*                                                                                                               *
* #define DMA_LCD_FRAME_FLUSH(buffer)  \                                                                        *
* do {\                                                                                                         *
*     outl( 0x10000 ,HW_LCDIF_CTRL+8);    \                                                                     *
*     outl( 0x40000 ,HW_LCDIF_CTRL+8);    \                                                                     *
*     outl( 0x10000 ,HW_LCDIF_CTRL+4);    \                                                                     *
*     outl(0x43004200,HW_LCDIF_DATA);\                                                                          *
*     while ((0x01000000 & inl(HW_LCDIF_CTRL)) ==0 ) ; \                                                        *
*     outl(0x45004400,HW_LCDIF_DATA); \                                                                         *
*     outl( 0x10000 ,HW_LCDIF_CTRL+8); \                                                                        *
*     outl( 0x40000 ,HW_LCDIF_CTRL+4); \                                                                        *
*     outl(0xffff ,HW_LCDIF_CTRL+4); \                                                                          *
*     DMA_LCD_FLUSH(buffer);  \                                                                                 *
* } while ( 0 );                                                                                                *
*                                                                                                               *
* #define IS_DMA_LCD_FRAME_FLUSH_COMPLETE() ((inl(DMA_CH3_SEMA_ADDR)&0x00ff0000)==0)                            *
****************************************************************************************************************/

#endif




#endif /*__ALPSCALE_AS3310_H__*/

