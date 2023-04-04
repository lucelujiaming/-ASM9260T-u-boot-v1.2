

/* =======================================================================
                                                                        
Alpha Scale AP18XX clock control                                        
He Yong, AlpScale Software Engineering, heyong@alpscale.com             
                                                                        
Clock Control Source file                                               

This file Manage 
CPU / AHB / APB and other devices Clock Frequency and Gating

                                                                        
Change log:                                                             
                                                                        
------------------- He Yong 2008-04-23  ----------------------                                                                    
    -- Create file                                                      
                                                                        
======================================================================= */ 


#include <common.h>
#include <command.h>
#include <./configs/as3310.h>
#include "clock_control.h"

#define clk_putc(x) if(1) putc(x)
/*

 ================  Basic  Funtions  ================

*/

clock_info_t clock_info = {

    .clk_cfg = {

       .pll = 480,
       .cpu_pll = 2,    // 180MHz
       .gpmi_pll = 6,   // 60MHz
       .ssp_pll = 5,    // 90MHz
       
        .spdif_pll = (1<<31)|12 ,  // stop it   
        .hbus_cpu = 2,   
        .emi_hbus = 1,   
        .xbus_cryst = 1, // 24MHz
        .xtal_cfg = 0x5f800000,// let UART & PWM Open
    },

    .clock_dump = do_clock_dump,
    .clock_gate_xtal = do_gate_xtal,
    .clock_sw = do_clk_switch,
};

/* globle pointer */
clock_info_t * clock_info_ptr;

