/*
 * (C) Copyright 2007
 * Alpha Scale co,ltd.
 * He Yong <heyong@alpscale.com>
 * Configuation settings for the AS3310C Test board.
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#ifndef __CONFIG_H
#define __CONFIG_H

/*
 * If we are developing, we might want to start armboot from ram
 * so we MUST NOT initialize critical regs like mem-timing ...
 */

//#define CONFIG_INIT_CRITICAL		/*undef for developing */

/*
 * High Level Configuration Options
 * (easy to change)
 */
#define CONFIG_AS3310D      1
#define CONFIG_AS3310D_BOOT_HEADER      0  
/* 
we do not need boot header since we have the AS3310 loader
if you want to enable the header please change config in board/as3310/config.mk
*/

//#define DEBUG

#define CONFIG_ARM926EJS	1	/* This is an arm926ejs CPU core  */

#define ARCH_NUMBER_AS3310  15000
#define BOOT_PARAMETER_ADDR 0x20000100

#define VERSION_STRING "\n\n\n\t\t Alpha Scale U-Boot r674\n\t\tAll Rights Reserved.(2008)\n"

#undef CONFIG_NAND_FLASH
/* input clock of PLL */
/* the AS3310 has 24MHz input clock */
#define CONFIG_SYS_CLK_FREQ	24000000

#undef CONFIG_USE_IRQ	/* we don't need IRQ/FIQ stuff */
//#define CONFIG_USE_IRQ

#define CONFIG_MISC_INIT_R

#define CONFIG_CMDLINE_TAG	1	/* enable passing of ATAGs  */
#define CONFIG_SETUP_MEMORY_TAGS	1

/*
 * Size of malloc() pool
 */
#define CFG_MALLOC_LEN	(CFG_ENV_SIZE + 128*1024)
#define CFG_GBL_DATA_SIZE	128	/* size in bytes reserved for initial data */

 /* default nand flash cen -> 0 */
#define CONFIG_NAND_CEN 0          

/*  CPU CLK Config  for AS3310C  */

/*  if CPU_CLK is configured 24 MHz, CPU will work at 24MHz directly */
/*     CPU  Clock  Frequency */    
#define CPU_CLK     180  
/*  APBH Bus clock divided from CPU clock */
#define HCLK_DIV_CPU 2
/*  EMI module Bus clock divided from APBH Bus clock */
#define EMI_DIV_HCLK 2
/*  APBX Bus clock divided from 24M  */
#define XCLK_DIV_CRYXTAL 1
/*  GPMI module Bus clock divided from CPU clock */
#define GPMI_CLK_DIV 3

#define NAND_DELAY 1000

/*
 * Hardware drivers
 */
/*
*/
#if 0
#define CONFIG_DRIVER_LAN91C96
#define CONFIG_LAN91C96_BASE ALPAS3310_LAN91C96_BASE
#define CONFIG_LAN91C96_EXT_PHY
#endif

/*
 * select serial console configuration
 */
#define CONFIG_SERIAL1	1	/* we use SERIAL 1 on OMAP5912 OSK */
/*
 * add mac
 */
//#define CONFIG_MAC9260

/* allow to overwrite serial and ethaddr */
#define CONFIG_ENV_OVERWRITE
#define CONFIG_CONS_INDEX	1
#define CONFIG_BAUDRATE	115200
#define CFG_BAUDRATE_TABLE	{ 9600, 19200, 38400, 57600, 115200 }
#define CONFIG_COMMANDS	(CONFIG_CMD_DFL | CFG_CMD_DHCP | CFG_CMD_PING)        /* add ping */
#define CONFIG_BOOTP_MASK	CONFIG_BOOTP_DEFAULT

/* this must be included AFTER the definition of CONFIG_COMMANDS (if any) */
#include <cmd_confdefs.h>
#include <configs/alpscale_as3310.h>

//#define CFG_HUSH_PARSER
//#define CFG_PROMPT_HUSH_PS2 ">"

#define CONFIG_BOOTDELAY	5
//#define CONFIG_BOOT_RETRY_TIME 3
//#define CONFIG_AUTOBOOT_KEYED
#define CONFIG_AUTOBOOT_PROMPT "Auto Boot in 0.1 Seconds..."
//#define CONFIG_AUTOBOOT_DELAY_STR
#define CONFIG_AUTOBOOT_STOP_STR " "

//#define CONFIG_AS3310_AUTOBOOT
//#define CONFIG_AS3310_AUTOBOOT_DELAY 0x200000
//#define CONFIG_AS3310_AUTOBOOT_DELAY 0x10000

#define CONFIG_AS3310_BOOT_LOGO 0x001d0000

