

/*
Alpha Scale AS3310X Booter, H-BOOT
Zhao Haiyuan, AlpScale Software Engineering, zhaoy@alpscale.com.cn
 
------------------- Version 1.0  ----------------------
Create File, 
    Support Nand ECC 
    Zhao Haiyuan 2007-04-29

*/


#include <common.h>
#include "flash.h"

#define ECC_MAX_PACKAGE     7
#define HW_HWECC_CTRL       0x80008000

#define ECC_UNCORRECTABLE   -1
#define ECC_NOERROR         0
#define ECC_CORRECTED       1
#define MAX_DMA_WAIT        0x100000

AS3310_DMA_PKG  ecc_pkg_table[ECC_MAX_PACKAGE];
int error_code;

#define HWECC_TIMEOUT 0x100000

void Ecc_Init(){ //reset HWECC and APBH 
outl(0xc0000000,HW_HWECC_CTRL + 8);  //hwecc sftrst = 0         
outl(0xc0000000,HW_APBH_CTRL0 + 8);  //dma sftrst = 0   
outl(0x00000000,HW_APBH_CTRL1);  //dma ctrl1 irq=1;irq_en=1;
}

/*
Ecc Encode. Encode the data from the address "datafrom" with the byte length of "Ecclth" eg 512.
Write the status code to "parityadd" and write the parity code to "parityadd+1" 
*/
void EccEncode(ulong datafrom, ulong parityadd)
{
int timeout;
ecc_pkg_table[1].NEXT_PKG = (ulong)(&ecc_pkg_table[2].NEXT_PKG);
ecc_pkg_table[1].CTRL = 0x02001046; 
ecc_pkg_table[1].BUFFER = datafrom;
ecc_pkg_table[1].CMD0 = 0x01c80042; //encode

ecc_pkg_table[2].NEXT_PKG = (ulong)(&ecc_pkg_table[3].NEXT_PKG);
ecc_pkg_table[2].CTRL = 0x00040045;
ecc_pkg_table[2].BUFFER = (ulong)&error_code;

ecc_pkg_table[3].NEXT_PKG = 0;
ecc_pkg_table[3].CTRL = 0x00090041; 
ecc_pkg_table[3].BUFFER = parityadd; //RsEncode+0x04; 512byte after 4 bytes of status adn 9 byte of parity

/*begin the DMA and set sema*/

outl((ulong)&(ecc_pkg_table[1].NEXT_PKG),HW_APBH_CH0_NXTCMDAR);  //next cmd addr 3 cmd
outl(0x3,HW_APBH_CH0_SEMA); //3 

timeout = HWECC_TIMEOUT;
while ((inl(HW_APBH_CH0_SEMA))&0x00ff0000) {
    if (timeout-- < 0 ) { printf("ECC DMA Time OUT\n"); break;}
 }

}


/*
Ecc Decode
decode the data from the address "datafrom" 
with the byte length of "Ecclth" eg 512.
based on the party code from parityadd
write the EccReport to "Report" 9*32 bit
*/
int EccDecodeCheck( int* datafrom,int* parityadd){

int Report[10];
int index;
int mask;
int j;
int wait_dma;


/*3 dma chain to Ecc decode. Produce Ecc Error Report*/
ecc_pkg_table[4].NEXT_PKG = (ulong)(&ecc_pkg_table[5].NEXT_PKG);
ecc_pkg_table[4].CTRL = 0x02001046; 
ecc_pkg_table[4].BUFFER = (ulong)datafrom;
ecc_pkg_table[4].CMD0 = 0x01c80053; //dncode

ecc_pkg_table[5].NEXT_PKG = (ulong)(&ecc_pkg_table[6].NEXT_PKG);
ecc_pkg_table[5].CTRL = 0x00090046;
ecc_pkg_table[5].BUFFER = (ulong)(parityadd);//parity code;

ecc_pkg_table[6].NEXT_PKG = 0;
ecc_pkg_table[6].CTRL = 0x00240041; 
ecc_pkg_table[6].BUFFER = (ulong)Report;

/*Start dma channel and sema*/
 outl(0x00000003,HW_APBH_CH0_SEMA);//sema
 outl((ulong)&(ecc_pkg_table[4].NEXT_PKG),HW_APBH_CH0_NXTCMDAR);  //next cmd addr     

 wait_dma = 0;
 while ((inl(HW_APBH_CH0_SEMA))&0x00ff0000) {
     if (wait_dma++ > MAX_DMA_WAIT) {
         puts("DMA Not Ready\n"); 
         break;
     }
 }
 
/*
Ecc correct data. Based on the EccReport from "Report" Check the data from "datafrom". Check the parity code from "parityadd". 
*/
    if (Report[0]==0)	{
    		return(ECC_NOERROR); //no error
    	}    
    else if (Report[0]==0x100403f7){
    		return(ECC_UNCORRECTABLE); //uncorrectable error. require irq to resend
    	}    
    else { // data error or parity error, correctable
        for(j=1;j<9;j++){//9 dword of EccReport    
            if(Report[j]==0) break;  // if Report==0, no error any more
            else	{  		  
                index = (Report[j]>>16);  //index locate the error
                mask = Report[j]&0x0000ffff; //mask correct the error
    			   
                if((index&0x8000)==0x8000){ //parity error
                    index=index&0x0fff; //erase the fist bit of index and correct the parity
                    if ( (index & 0x1 )==0){
                        parityadd[index/2-1] = parityadd[index/2-1]^(mask<<16);
                    }
    				else{
                        parityadd[(index+1)/2-1] = parityadd[(index+1)/2-1]^(mask);
                    }	 	
                }			 
                else{//data error and correct them
                    if ((index & 0x1 )==0) { 
                        datafrom[index/2-1] = datafrom[index/2-1]^(mask<<16);
                        }
    				else {
                        datafrom[(index+1)/2-1] = datafrom[(index+1)/2-1]^(mask);
                    }
                }
            }	
        }
        return(ECC_CORRECTED); /*the error has bee corrected*/	
    }
}	 


    
