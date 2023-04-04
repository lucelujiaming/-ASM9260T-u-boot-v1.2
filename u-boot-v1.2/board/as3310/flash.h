
/*
Alpha Scale AS3310X BootLoader H-BOOT
He Yong, AlpScale Software Engineering, hoffer@sjtu.org
 
AS3310 Boot Loader Nand table Header file

------------------- Version 1.0  ----------------------
Create File, Support 26 types of Nand Flash
 He Yong 2006-11-28

*/
#include <asm/types.h>

#ifndef __NAND_TABLE_STRUCT__
#define __NAND_TABLE_STRUCT__

#define	NF_MAX_DEVID	13
#define	NF_MAX_M	7
#define	NF_MAX_MLC	4
#define	NF_PROTECT_DEVICE	0x00

#define NAND_INFO_MAGIC     0x53414C46// FLAS

/*  Length 0x04*/
typedef struct Nand_Param_struct{
	u8	NfType;   
		// 0 : page_shift = 9 , 2,3 : page_shift = 11          
        // Other --> High 2 bit :col_cycles, Low 6 bit: page_shift
	u8	ShiftBlockPage;
	u8	DeviceZone;
	u8	ExtAddr;
	        //row cycle = 2 + ExtAddr
        }__attribute__((packed)) Nand_Param;


/*  Length 0x20*/
typedef struct nand_chip_table_struct{
    u32     Magic;   //  0x53414C46 FLAS
	u16	max_devid;	//  NF_MAX_DEVID   //  13
	u8	max_maker;	//  NF_MAX_M   //  7
	u8	max_mlc;	//  NF_MAX_MLC   //  4                 
    u32 Rsvd[2];
	const Nand_Param ParaTable[24];       //  0x60 bytes
	const Nand_Param MLCParaTable[24];    //  0x60 bytes
	const u8 	DevidTable[24];             //  0x18 bytes
	const u8 	LvDevidTable[24];           //  0x18 bytes
	const u8 	MakerTable[16];             //  0x10 bytes
    const u8 	MLCTable[16];               //  0x10 bytes
        }__attribute__((packed)) nand_chip_table;  // total    0x120


/*  Length 0x20*/
typedef struct nand_info_struct{
	u8	maker_code;
	u8	dev_id;
	u8	extra_1;
	u8	extra_2;
	u32	extra_3;
	u32	page_shift;
	u32	block_shift;
	u32	row_cycles;
	u32	col_cycles;
    u8  * maker_name;
	nand_chip_table * chip_table;
	
}__attribute__((packed)) nand_info;


/*  ===========  NAND FLASH General 4th ID  ============ */
        /*  =  Samsung / ST / Micron / HYNIX  == */

/*
Col Cycle = 2
Row Cycle = 3  default 2Gb+
PageShift = nand_general_id4.PageSize + 10
BlockPageShift = 6 + nand_general_id4.BlockSize - nand_general_id4.PageSize
*/

#define NAND_SAMSUNG_MANID      0xec
#define NAND_ST_MANID           0x20
#define NAND_HYNIX_MANID        0xad
#define NAND_MICRON_MANID       0x2c
#define NAND_TOSHIBA_MANID      0x98
#define NAND_SANDISK_MANID      0x45
#define NAND_RENESAS_MANID      0x07

#define NAND_GENERAL_PAGE_SIZE_1KB  0x0
#define NAND_GENERAL_PAGE_SIZE_2KB  0x1
#define NAND_GENERAL_PAGE_SIZE_4KB  0x2
#define NAND_GENERAL_PAGE_SIZE_8KB  0x3

#define NAND_GENERAL_PAGE_SIZE_64KB     0x0
#define NAND_GENERAL_PAGE_SIZE_128KB    0x1
#define NAND_GENERAL_PAGE_SIZE_256KB    0x2
#define NAND_GENERAL_PAGE_SIZE_512KB    0x3

#define NAND_GENERAL_ORG_X8     0x0
#define NAND_GENERAL_ORG_X16    0x1

typedef union _nand_general_id4{
    struct _nand_general_id4_{
        u8 PageSize             : 2;    // PageSize
        u8 SpareAreaSize        : 1;    // 
        u8 AccessTime1          : 1;    // 
        u8 BlockSize            : 2;    // BlockSize
        u8 Orgniz               : 1;    // x8 or x16
        u8 AccessTime2          : 1;    //
    } __attribute__((packed)) field ;
    u8 value;
}nand_general_id4;


typedef struct {
		u32		NEXT_PKG	;   //	0x00	R/W		
		u32		CTRL        ;  //	0x04	R/W 
		u32		BUFFER      ;  //	0x08	R/W 
		u32		CMD0        ;  //	0x0c	R/W 
		u32		CMD1        ;  //	0x10	R/W 
} /*__attribute__((__packed__))*/ AS3310_DMA_PKG;


// static inline nand_info * const get_nand_info(){
// return (nand_info * const)NAND_INFO_BASE;
// }
//
// static inline nand_chip_table * const get_nand_table(){
// return (nand_chip_table * const)NAND_TABLE_BASE;
// }
//
// static inline nand_chip_table * const get_rom_nand_table(){
// return (nand_chip_table * const)(&__NAND_TABLE_START);
// }

#define Struct_Section_NAND_TABLE  __attribute__ ((section (".nand_table")))

#endif // __BCB_STRUCT__