void do_clk_switch(clock_cfg_t * clk_cfg){
    AS3310_CLK * clk;
    clk = AS3310_GetBase_CLK();
    clk->PLLCTRL0[REG_SET] = AS3310_PLL_MUX_CTRL0_24MHZ;// choose 24MHz;
    clk_putc('A');
    clk->PLLCTRL0[REG_CLR] = AS3310_PLL_MUX_CTRL0_180MHZ;// choose 24 or 480;
    clk_putc('B');

    clk->CPUCLKCTRL[0] = clk_cfg->cpu_pll;//HCLK devided by x from CPU CLK
    clk->HBUSCLKCTRL[0] = clk_cfg->hbus_cpu;//HCLK devided by x from CPU CLK
    clk->XBUSCLKCTRL[0] = clk_cfg->xbus_cryst;//XCLK devided by x from CRYSTAL 24M
    clk->EMICLKCTRL[0] = clk_cfg->emi_hbus;// EMI devided by x from HCLK

    clk->SSPCLKCTRL[0] = clk_cfg->ssp_pll;// SSP devided by PLL CLK                                                   
    clk->SPDIFCLKCTRL[0] = clk_cfg->spdif_pll;// SPDIF devided by PLL CLK                                         
    clk->GPMICLKCTRL[0] = clk_cfg->gpmi_pll;// GPMI devided by x from PLL CLK                                                   


    /* Main PLL , USB High Speed Enabled */
     if (clk_cfg->pll==480) {
         clk->PLLCTRL0[0] = 0x000701e0;                  // 480MHz
         clk->PLLCTRL1[REG_SET] = 0x20000000;            // divide by 4        
         clk_putc('C');
         while ((clk->PLLCTRL1[0] & (1<<31)) ==0);       // wait
         clk_putc('D');
         clk->UTMICLKCTRL[0] = 0x0;                      // enable 30m 120m clk        
         clk->PLLCTRL0[REG_CLR] = 0x00028000;            /* b15 = 0, b17 = 0 , USE Main 480 PLL */  
         clk->CLKCTRL_PLL180[REG_CLR] = 0x00001000;    // Internal Core-180 Power Down
     }
     /* Main PLL , Full Performance, USB Disabled*/
     else if (clk_cfg->pll==360) {
         clk->PLLCTRL0[0] = 0x00070168;                  // 360MHz
         clk_putc('E');
         while ((clk->PLLCTRL1[0] & (1<<31)) ==0);       // wait
         clk_putc('F');
         clk->UTMICLKCTRL[0] = 0xc0000000;               // disable 30m 120m clk        
         clk->PLLCTRL0[REG_CLR] = 0x00028000;            /* b15 = 0, b17 = 0 , USE Main 480 PLL */  
         clk->CLKCTRL_PLL180[REG_CLR] = 0x00001000;      // Internal Core-180 Power Down
     }
     /* Main PLL , USB Full Speed Enabled */
     else if (clk_cfg->pll==240) {
         clk->PLLCTRL0[0] = 0x000700f0;                  // 240MHz
         clk->PLLCTRL1[REG_CLR] = 0x20000000;            // divide by 2        
         while ((clk->PLLCTRL1[0] & (1<<31)) ==0);       // wait
         clk->UTMICLKCTRL[0] = 0x0;                      // enable 30m 120m clk        
         clk->PLLCTRL0[REG_CLR] = 0x00028000;            /* b15 = 0, b17 = 0 , USE Main 480 PLL */  
         clk->CLKCTRL_PLL180[REG_CLR] = 0x00001000;    // Internal Core-180 Power Down
     }

     /* Core PLL , USB Full Speed Stable */
     else if (clk_cfg->pll==241){
         clk->CLKCTRL_PLL180[0] = 0x404a;                        // 1:10  240 MHz
         clk->CLKCTRL_PLL180[REG_SET] = 0x00001000;              // Internal Core-180 Power on
         clk->PLLCTRL1[REG_CLR] = 0x20000000;                    // divide by 2        
         clk_putc('G');
         while ((clk->CLKCTRL_PLL180[0] & (1<<13)) ==0);
         clk_putc('H');
         clk->PLLCTRL0[REG_SET] = AS3310_PLL_MUX_CTRL0_180MHZ;   /* b15 = 1, Internal PLL */  
         clk->UTMICLKCTRL[0] = 0x00000001;                       // utmi use 180 PLL
         clk->PLLCTRL0[2] = (1<<16);                             // Main 480 PLL Power Down
     }

     /* Core PLL 180,  USB Disabled */
     else if (clk_cfg->pll==180){
         clk->CLKCTRL_PLL180[0] = 0x408f;                        // 24*15/2==180 MHz
         clk->CLKCTRL_PLL180[REG_SET] = 0x00001000;              // Internal Core-180 Power on
         clk->PLLCTRL1[REG_CLR] = 0x20000000;                    // divide by 2        
         clk_putc('G');
         while ((clk->CLKCTRL_PLL180[0] & (1<<13)) ==0);
         clk_putc('H');
         clk->PLLCTRL0[REG_SET] = AS3310_PLL_MUX_CTRL0_180MHZ;   /* b15 = 1, Internal PLL */  
         clk->UTMICLKCTRL[0] = 0x00000001;                       // utmi use 180 PLL
         clk->PLLCTRL0[2] = (1<<16);                             // Main 480 PLL Power Down
     }

     /* Direct External Crystal 24MHz */
     else{
         /*24 MHz */
         clk_putc('I');
         clk->PLLCTRL0[2] = (1<<16);                     // Main 480 PLL Power Down
         clk->CLKCTRL_PLL180[REG_SET] = 0x00001000;      // Internal Core-180 Power Down
         clk->UTMICLKCTRL[0] = 0xc0000000;               // disable 30m 120m clk        
     }
     clk_putc('J');

}

void do_gate_xtal(clock_cfg_t * clk_cfg){
    AS3310_CLK * clk;
    clk = AS3310_GetBase_CLK();
    clk->XTALCLKCTRL[REG_VAL] = clk_cfg->xtal_cfg;
}