//#define CONFIG_AS3310_AUTOBOOT_KERNEL "nandr 0x00080000 0x00100000 0x20400000"
//#define CONFIG_AS3310_AUTOBOOT_ROOTFS "nandr 0x00300000 0x00200000 0x20600000"
//#define CONFIG_AS3310_AUTOBOOT_BOOT "bootm 0x20400000"
#define CONFIG_AS3310_AUTOBOOT_BOOT "bootm 0x20800000"

#define CONFIG_BOOTARGS	"mem=16M console=ttyS0,115200n8 root=/dev/mtdblock1 init=/initrc lcd=rgb24 nftlfmt=0 mtdparts=NAND:2M@5M(logo)ro,57M@7M(root)ro,-@64M(NFTL2K)"
//#define CONFIG_BOOTARGS_CMD	"setenv bootargs mem=%dM console=ttyS0,115200n8 root=/dev/mtdblock1 init=/initrc lcd=rgb24 nftlfmt=%d mtdparts=NAND:2M@5M(logo)ro,57M@7M(root)ro,128M@64M(yaffs),-@192M(NFTL2K)"
//#define CONFIG_BOOTARGS_CMD	"setenv bootargs mem=%dM console=ttyS0,115200n8 root=/dev/mtdblock1 init=/initrc lcd=rgb24 nftlfmt=%d mtdparts=NAND:4M@10M(logo)ro,57M@14M(root)ro,128M@71M(yaffs),-@199M(NFTL2K)"
//#define CONFIG_BOOTARGS_CMD	"setenv bootargs mem=%dM console=ttyS4,115200n8 root=/dev/mtdblock1 init=/linuxrc lcd=rgb24 nftlfmt=%d mtdparts=NAND:4M@10M(logo)ro,60M@14M(root)ro,-@74M(yaffs)"	//cramfs
//#define CONFIG_BOOTARGS_CMD	"setenv bootargs mem=%dM console=ttyS0,115200n8 root=/dev/mtdblock1 init=/initrc lcd=rgb24 nftlfmt=%d mtdparts=NAND:4M@10M(logo),60M@14M(root),128M@74M(yaffs),-@202M(NFTL2K)"	//yaffs2
//#define CONFIG_BOOTARGS_CMD	"setenv bootargs mem=%dM console=ttyS4,115200n8 root=/dev/mtdblock1 init=/init mtdparts=NAND:2M@0M(boot),7M@2M(root),-@16M(yaffs)"
//#define CONFIG_BOOTARGS_CMD	"setenv bootargs mem=%dM console=tty root=/dev/mtdblock1 init=/init lcd=rgb24 nftlfmt=%d mtdparts=NAND:5M@1M(boot)ro,10M@6M(root)ro,-@16M(yaffs)"	//cramfs

//#define CONFIG_BOOTARGS	"mem=16M console=ttyS0,115200n8 bootdelay=3 root=/dev/mmcblk0p1 rw rootwait   noinitrc lcd=rgb24 "
//#define CONFIG_BOOTARGS_CMD	"setenv bootargs mem=%dM console=ttyS0,115200n8 bootdelay=3 root=/dev/mmcblk0p1 rw rootwait   init=/initrc lcd=rgb24 "

//#define CONFIG_BOOTARGS	"mem=16M console=ttyS0,115200n8 root=/dev/nfs init=/linuxrc nfsroot=192.168.1.188:/home/nfsroot/rootfs ip=192.168.1.156:192.168.1.188:192.168.1.1:255.255.255.0 rw "
//#define CONFIG_BOOTARGS_CMD	"setenv bootargs mem=%dM console=ttyS0,115200n8 root=/dev/nfs init=/linuxrc nfsroot=192.168.1.188:/home/nfsroot/rootfs ip=192.168.1.156:192.168.1.188:192.168.1.1:255.255.255.0 rw "

#define CONFIG_NETMASK	255.255.255.0	/* talk on MY local net */
//#define CONFIG_IPADDR	192.168.1.134	/* static IP I currently own */
#define CONFIG_IPADDR	192.168.100.11	/* static IP I currently own */
#define CONFIG_GATEWAYIP	192.168.100.1	/* current Gateway IP of my dev pc */
#define CONFIG_SERVERIP	192.168.100.51/* current IP of my dev pc */
//#define CONFIG_ETHADDR 00:e0:4c:5c:00:00
#define CONFIG_ETHADDR 00:e0:a3:a4:98:67
#define CONFIG_BOOTFILE	"uImage"	/* file to load */
#define CONFIG_NET_MULTI

#if (CONFIG_COMMANDS & CFG_CMD_KGDB)
#define CONFIG_KGDB_BAUDRATE	115200	/* speed to run kgdb serial port */
#define CONFIG_KGDB_SER_INDEX	1	/* which serial port to use */
#endif

