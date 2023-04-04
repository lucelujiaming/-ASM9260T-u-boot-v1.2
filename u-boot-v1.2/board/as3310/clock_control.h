

/* =======================================================================
                                                                        
Alpha Scale AP18XX clock control                                        
He Yong, AlpScale Software Engineering, heyong@alpscale.com             
                                                                        
Clock Control Header file                                               

This file Manage 
CPU / AHB / APB and other devices Clock Frequency and Gating

                                                                        
Change log:                                                             
                                                                        
------------------- He Yong 2008-04-23  ----------------------                                                                    
    -- Create file                                                      
                                                                        
======================================================================= */ 

#include <common.h>
#include <./configs/as3310.h>

/*
 * ================  Type Defs  ================
 */
    
/*			clock register BASE			*/
#define AS3310_CLK_BASE         0x80040000

#define AS3310_PLL_MUX_CTRL0_180MHZ     (1<<15) /* b15 = 1*/    
#define AS3310_PLL_MUX_CTRL0_24MHZ      (1<<17) /* b15 = 0, b17 = 1*/    
#define AS3310_PLL_MUX_CTRL0_480MHZ     (1<<17) /* b15 = 0, b17 = 0*/    

#define AS3310_XCLOCK_DEFAULT 24

/* Bit Field in XTALCLKCTRL 	0x50	R/W */
#define UART_Clock_Gate             (1<<31)
#define Digi_Filter_Clock_Gate      (1<<30)
#define PWM_Clock_Gate              (1<<29)
#define DRI_Clock_Gate              (1<<28)
#define DIG_Clock_Gate              (1<<27)
#define TIMROT_Clock_Gate           (1<<26)
#define EXRAM_Clock_Gate            (1<<25)
#define LRADC_Clock_Gate            (1<<24)
#define UTMI_120MHz_Clock_Gate      (1<<31)
#define UTMI_30MHz_Clock_Gate       (1<<30)


typedef struct _ap18xx_clockreg_ {
		AS3310_REG32		PLLCTRL0[4]	 ;	//	0x00	R/W		
		AS3310_REG32		PLLCTRL1[4]   ;  //	0x10	R/W 
		AS3310_REG32		CPUCLKCTRL[4]     ;  //	0x20	R/W 
		AS3310_REG32		HBUSCLKCTRL[4]	 ;  //	0x30	R/W 
		AS3310_REG32		XBUSCLKCTRL[4]   ;  //	0x40	R/W 
		AS3310_REG32		XTALCLKCTRL[4]   ;  //	0x50	R/W 
		AS3310_REG32		OCRAMCLKCTRL[4]      ;  //	0x60	R/W 
		AS3310_REG32		UTMICLKCTRL[4]      ;  //	0x70	R/W	
		AS3310_REG32		SSPCLKCTRL[4]     ;  //	0x80	R/W
		AS3310_REG32		GPMICLKCTRL[4]   ;  //	0x90	R/W 
		AS3310_REG32		SPDIFCLKCTRL[4]      ;  //	0xA0	R/W 
		AS3310_REG32		EMICLKCTRL[4]      ;  //	0xB0	R/W	
		AS3310_REG32		IRCLKCTRL[4]     ;  //	0xC0	R/W
		AS3310_REG32		CLKCTRL_PLL180[4]     ;  //	0xD0	R/W   LCD
} /*__attribute__((__packed__))*/ AS3310_CLK;


static inline AS3310_CLK * AS3310_GetBase_CLK(void)
{
	return (AS3310_CLK * )IO_ADDRESS(AS3310_CLK_BASE);
}

/* clock config data struct  */

typedef struct _clock_cfg_{
    int pll;
    int cpu_pll;
    int gpmi_pll;
    int ssp_pll;
    int spdif_pll;
    int hbus_cpu;
    int emi_hbus;
    int xbus_cryst;
    int xtal_cfg; // this refer to reg : XTALCLKCTRL[4]   ;  //	0x50	R/W 
} clock_cfg_t ;


/* 
 * Struct Clock info 
 * this is the main structure of clock control
 * including config datas and operations
*/
typedef struct _clock_info_{
    /* configs */
    clock_cfg_t clk_cfg;
    /* clock ops */
    void (*clock_sw)(clock_cfg_t * clk_cfg);
    void (*clock_dump)();
    void (*clock_gate_xtal)(clock_cfg_t * clk_cfg);

} clock_info_t ;

extern clock_info_t * clock_info_ptr;

static inline clock_info_t * get_clock_info(void)
{
	return (clock_info_ptr);
}

void clk_init(void);
void do_clk_switch(clock_cfg_t * clk_cfg);
void do_clock_dump(void);
void do_gate_xtal(clock_cfg_t * clk_cfg);