void do_clock_dump()
{
	int pll, cpu,hbus;
    int temp_emi_time_val;
    AS3310_CLK * clk;

    clk = AS3310_GetBase_CLK();
    
    // if ((argc < 6)||(argc > 6)) {puts("Invalid args.");puts(cmdtp->usage);putc('\n');return 0;}  

    printf("AS3310 Dump Clock:\n");

    if ((clk->PLLCTRL0[0] & AS3310_PLL_MUX_CTRL0_180MHZ) != 0) {
        // use 180 pll
        pll = AS3310_XCLOCK_DEFAULT * (clk->CLKCTRL_PLL180[0]&0x3f) / 
        ((clk->CLKCTRL_PLL180[0]>>6)&0x3f) ;
        printf("PLL: USE <PLL-Core180> at %d MHz\n",pll);
    }
    else if (clk->PLLCTRL0[0] & AS3310_PLL_MUX_CTRL0_24MHZ) {
        /* 24 MHz */
        pll = AS3310_XCLOCK_DEFAULT ;
        printf("PLL: USE <Direct Crystal> at %d MHz\n",AS3310_XCLOCK_DEFAULT);
    }
    else {
        /* 480 MHz */
        pll = clk->PLLCTRL0[0] & 0x1ff ;
        printf("PLL: USE <PLL-480> at %d MHz\n",pll);
    }

    cpu = pll / (clk->CPUCLKCTRL[0]& 0x3ff);
    printf("CPU: at %d MHz\t\t",cpu); 
    hbus = cpu / (clk->HBUSCLKCTRL[0]& 0x3ff);
    printf("HBUS: at %d MHz\t",hbus);
    printf("XBUS: at %d MHz\n",AS3310_XCLOCK_DEFAULT / (clk->XBUSCLKCTRL[0]& 0x3ff) );

    printf("EMI: at %d MHz\t\t",hbus / (clk->EMICLKCTRL[0] & 0x3ff) );
    printf("GPMI: at %d MHz\n",pll / (clk->GPMICLKCTRL[0]& 0x3ff) );

    printf("SSP: at %d MHz\t\t",pll / (clk->SSPCLKCTRL[0]& 0x3ff) );
    printf("SPDIF: at %d MHz\n\n", (clk->SPDIFCLKCTRL[0]& (1<<31)) ? 0 :  pll / (clk->SPDIFCLKCTRL[0]& 0x3ff) );

    printf("Clock Gate Info:\n");
    printf("UART Clock: <%s>\t\t", (clk->XTALCLKCTRL[0]& (1<<31)) ? "Off":"On");
    printf("Digi-Filter Clock: <%s>\n", (clk->XTALCLKCTRL[0]& (1<<30)) ? "Off":"On");
    printf("PWM Clock: <%s>\t\t\t", (clk->XTALCLKCTRL[0]& (1<<29)) ? "Off":"On");
    printf("DRI Clock: <%s>\n", (clk->XTALCLKCTRL[0]& (1<<28)) ? "Off":"On");
    printf("DIG Clock: <%s>\t\t", (clk->XTALCLKCTRL[0]& (1<<27)) ? "Off":"On");
    printf("TIMROT Clock: <%s>\n", (clk->XTALCLKCTRL[0]& (1<<26)) ? "Off":"On");
    printf("EXRAM Clock: <%s>\t\t", (clk->XTALCLKCTRL[0]& (1<<25)) ? "Off":"On");
    printf("LRADC Clock: <%s>\n", (clk->XTALCLKCTRL[0]& (1<<24)) ? "Off":"On");

    printf("UTMI 120MHz Clock: <%s>\t\t", (clk->UTMICLKCTRL[0]& (1<<31)) ? "Off":"On");
    printf("UTMI 30MHz Clock: <%s>\n\n", (clk->UTMICLKCTRL[0]& (1<<30)) ? "Off":"On");

  return ;          
}


void clk_init(){
    //clock_info_ptr = &clock_info;
    /* register to kernel_info here */
}