/*
 * Miscellaneous configurable options
 */
#define CFG_LONGHELP	/* undef to save memory     */
#define CFG_PROMPT	"ASAP18xx#"	/* Monitor Command Prompt   */
#define CFG_CBSIZE	256		/* Console I/O Buffer Size  */
/* Print Buffer Size */
#define CFG_PBSIZE	(CFG_CBSIZE+sizeof(CFG_PROMPT)+16)
#define CFG_MAXARGS	16		/* max number of command args   */
#define CFG_BARGSIZE	CFG_CBSIZE	/* Boot Argument Buffer Size    */

#define CFG_MEMTEST_START	0x20000000	/* memtest works on */
#define CFG_MEMTEST_END	0x24000000	/* 64 MB in DRAM    */

#undef	CFG_CLKS_IN_HZ		/* everything, incl board info, in Hz */

#define CFG_LOAD_ADDR	0x20000000	/* default load address */

/* The AS3310 has 2 timers, they can be driven by the RefClk (12Mhz) or by
 * DPLL1. This time is further subdivided by a local divisor.
 */
#define CFG_TIMERBASE	0x80000040	/* use timer 1 */
#define CFG_PVT	10	/* 2^(pvt+1), divide by 2048 */
#define CFG_HZ	((CONFIG_SYS_CLK_FREQ)/(2 << CFG_PVT))

/*-----------------------------------------------------------------------
 * Stack sizes
 *
 * The stack sizes are set up in start.S using the settings below
 */
#define CONFIG_STACKSIZE	(128*1024)	/* regular stack */
#ifdef CONFIG_USE_IRQ
#define CONFIG_STACKSIZE_IRQ	(4*1024)	/* IRQ stack */
#define CONFIG_STACKSIZE_FIQ	(4*1024)	/* FIQ stack */
#endif

/*-----------------------------------------------------------------------
 * Physical Memory Map
 */
#define CONFIG_NR_DRAM_BANKS	1	/* we have 1 bank of DRAM */
#define PHYS_SDRAM_1	0x20000000	/* SDRAM Bank #1 */

/*  SDRAM Macro */
#define SDRAM_64MB          0
#define SDRAM_32MB          0
#define SDRAM_16MB          1
#define SDRAM_8MB           0

#if SDRAM_64MB
#define PHYS_SDRAM_1_SIZE	0x04000000	/* 64 MB */
#define SDRAM_ROW 13
#define SDRAM_COL 10  
#endif 

#if SDRAM_32MB
#define PHYS_SDRAM_1_SIZE	0x02000000	/* 32 MB */
#define SDRAM_ROW 13
#define SDRAM_COL 9  
#endif 

#if SDRAM_16MB
#define PHYS_SDRAM_1_SIZE	0x01000000	/* 16 MB */
#define SDRAM_ROW 12
#define SDRAM_COL 9  
#endif 

#if SDRAM_8MB
#define PHYS_SDRAM_1_SIZE	0x00800000	/* 8 MB */
#define SDRAM_ROW 12
#define SDRAM_COL 8  
#endif 
#define SDRAM_LAGECY 0x30


#define PHYS_FLASH_1	0x00000000	/* Flash Bank #1 */

#define CFG_FLASH_BASE	PHYS_FLASH_1
#define __HAVE_NAND_FLASH_

/*-----------------------------------------------------------------------
 * FLASH and environment organization
 */
#define CFG_MAX_FLASH_BANKS	1	/* max number of memory banks */
#define PHYS_FLASH_SIZE	0x200000	/* 2MB */
#define CFG_MAX_FLASH_SECT	(35)	/* max number of sectors on one chip */
/* addr of environment */
#define CFG_ENV_ADDR	(CFG_FLASH_BASE + 0x020000)

/* timeout values are in ticks */
#define CFG_FLASH_ERASE_TOUT	(20*CFG_HZ)	/* Timeout for Flash Erase */
#define CFG_FLASH_WRITE_TOUT	(20*CFG_HZ)	/* Timeout for Flash Write */

#define CFG_ENV_IS_IN_FLASH	0
#define CFG_ENV_SIZE	0x20000	/* Total Size of Environment Sector */
#define CFG_ENV_OFFSET	0x20000	/* environment starts here  */

/*  Boot Info Header  for AS3310C   */


#define Struct_Section_BOOT_INFO  __attribute__ ((section (".boot_info_header")))

