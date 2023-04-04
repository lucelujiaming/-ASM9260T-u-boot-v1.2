/*
 * (C) Copyright 2002
 * Sysgo Real-Time Solutions, GmbH <www.elinos.com>
 * Marius Groeger <mgroeger@sysgo.de>
 *
 * (C) Copyright 2002
 * David Mueller, ELSOFT AG, <d.mueller@elsoft.ch>
 *
 * (C) Copyright 2003
 * Texas Instruments, <www.ti.com>
 * Kshitij Gupta <Kshitij@ti.com>
 * 
 * (C) Copyright 2004
 * Texas Instruments, <www.ti.com>
 * Rishi Bhattacharya <rishi@ti.com> 
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

#include <common.h>
#include <command.h>
#include <./configs/as3310.h>
#include "clock_control.h"


void flash__init (void);
void ether__init (void);
void set_muxconf_regs (void);
void peripheral_power_enable (void);

#define COMP_MODE_ENABLE ((unsigned int)0x0000EAEF)

static inline void delay (unsigned long loops)
{
	__asm__ volatile ("1:\n"
		"subs %0, %1, #1\n"
		"bne 1b":"=r" (loops):"0" (loops));
}

/*
 * Miscellaneous platform dependent initialisations
 */

int board_init (void)
{
	DECLARE_GLOBAL_DATA_PTR;

	/* arch number of AS3310-Board */
	gd->bd->bi_arch_number = ARCH_NUMBER_AS3310;

	/* adress of boot parameters */
	gd->bd->bi_boot_params = BOOT_PARAMETER_ADDR; // 0x20000100;

/* this speeds up your boot a quite a bit.  However to make it
 *  work, you need make sure your kernel startup flush bug is fixed.
 *  ... rkw ...
 */
        icache_enable ();
    //clk_init();
    //mac9260_eth_initialize(0,NULL,NULL);
    //audio_reference_init();
    //gpio_init();
    //clk_switch(121,2,2,2,1);
#if 0
   outl(0x000301e0,0x80040000);//USE 24M & PLL=480
    outl(0x00040000,0x80040004);//EN PLL CLK
    outl(2,0x80040020);//CPU CLK DIV
    outl(0x3f030002,0x80040030);//HCLK DIV
    outl(4,0x800400b0);//EMI CLK DIV

    outl(4,0x80040080);//SSP CLK

    while((inl(0x80040010)&0x80000000) != 0x80000000) {
        ;
    }//wait PLL LOCK

    outl(0x05111110,0x800200c0);//EMI PARAM
    outl(0x00020000,(0x80040000+8));//480 PLL EN
#endif
	return 0;
}



// #ifdef CONFIG_KEYBOARD_AS3310_10
 struct  as3310_kp_platform_data kp_data = {
     .keypad_keycode ={
    // {KEY_KPENTER    ,KEY_A          ,KEY_F2},
    // {KEY_UP         ,KEY_RIGHT      ,KEY_B },
    // {KEY_LEFT       ,KEY_DOWN       ,KEY_F1}
     },
     .gpio_rows  = {{0,12}, {0,13},{0,14}},
     .gpio_cols  = {{3,14}, {3,15}, {3,16}},
 };
 
// #endif

void gpio_init(){

    set_pin_mux(0,12,3);
    set_pin_mux(0,13,3);
    set_pin_mux(0,14,3);

    set_pin_mux(3,14,3);
    set_pin_mux(3,15,3);
    set_pin_mux(3,16,3);

/* RD:byzsma on: Sat, 04 Jul 2015 17:06:14 +0800
 * EMMC pin init.
 */
    set_pin_mux(14,0,0);
    clear_GPIO(14,0);
    set_pin_mux(14,1,0);
    clear_GPIO(14,1);
    set_pin_mux(14,2,0);
    clear_GPIO(14,2);
    set_pin_mux(14,3,0);
    clear_GPIO(14,3);
    set_pin_mux(14,4,0);
    clear_GPIO(14,4);
    set_pin_mux(14,5,0);
    clear_GPIO(14,5);
// End of RD:byzsma
}


int misc_init_r (void)
{
	/* currently empty */
	return (0);
}

/******************************
 Routine:
 Description:
******************************/
void flash__init (void)
{
}
/*************************************************************
 Routine:ether__init
 Description: Pinassign
*************************************************************/
void ether__init (void)
{

}

/******************************
 Routine:
 Description:
******************************/
int dram_init (void)
{
    int row,col;

	DECLARE_GLOBAL_DATA_PTR;
    row = (inl(HW_EMI_SCONR)>>5) & 0xf;
    col = (inl(HW_EMI_SCONR)>>9) & 0xf;

	gd->bd->bi_dram[0].start = PHYS_SDRAM_1;
	gd->bd->bi_dram[0].size = (1<<(row+col+5)); /* detect RAM config */

	return 0;
    
}



void audio_reference_init(){
/*  generate analog reference voltage / current */
    outl(0x0,       HW_AUDIO_OUT_CTRL); // HW_AUDIO_OUT_CTRL
    outl(0x0,       HW_AUDIO_IN_CTRL); // HW_AUDIO_IN_CTRL
    outl(0x01001111,HW_AUDIO_OUT_PWRD_CTRL); // HW_AUDIO_OUT_PWRD_CTRL
    outl(0x00003770,HW_AUDIO_OUT_REF_CTRL); // HW_AUDIO_OUT_REF_CTRL
}

#if CONFIG_AS3310D_BOOT_HEADER

/*
UART U-boot for AS3310D
*/
boot_header __uart_header_ Struct_Section_BOOT_INFO = {
    .Magic = BOOT_INFO_MAGIC,
    .dest_addr = 0x20280000,
    .n_bytes = 0x20000,  /*   Auto configured in astty */
    .src_addr = 0, /*   not checked in UART Mode */
    .starting_offset = 0,
    .nand_t = {
        .field = {        
            .starting_block      = 1 ,   // 12b boot block no.
            .block_page_shift    = 6  ,  // Block page shift.
            .page_cycle          = 2  ,  // nand page cycles
            .col_cycle           = 2  ,  // nand col cycles, zero for some mlc nand
            .page_shift          = 11  ,  // nand page shift
            .nand_cen            = 0  ,  // cen number of nand chip for boot
            .ECC_Enable          = 0  ,  // Enable ECC when relocating code
            .Auto_search         = 1  ,  // Searching in nand table, clear to use settings above
        },
    },
    .config_t = {
        .field = {        
            .nand_table_offset      = 3*sizeof(boot_header) ,    // 12b where to locate the nand_table
            .source_device          = LOCATION_NAND  ,  // 0 == Nand , 1 = I2C  , 2 = NorFlash , 3 = ROM , 4 = USB , 5 = UART
            .sdram_row              = SDRAM_ROW  ,     // nand col cycles, zero for some mlc nand         
            .sdram_col              = SDRAM_COL  ,      // nand page shift                                 
            .sdram_cen              = 2  ,      // cen number of sdram chip, default 10b           
            .SBZ1                   = 0  ,      // should be zero                                  
            .sdram_latency          = 0  ,      // set: 0x2 , clear 0x3                            
            .nand_table_dev         = LOCATION_I2C  ,  // 1 = I2C  , 2 = NorFlash , 3 = ROM    
            .SBZ2                   = 0,        // should be zero   
            .usb_external_phy       = 0,        // set:external phy 
        },                                                           
    },                                                                
    .header_checksum = 0x856FC489,  /*   Auto configured in astty */
};

#endif



