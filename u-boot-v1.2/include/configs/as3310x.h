
#ifndef __AS3310x_H__
#define __AS3310x_H__


typedef volatile unsigned char AS3310_REG8;
typedef volatile unsigned short AS3310_REG16;
typedef volatile unsigned long AS3310_REG32;

typedef struct {
		AS3310_REG32		REV	   	;		/*	0x00	R		*/
		AS3310_REG32		ACCESS      ;  /*	0x04	R/W */
		AS3310_REG32		ADDR_SRC    ;  /*	0x08	R/W */
		AS3310_REG32		ADDR1_SRC ;  /*	0x0C	R/W */
		AS3310_REG32		CTRL        ;  /*	0x10	R/W */
		AS3310_REG32		MASK        ;  /*	0x14	R/W */
		AS3310_REG32		STATUS      ;  /*	0x18	R/W */
		AS3310_REG32		READY       ;    /*	0x1C	R	   */ 
		AS3310_REG32		COMMAND     ;  /*	0x20	R/W */
		AS3310_REG32		COMMAND_SEC ;  /*	0x24	R/W */
		AS3310_REG32		ECC_SELECT  ;  /*	0x28	R/W */

		AS3310_REG32		ECC[9]      ; 		
                         		                            
		AS3310_REG32		RESET       ;  /*	0x50	R/W */
		AS3310_REG32		FIFO        ;  /*	0x54	R/W */
		AS3310_REG32		FIFO_CTRL   ;  /*	0x58	R/W */
		AS3310_REG32		PSC_CLK     ;  /*	0x5C	R/W */
		AS3310_REG32		SYSTEST     ;  /*	0x60	R/W */
		AS3310_REG32		SYSCFG      ;  /*	0x64	R/W */
		AS3310_REG32		SYSSTATUS   ;    /*	0x68	R	   */ 
		
		AS3310_REG32		FIFOTEST[8]	;
		
		AS3310_REG32		PSC_CLK1    ;  /*	0x8C	R/W */
		AS3310_REG32		PSC_CLK2    ;  /*	0x90	R/W */
} /*__attribute__((__packed__))*/ AS3310_NAND;



typedef struct {
		AS3310_REG32	SPCR		;		/*	0x80000180			R/W	0x10	Control register*/
		AS3310_REG32	SPSR		;		/*	0x80000184	 	R	0x00	Status register */
		AS3310_REG32	SPER		;		/*	0x80000188			R/W	0x00	Extensions Register*/
		AS3310_REG32	RE_REG	;		/*	0x8000018c 	R	0x00	Receive register*/
		AS3310_REG32	TR_REG	;		/*	0x80000190		W	0x00	Transmitter register*/
} /*__attribute__((__packed__))*/ AS3310_SPI;



typedef struct {
		AS3310_REG32	PRER	;	/*0x80000140	 WR	32’h0	Clock Prescale register*/
		AS3310_REG32	EMPTY	;	/*0x80000144	*/
		AS3310_REG32	CTR		;	/*0x80000148		WR	32’h0	Control register */
		AS3310_REG32	TXR		;	/*0x8000014c		W	32’h0	Transmit register */
		AS3310_REG32	CR		;	/*0x80000150	   	W	32’h0	Command register*/
		AS3310_REG32	RXR		;	/*0x80000154		R	32’h0	Receive register*/
		AS3310_REG32	SR		;	/*0x80000158	   	R	32’h0	Status register*/
} /*__attribute__((__packed__))*/ AS3310_I2C;


static inline AS3310_NAND * const AS3310_GetBase_NAND(void)
{
	return (AS3310_NAND * const)AS3310_NAND_BASE;
}
static inline AS3310_SPI * const AS3310_GetBase_SPI(void)
{
	return (AS3310_SPI * const)AS3310_SPI_BASE;
}
static inline AS3310_I2C * const AS3310_GetBase_I2C(void)
{
	return (AS3310_I2C * const)AS3310_I2C_BASE;
}

#endif /*__AS3310x_H__*/