#define BOOT_INFO_MAGIC     0x544F4F42// BOOT
#define HEADER_SIZE     0x30
#define LOCATION_NAND   0
#define LOCATION_I2C    1
#define LOCATION_NOR    2
#define LOCATION_UART   3
#define LOCATION_USB    4
#define LOCATION_ROM    5
#define CODE_START      0x20280000
#define CODE_SIZE_BOOT_INFO   0x20000


    #ifndef __ASSEMBLY__
    
        #if CONFIG_AS3310C
        
        typedef struct boot_info_struct {
            unsigned long     Magic;   //  0x544F4F42 BOOT
            unsigned long     m_u32StartMode; // 0 == Nand , 1 = I2C  , 2 = NorFlash , 3 = UART , 4 = USB , 5 = ROM default = USB
            unsigned long     src_addr;               // the address we got the code
            unsigned long     dest_addr;              // dest address , where the app will run
            unsigned long     n_bytes;                // code length to recloate
            unsigned long     starting_sector;        // starting sector, for nand
            unsigned long     starting_offset;        // entry offset
            unsigned long		(*run)(int, char *[]);  // main entry
            unsigned long    Nand_table_location; // 0 == Nand , 1 = I2C  , 2 = NorFlash , 3 = UART , 4 = USB , 5 = ROM default = USB
            unsigned long    Nand_table_offset;   // where to locate the nand_table
            unsigned char      sdram_row;   
            unsigned char      sdram_col; 
            unsigned char     sdram_lagecy;    // 0x20 or 0x30
            unsigned char     resv_8; 
            unsigned long    resv_32;    
                }__attribute__((packed)) boot_info;
        #endif  //CONFIG_AS3310C
    
        #if CONFIG_AS3310D
        typedef unsigned long U32;
        
        typedef union _nand_tag{
            struct _nand_tag_{
                U32 Auto_search         : 1;    // Searching in nand table, clear to use settings above
                U32 ECC_Enable          : 1;    // Enable ECC when relocating code
                U32 nand_cen            : 2;    // cen number of nand chip for boot
                U32 page_shift          : 4;    // nand page shift
                U32 col_cycle           : 4;    // nand col cycles, zero for some mlc nand
                U32 page_cycle          : 4;    // nand page cycles
                U32 block_page_shift    : 4;    // Block page shift.
                U32 starting_block      : 12;   // 12b boot block no.
            } __attribute__((packed)) field ;
            U32 value;
        }nand_tag;
        
        
        typedef union _config_tag{
            struct _config_tag_{
                U32 usb_external_phy        : 1;    // set:external phy
                U32 SBZ2                    : 1;    // should be zero
                U32 nand_table_dev          : 2;    // 1 = I2C  , 2 = NorFlash , 3 = ROM
                U32 sdram_latency           : 1;    // set: 0x2 , clear 0x3
                U32 SBZ1                    : 1;    // should be zero
                U32 sdram_cen               : 2;    // cen number of sdram chip, default 10b
                U32 sdram_col               : 4;    // nand page shift
                U32 sdram_row               : 4;    // nand col cycles, zero for some mlc nand
                U32 source_device           : 4;    // 0 == Nand , 1 = I2C  , 2 = NorFlash , 3 = ROM , 4 = USB , 5 = UART
                U32 nand_table_offset       : 12;   // 12b where to locate the nand_table
            } __attribute__((packed)) field ;
            U32 value;
        }config_tag;
        
        
         /*  Length 0x20*/
        typedef struct boot_info_struct_d_ {
            U32         Magic;                  //  0x544F4F42 BOOT
            U32         src_addr;               // the address we got the code
            U32         dest_addr;              // dest address , where the app will run
            U32         n_bytes;                // code length to recloate
            U32         starting_offset;        // entry offset
            nand_tag    nand_t;                 // nand tag
            config_tag   config_t;              // config tag
            U32          header_checksum;       // Header CheckSum
                }__attribute__((packed)) boot_header;
        
        #endif  //CONFIG_AS3310D


        /*****************************************************************
                                as3310_keypad
                        row0(O0)----0-------1-------2
                                    |       |       |
                                    |       |       |
                        row1(O1)----4-------5-------6
                                    |       |       | 
                                    |       |       | 
                        row2(O2)----8-------9-------10
                                    |       |       | 
                                    |       |       |    
                              col0(I0) col1(I1) col2(I2) 
         *****************************************************************/
        
        struct as3310_kp_gpios_row{
            int port;
            int pin;
        };
        
        struct as3310_kp_gpios_col{
            int port;
            int pin;
        };
        
        #define COL_KP 3
        #define ROW_KP 3
        
        struct as3310_kp_platform_data {
        
            int keypad_keycode[COL_KP][ROW_KP];
        	struct as3310_kp_gpios_row gpio_rows[ROW_KP];
        	struct as3310_kp_gpios_col gpio_cols[COL_KP];
        };
    #endif //__ASSEMBLY__
#endif							/* __CONFIG_H */