int do_clk_sw(cmd_tbl_t *cmdtp,int flag,int argc,char* argv[]){

    clock_info_t * clk_info_ptr = get_clock_info();
    int cpu;
    
    if ((argc < 6)||(argc > 6)) {puts("Invalid args.");puts(cmdtp->usage);putc('\n');return 0;}  

    cpu = simple_strtoul(argv[1],NULL,10);

    switch (cpu) {
    case 181:
        clk_info_ptr->clk_cfg.cpu_pll = 0x1;//internal 180M / 2
        clk_info_ptr->clk_cfg.pll = 180;
        break;
    case 180:
        clk_info_ptr->clk_cfg.cpu_pll = 0x2;//360M / 2
        clk_info_ptr->clk_cfg.pll = 360;
        break;
    case 160:
        clk_info_ptr->clk_cfg.cpu_pll = 0x3;//480M / 3
        clk_info_ptr->clk_cfg.pll = 480;
        break;
    case 121:
        clk_info_ptr->clk_cfg.cpu_pll = 0x2;//internal 240M / 2
        clk_info_ptr->clk_cfg.pll = 241;
        break;
    case 120:
        clk_info_ptr->clk_cfg.cpu_pll = 0x2;//240M / 2
        clk_info_ptr->clk_cfg.pll = 240;
        break;
    case 90:
        clk_info_ptr->clk_cfg.cpu_pll = 0x4;// 360M / 4
        clk_info_ptr->clk_cfg.pll = 360;
        break;
    case 80:
        clk_info_ptr->clk_cfg.cpu_pll = 0x3;// 240M / 3
        clk_info_ptr->clk_cfg.pll = 240;
        break;
    case 60:
        clk_info_ptr->clk_cfg.cpu_pll = 0x4;// 240M / 4
        clk_info_ptr->clk_cfg.pll = 240;
        break;
    case 51:
        clk_info_ptr->clk_cfg.cpu_pll = 0x7;// 360M / 7 = 51.4
        clk_info_ptr->clk_cfg.pll = 360;
        break;
    case 48:
        clk_info_ptr->clk_cfg.cpu_pll = 0x5;// 240M / 5
        clk_info_ptr->clk_cfg.pll = 240;
        break;
    case 45:
        clk_info_ptr->clk_cfg.cpu_pll = 0x8;// 360M / 8
        clk_info_ptr->clk_cfg.pll = 360;
        break;
    case 40:
        clk_info_ptr->clk_cfg.cpu_pll = 6;// 240M / 6
        clk_info_ptr->clk_cfg.pll = 240;
        break;
    default: 
        clk_info_ptr->clk_cfg.cpu_pll = 0x1;//24M / 1
        clk_info_ptr->clk_cfg.pll = 24;
        break;
    }

    clk_info_ptr->clk_cfg.hbus_cpu = simple_strtoul(argv[2],NULL,10);
    clk_info_ptr->clk_cfg.emi_hbus = simple_strtoul(argv[3],NULL,10);
    clk_info_ptr->clk_cfg.gpmi_pll = clk_info_ptr->clk_cfg.cpu_pll * simple_strtoul(argv[4],NULL,10);
    clk_info_ptr->clk_cfg.ssp_pll = clk_info_ptr->clk_cfg.cpu_pll * simple_strtoul(argv[5],NULL,10);

    clk_info_ptr->clock_sw(&clk_info_ptr->clk_cfg);
    //clk_info_ptr->clock_dump(&clk_info_ptr->clk_cfg);
}


U_BOOT_CMD(clksw,   6,   1, do_clk_sw,
	"clksw   - switch system clock\n",
    "clksw cpu_clk hclk_cpu emi_hclk gpmi_cpu ssp_cpu\n"
     "clksw 40(f)|45|48(f)|51|60(f)|80(f)|90|120(f)|121(if)|160(h)|180|181(i)\n"
     "i: internal PLL\t f: usb full speed\t h: usb high speed\n"
	);

int do_clkdump(cmd_tbl_t *cmdtp,int flag,int argc,char * argv[]){
    clock_info_t * clk_info_ptr = get_clock_info();
    clk_info_ptr->clock_dump(&clk_info_ptr->clk_cfg);
}


U_BOOT_CMD(clkdump,   2,   1, do_clkdump,
    "clkdump   - Dump Curent Clock State\n",
    "clkdump\n"
	);




