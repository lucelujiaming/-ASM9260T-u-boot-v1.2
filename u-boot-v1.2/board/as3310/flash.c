/*
 * (C) Copyright 2001
 * Kyle Harris, Nexus Technologies, Inc. kharris@nexus-tech.net
 *
 * (C) Copyright 2001
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
 *
 * (C) Copyright 2003
 * Texas Instruments, <www.ti.com>
 * Kshitij Gupta <Kshitij@ti.com>

 * (C) Copyright 2004
 * Texas Instruments <www.ti.com>
 * Rishi Bhattacharya
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

#include "flash.h"
#include <common.h>
#include <linux/byteorder/swab.h>

#define PHYS_FLASH_SECT_SIZE	0x00020000	/* 256 KB sectors (x2) */
flash_info_t flash_info[CFG_MAX_FLASH_BANKS];	/* info for FLASH chips    */

/* Nand configure */
uchar nand_cycles;
uchar nand_row_cycles;
uchar nand_col_cycles;
uchar nand_page_shift;
uchar nand_block_shift;
ulong nand_current_cen;
nand_info  nand_info_s;

/* Board support for 1 or 2 flash devices */
#undef FLASH_PORT_WIDTH32
#define FLASH_PORT_WIDTH16

#ifdef FLASH_PORT_WIDTH16
#define FLASH_PORT_WIDTH		ushort
#define FLASH_PORT_WIDTHV		vu_short
#define SWAP(x)			__swab16(x)
#else
#define FLASH_PORT_WIDTH		ulong
#define FLASH_PORT_WIDTHV		vu_long
#define SWAP(x)			__swab32(x)
#endif

#define FPW	FLASH_PORT_WIDTH
#define FPWV	FLASH_PORT_WIDTHV

#define mb() __asm__ __volatile__ ("" : : : "memory")



/* Flash Organization Structure */
typedef struct OrgDef {
	unsigned int sector_number;
	unsigned int sector_size;
} OrgDef;


/* Flash Organizations */
OrgDef OrgIntel_28F256L18T[] = {
	{4, 32 * 1024},				/* 4 * 32kBytes sectors */
	{255, 128 * 1024},			/* 255 * 128kBytes sectors */
};

/* Flash Organizations */
OrgDef OrgMem_29LV160TE[] = {
	{1, 16 * 1024},				/* 1 * 16kBytes sectors */
    {3, 8 * 1024},				/* 3 * 8kBytes sectors */
    {31, 32 * 1024},				/* 31 * 32kBytes sectors */

};

/*-----------------------------------------------------------------------
 * Functions
 */
unsigned long flash_init (void);
static ulong flash_get_size (FPW * addr, flash_info_t * info);
static int write_data (flash_info_t * info, ulong dest, FPW data);
static void flash_get_offsets (ulong base, flash_info_t * info);
void inline spin_wheel (void);
void flash_print_info (flash_info_t * info);
void flash_unprotect_sectors (FPWV * addr);
int flash_erase (flash_info_t * info, int s_first, int s_last);
int write_buff (flash_info_t * info, uchar * src, ulong addr, ulong cnt);

/*-----------------------------------------------------------------------
 */

unsigned long flash_init (void)
{
	int i;
	ulong size = 0;
	for (i = 0; i < CFG_MAX_FLASH_BANKS; i++) {
		switch (i) {
		case 0:
			flash_get_size ((FPW *) PHYS_FLASH_1, &flash_info[i]);
			flash_get_offsets (PHYS_FLASH_1, &flash_info[i]);
			break;
		default:
			panic ("configured too many flash banks!\n");
			break;
		}
		size += flash_info[i].size;
	}

	/* Protect monitor and environment sectors
	 */
	flash_protect (FLAG_PROTECT_SET,
			CFG_FLASH_BASE,
			CFG_FLASH_BASE + monitor_flash_len - 1, &flash_info[0]);

	flash_protect (FLAG_PROTECT_SET,
			CFG_ENV_ADDR,
			CFG_ENV_ADDR + CFG_ENV_SIZE - 1, &flash_info[0]);

	return size;
}

/*-----------------------------------------------------------------------
 */
static void flash_get_offsets (ulong base, flash_info_t * info)
{
	int i;
	OrgDef *pOrgDef;

	pOrgDef = OrgMem_29LV160TE/*OrgIntel_28F256L18T*/;  
	if (info->flash_id == FLASH_UNKNOWN) {
		return;
	}

	if ((info->flash_id & FLASH_VENDMASK) == FLASH_MAN_INTEL) {
		for (i = 0; i < info->sector_count; i++) {
			if (i > 255) {
				info->start[i] = base + (i * 0x8000);
				info->protect[i] = 0;
			} else {
				info->start[i] = base +
						(i * PHYS_FLASH_SECT_SIZE);
				info->protect[i] = 0;
			}
		}
	}
}

/*-----------------------------------------------------------------------
 */
void flash_print_info (flash_info_t * info)
{
	int i;

	if (info->flash_id == FLASH_UNKNOWN) {
		printf ("missing or unknown FLASH type\n");
		return;
	}

	switch (info->flash_id & FLASH_VENDMASK) {
	case FLASH_MAN_INTEL:
		printf ("INTEL ");
		break;
	default:
		printf ("Unknown Vendor ");
		break;
	}

	switch (info->flash_id & FLASH_TYPEMASK) {
	case FLASH_28F256L18T:
		printf ("FLASH 28F256L18T\n");
		break;
	case FLASH_28F128J3A:
		printf ("FLASH 28F128J3A\n");
		break;
	default:
		printf ("Unknown Chip Type\n");
		break;
	}

	printf ("  Size: %ld MB in %d Sectors\n",
			info->size >> 20, info->sector_count);

	printf ("  Sector Start Addresses:");
	for (i = 0; i < info->sector_count; ++i) {
		if ((i % 5) == 0)
			printf ("\n   ");
		printf (" %08lX%s",
			info->start[i], info->protect[i] ? " (RO)" : "     ");
	}
	printf ("\n");
	return;
}

/*
 * The following code cannot be run from FLASH!
 */
static ulong flash_get_size (FPW * addr, flash_info_t * info)
{
	volatile FPW value;

	/* Write auto select command: read Manufacturer ID */
	addr[0x5555] = (FPW) 0x00AA00AA;
	addr[0x2AAA] = (FPW) 0x00550055;
	addr[0x5555] = (FPW) 0x00900090;

	mb ();
	value = addr[0];

	switch (value) {

	case (FPW) INTEL_MANUFACT:
		info->flash_id = FLASH_MAN_INTEL;
		break;
              case (FPW) MT2_MANUFACT:
		info->flash_id = FLASH_MAN_INTEL;
		break;

	default:
		info->flash_id = FLASH_UNKNOWN;
		info->sector_count = 0;
		info->size = 0;
		addr[0] = (FPW) 0x00FF00FF;	/* restore read mode */
		return (0);		/* no or unknown flash  */
	}

	mb ();
	value = addr[1];	/* device ID        */
	switch (value) {

	case (FPW) (INTEL_ID_28F256L18T):
		info->flash_id += FLASH_28F256L18T;
		info->sector_count = 259;
		info->size = 0x02000000;
		break;			/* => 32 MB     */

              case (FPW) (INTEL_ID_28F256K3):
		info->flash_id +=FLASH_28F256K3;                  
		info->sector_count = 259;
		info->size = 0x02000000;
                            printf ("\Intel StrataFlash 28F256K3C device initialized\n");
		break;			/* => 32 MB     */

 	case (FPW) (INTEL_ID_28F128J3A):
		info->flash_id +=FLASH_28F128J3A;                  
		info->sector_count = 259;
		info->size = 0x02000000;
                            printf ("\Micron StrataFlash MT28F128J3 device initialized\n");
		break;			/* => 32 MB     */

	default:
		info->flash_id = FLASH_UNKNOWN;
		break;
	}

	if (info->sector_count > CFG_MAX_FLASH_SECT) {
		printf ("** ERROR: sector count %d > max (%d) **\n",
				info->sector_count, CFG_MAX_FLASH_SECT);
		info->sector_count = CFG_MAX_FLASH_SECT;
	}

	addr[0] = (FPW) 0x00FF00FF;	/* restore read mode */

	return (info->size);
}


/* unprotects a sector for write and erase
 * on some intel parts, this unprotects the entire chip, but it
 * wont hurt to call this additional times per sector...
 */
void flash_unprotect_sectors (FPWV * addr)
{
#define PD_FINTEL_WSMS_READY_MASK    0x0080

FPW status;

	*addr = (FPW) 0x00500050;	/* clear status register */

	/* this sends the clear lock bit command */
	*addr = (FPW) 0x00600060;
	*addr = (FPW) 0x00D000D0;
              
              while (((status =*addr) & (FPW) 0x00800080) != (FPW) 0x00800080);

              *addr = (FPW) 0x00FF00FF;

}


/*-----------------------------------------------------------------------
 */

int flash_erase (flash_info_t * info, int s_first, int s_last)
{
	int flag, prot, sect;
	ulong type, start, last;
	int rcode = 0;

	if ((s_first < 0) || (s_first > s_last)) {
		if (info->flash_id == FLASH_UNKNOWN) {
			printf ("- missing\n");
		} else {
			printf ("- no sectors to erase\n");
		}
		return 1;
	}

	type = (info->flash_id & FLASH_VENDMASK);
	if ((type != FLASH_MAN_INTEL)) {
		printf ("Can't erase unknown flash type %08lx - aborted\n",
				info->flash_id);
		return 1;
	}

	prot = 0;
	for (sect = s_first; sect <= s_last; ++sect) {
		if (info->protect[sect]) {
			prot++;
		}
	}

	if (prot) {
		printf ("- Warning: %d protected sectors will not be erased!\n",
				prot);
	} else {
		printf ("\n");
	}


	start = get_timer (0);
	last = start;

	/* Disable interrupts which might cause a timeout here */
	flag = disable_interrupts ();

	/* Start erase on unprotected sectors */
	for (sect = s_first; sect <= s_last; sect++) {
		if (info->protect[sect] == 0) {	/* not protected */
			FPWV *addr = (FPWV *) (info->start[sect]);
			FPW status;

			printf ("Erasing sector %2d ... ", sect);

			flash_unprotect_sectors (addr);

			/* arm simple, non interrupt dependent timer */
			reset_timer_masked ();

			*addr = (FPW) 0x00500050;/* clear status register */
			*addr = (FPW) 0x00200020;/* erase setup */
			*addr = (FPW) 0x00D000D0;/* erase confirm */

			while (((status =
				*addr) & (FPW) 0x00800080) !=
				(FPW) 0x00800080) {
					if (get_timer_masked () >
					CFG_FLASH_ERASE_TOUT) {
					printf ("Timeout\n");
					/* suspend erase     */
					*addr = (FPW) 0x00B000B0;
					/* reset to read mode */
					*addr = (FPW) 0x00FF00FF;
					rcode = 1;
					break;
				}
			}

			/* clear status register cmd.   */
			*addr = (FPW) 0x00500050;
			*addr = (FPW) 0x00FF00FF;/* resest to read mode */
			printf (" done\n");
		}
	}
	return rcode;
}

/*-----------------------------------------------------------------------
 * Copy memory to flash, returns:
 * 0 - OK
 * 1 - write timeout
 * 2 - Flash not erased
 * 4 - Flash not identified
 */

int write_buff (flash_info_t * info, uchar * src, ulong addr, ulong cnt)
{
	ulong cp, wp;
	FPW data;
	int count, i, l, rc, port_width;

	if (info->flash_id == FLASH_UNKNOWN) {
		return 4;
	}
/* get lower word aligned address */
#ifdef FLASH_PORT_WIDTH16
	wp = (addr & ~1);
	port_width = 2;
#else
	wp = (addr & ~3);
	port_width = 4;
#endif

	/*
	 * handle unaligned start bytes
	 */
	if ((l = addr - wp) != 0) {
		data = 0;
		for (i = 0, cp = wp; i < l; ++i, ++cp) {
			data = (data << 8) | (*(uchar *) cp);
		}
		for (; i < port_width && cnt > 0; ++i) {
			data = (data << 8) | *src++;
			--cnt;
			++cp;
		}
		for (; cnt == 0 && i < port_width; ++i, ++cp) {
			data = (data << 8) | (*(uchar *) cp);
		}

		if ((rc = write_data (info, wp, SWAP (data))) != 0) {
			return (rc);
		}
		wp += port_width;
	}

	/*
	 * handle word aligned part
	 */
	count = 0;
	while (cnt >= port_width) {
		data = 0;
		for (i = 0; i < port_width; ++i) {
			data = (data << 8) | *src++;
		}
		if ((rc = write_data (info, wp, SWAP (data))) != 0) {
			return (rc);
		}
		wp += port_width;
		cnt -= port_width;
		if (count++ > 0x800) {
			spin_wheel ();
			count = 0;
		}
	}

	if (cnt == 0) {
		return (0);
	}

	/*
	 * handle unaligned tail bytes
	 */
	data = 0;
	for (i = 0, cp = wp; i < port_width && cnt > 0; ++i, ++cp) {
		data = (data << 8) | *src++;
		--cnt;
	}
	for (; i < port_width; ++i, ++cp) {
		data = (data << 8) | (*(uchar *) cp);
	}

	return (write_data (info, wp, SWAP (data)));
}

/*-----------------------------------------------------------------------
 * Write a word or halfword to Flash, returns:
 * 0 - OK
 * 1 - write timeout
 * 2 - Flash not erased
 */
static int write_data (flash_info_t * info, ulong dest, FPW data)
{
	FPWV *addr = (FPWV *) dest;
	ulong status;
	int flag;

	/* Check if Flash is (sufficiently) erased */
	if ((*addr & data) != data) {
		printf ("not erased at %08lx (%x)\n", (ulong) addr, *addr);
		return (2);
	}
	/* Disable interrupts which might cause a timeout here */
	flag = disable_interrupts ();
	*addr = (FPW) 0x00400040;	/* write setup */
	*addr = data;

	/* arm simple, non interrupt dependent timer */
	reset_timer_masked ();

	/* wait while polling the status register */
	while (((status = *addr) & (FPW) 0x00800080) != (FPW) 0x00800080) {
		if (get_timer_masked () > CFG_FLASH_WRITE_TOUT) {
			*addr = (FPW) 0x00FF00FF;	/* restore read mode */
			return (1);
		}
	}
	*addr = (FPW) 0x00FF00FF;	/* restore read mode */
	return (0);
}

void inline spin_wheel (void)
{
	static int p = 0;
	static char w[] = "\\/-";

	printf ("\010%c", w[p]);
	(++p == 3) ? (p = 0) : 0;
}




/* Write Nor Flash*/
/*
*@Input  unsigned long addr; the address where we wants to write
unsigned char data; the data which we wants to write
*@Return int ; 1- success,0- fail
*@Description write ONE BYTE data to flash memory with a given address.
*/

#define NOR_WRITE_RETRY 1000
int Nor_Write_AS3310_single(ulong addr,ushort data){

int is_pass;
volatile unsigned char data_polling;
int retry;
retry = NOR_WRITE_RETRY;
is_pass=0;
addr=addr&0xffffffe;

as_outw(NOR_MBM29LV160TE_COMMON_CMD_1,NOR_MBM29LV160TE_ADDR_WORDMODE_1);
as_outw(NOR_MBM29LV160TE_COMMON_CMD_2,NOR_MBM29LV160TE_ADDR_WORDMODE_2);
as_outw(NOR_MBM29LV160TE_WRITE_CMD_3,NOR_MBM29LV160TE_ADDR_WORDMODE_3);
as_outw(data,addr);

while(/*ctrlc()!=*/1){
    data_polling = as_inw(addr);
    retry--;
    if (retry <= 0) {
//        putc('^');
        break;
    }
    if ((data_polling&0x80) == (data&0x80)) {is_pass=1;break;}
    if ((data_polling&0x20)==0) continue;
    else{	
        data_polling = as_inw(addr);
        if ((data_polling&0x80) == (data&0x80)) {is_pass=1;break;}
        else {is_pass=0;break;}
	}
}
return is_pass;
}





/*
Copy data from SDRAM to Nor Flash
return total errors
*/
#define KEY_BACKSPACE 0x7f //27 105
#define KEY_LEFT_1 0x1b
#define KEY_LEFT_2 0x5b
#define KEY_LEFT_3 0x44
#define NOR_M_WRITE_RETRY 3

int Nor_Write_AS3310(ulong addr,ulong* buffer,ulong length){
int error_flag;
int i;
ulong current_addr,end_addr,errors,try_num,block;
volatile ushort *wptr;
volatile ushort temp;

puts("Write to Flash...           ");
wptr = (ushort *)buffer;
end_addr = addr + length;
current_addr = addr;
errors = 0;
block = 0;
    while(current_addr < end_addr){
        try_num = 0;
        do{        
            temp = *wptr;
            error_flag = Nor_Write_AS3310_single(current_addr,temp);
//          if (error_flag == 1) break;
//            errors += (1-error_flag);  
            try_num ++;
            }while((as_inw(current_addr)!=temp)&&(try_num<NOR_M_WRITE_RETRY));
        if (try_num == NOR_M_WRITE_RETRY ) errors++;
        if (((current_addr-addr)&0x3fff) == 0) {
            for (i=0; i<11 ; i++) { 
                putc(KEY_LEFT_1);putc(KEY_LEFT_2);putc(KEY_LEFT_3);
            }
            printf("%8d KB",(++block)*16);
        }
        current_addr = current_addr + 2;
        wptr++;
    }
putc('\n');
   return errors;
}


/*
NAND Erase
addr: sector address to erase
addr = 0xfffffffe -> Chip Erase
*/
void NOR_MBM29LV160TE_Erase(ulong addr)/*in as3310 case */
{
int is_pass;
unsigned char data_polling;
uchar data;

is_pass=0;
addr=addr&0xffffffe;

as_outw(NOR_MBM29LV160TE_COMMON_CMD_1,NOR_MBM29LV160TE_ADDR_WORDMODE_1);
as_outw(NOR_MBM29LV160TE_COMMON_CMD_2,NOR_MBM29LV160TE_ADDR_WORDMODE_2);
as_outw(NOR_MBM29LV160TE_ERASE_CMD_3,NOR_MBM29LV160TE_ADDR_WORDMODE_3);
as_outw(NOR_MBM29LV160TE_ERASE_CMD_4,NOR_MBM29LV160TE_ADDR_WORDMODE_4);
as_outw(NOR_MBM29LV160TE_ERASE_CMD_5,NOR_MBM29LV160TE_ADDR_WORDMODE_5);

if (addr == 0xfffffffe) {// Chip Erease
as_outw(NOR_MBM29LV160TE_ERASE_CMD_6_CHIP,NOR_MBM29LV160TE_ADDR_WORDMODE_6);
data = NOR_MBM29LV160TE_ERASE_CMD_6_CHIP;
}
else {
    as_outw(NOR_MBM29LV160TE_ERASE_CMD_6_SECTOR,addr);// Sector Erease
    data = NOR_MBM29LV160TE_ERASE_CMD_6_SECTOR;
}

while(ctrlc()!=1){
    data_polling = as_inw(addr);

    if ((data_polling&0x80) == (data&0x80)) {is_pass=1;break;}
    if ((data_polling&0x20)==0) continue;
    else{	
        data_polling = as_inw(addr);
        if ((data_polling&0x80) == (data&0x80)) {is_pass=1;break;}
        else {is_pass=0;break;}
	}
}
return is_pass;
}



#ifdef __HAVE_NAND_FLASH_

nand_chip_table __rom_nand_table_ Struct_Section_NAND_TABLE = {
.Magic = NAND_INFO_MAGIC,
.max_devid = NF_MAX_DEVID,
.max_maker = NF_MAX_M,
.max_mlc   = NF_MAX_MLC,                
.Rsvd = {0,0}, 
.ParaTable = {
    {0,5,1,0},    // K9F2808        
	{0,5,2,0},    // K9F5608        
 	{0,5,4,1},    // K9F1208        
	{0,5,8,1},    // K9K1G08        
	{0,5,16,1},   // SDTNGBHE0-2048 
	{2,6,1,0},    // K9F1G08        
    {0xff,6,2,1},    // K9F2G08        
	{0xff,6,4,1},    // K9F4G08        
    {0xff,6,8,1},    // K9F8G08        
    {0xff,6,16,1},   // 16G            
    {3,2,4,0},    // HN29V25691     
    {3,3,8,0},    // HN29V1G91      
    {0xff,4,16,1},   // HN29V4G91      
    {0xff,0xff,0xff,0xff},
    {0xff,0xff,0xff,0xff},
    {0xff,0xff,0xff,0xff},
    {0xff,0xff,0xff,0xff},
    {0xff,0xff,0xff,0xff},
    {0xff,0xff,0xff,0xff},
    {0xff,0xff,0xff,0xff},
    {0xff,0xff,0xff,0xff},
    {0xff,0xff,0xff,0xff},
    {0xff,0xff,0xff,0xff},
    {0xff,0xff,0xff,0xff},
},
.MLCParaTable  = {
	{0xff,5,1,0},    // K9F2808         
	{0,6,1,0},       // K9F5608         
	{0,6,2,1},       // K9F1208         
	{0,6,4,1},       // K9K1G08         
	{0,6,8,1},       // SDTNGBHE0-2048  
	{0xff,6,1,0},    // K9F1G08         
	{0xff,6,8,1},    // K9F2G08         
	{0xff,6,16,1},   // K9F4G08         
	{0xff,6,8,1},    // K9F8G08         
	{0xff,6,16,1},   // 16G             
	{0xff,4,1,0},    // HN29V25691      
	{0xff,4,4,0},    // HN29V1G91       
	{0xff,4,16,1},   // HN29V4G91       
    {0xff,0xff,0xff,0xff},
    {0xff,0xff,0xff,0xff},
    {0xff,0xff,0xff,0xff},
    {0xff,0xff,0xff,0xff},
    {0xff,0xff,0xff,0xff},
    {0xff,0xff,0xff,0xff},
    {0xff,0xff,0xff,0xff},
    {0xff,0xff,0xff,0xff},
    {0xff,0xff,0xff,0xff},
    {0xff,0xff,0xff,0xff},
    {0xff,0xff,0xff,0xff}
},
.DevidTable   = {
	0x73,	// K9F2808
	0x75,	// K9F5608
	0x76,	// K9F1208
	0x79,	// K9K1G08
	0x71,	// SDTNGBHE0-2048
	0xf1,	// K9F1G08
	0xda,	// K9F2G08
	0xdc,	// K9F4G08
	0xd3,	// K9F8G08
	0xd5,	// 16G
	0x08,	// HN29V25691
	0X01,	// HN29V1G91
	0x29,	// HN29V4G91
},      
.LvDevidTable   = {
	0x33,	// K9F2808
	0x35,	// K9F5608
	0x36,	// K9F1208
	0x39,	// K9K1G08
	0x31,	// SDTNGBHE0-2048
	0xa1,	// K9F1G08
	0xaa,	// K9F2G08
	0xac,	// K9F4G08
	0xa3,	// K9F8G08
	0xa5,	// 16G
	0x08,	// HN29V25691
	0X01,	// HN29V1G91
	0x29,	// HN29V4G91
},
.MakerTable = {
	0xec,	// SAMSUNG
	0x98,	// TOSHIBA
	0x45,	// SANDISK
	0x20,	// ST
	0xad,	// HYNIX
	0x2c,	// MICRON
	0x07,	// RENESAS
},
.MLCTable = { // ID2 byte 0
    0x24,
    0x25,
    0x64,
    0x65,
},
};

void delay(ulong volatile time){
 while(time)time--; 
}


void select_nand_cen(u8 cen){
    nand_current_cen = cen;
    outl(0x2 << ((cen)*2) ,HW_PINCTRL_MUXSEL6 + 8); 
    outl(0x1 << ((cen)*2) ,HW_PINCTRL_MUXSEL6 + 4); 
}

int NAND_init(){
    nand_info * nand_dev_info;
    AS3310_GPMI * gpmi_ptr;
    nand_general_id4 fourth_id;
    int i;

    /* Pin Assign */
    outl(0x0,HW_PINCTRL_MUXSEL0); 
    outl(0x0,HW_PINCTRL_MUXSEL1);  
    outl(0x1,HW_PINCTRL_MUXSEL6_SET); 
    /* Select nand at CEN 0 */
    select_nand_cen(CONFIG_NAND_CEN);

    /* WP set to HIGH */
    set_pin_mux(1,20,PIN_FUNCTION_GPIO);
    set_GPIO(1,20);

    /* get pointer */
	gpmi_ptr = AS3310_GetBase_GPMI();

	gpmi_ptr->CTRL0[0] = 0;  // Gate
    gpmi_ptr->CTRL1[0] = 4;  // choose nand mode
    gpmi_ptr->TIMING1[0] = 0xffff0000;  // wait max

    outl(0x40000000,HW_APBH_CTRL0_CLR);//clear the clk gate
    outl(0x80000000,HW_APBH_CTRL0_CLR);//clear the clk gate     
    outl(0x00100000,HW_APBH_CTRL0);// nand dma channel

    nand_dev_info = &nand_info_s;
    nand_dev_info->chip_table = &__rom_nand_table_;

    if(NandSearch(nand_dev_info)<0) {
        for (i = 0; i < 7 ; i++) {
            if (nand_dev_info->maker_code == nand_dev_info->chip_table->MakerTable[i]) {
                // init as General NAND Chip
                fourth_id.value = nand_dev_info->extra_2;
                if (fourth_id.field.Orgniz != NAND_GENERAL_ORG_X8) {
                    // not supported x16 Nand
                    break;
                }
                nand_dev_info->row_cycles = 3;
                nand_dev_info->col_cycles = 2;
                nand_dev_info->page_shift = fourth_id.field.PageSize + 10;
                nand_dev_info->block_shift = 6 + fourth_id.field.BlockSize - fourth_id.field.PageSize;
                goto _found_nand;
            }
        }
        // nand not found
        nand_dev_info->row_cycles = 0; 
        nand_dev_info->col_cycles = 0; // marked unknow
        puts("unknow nand chip\n");
        return -1;
    }


_found_nand:

    nand_cycles = nand_dev_info->row_cycles + nand_dev_info->col_cycles;
    nand_page_shift = nand_dev_info->page_shift;
    nand_row_cycles = nand_dev_info->row_cycles;
    nand_col_cycles = nand_dev_info->col_cycles;
    nand_block_shift = nand_dev_info->block_shift;

    puts("Nand inited.");
//    puts("\tMaker : ");puts(nand_dev_info->maker_name);
    printf("\tMaker id: 0x%02x",nand_dev_info->maker_code);
    printf("\tDev id: 0x%02x",nand_dev_info->dev_id);
    printf("\nNand Page shift: %d",nand_dev_info->page_shift);
    printf("\tSector shift: %d",nand_dev_info->block_shift);
    printf("\trow: %d",nand_dev_info->row_cycles);
    printf("\tcol: %d\n",nand_dev_info->col_cycles);
    return 0;
}


/* Before this func , NAND_init() must be executed */
int NandSearch(nand_info * nand_dev_info){
   u8 dev_id[8];
   u8 dev_id2[4];
   int is_mlc;
   int gcNfDidType,gcNfMidType;
   int i;
   u8   gcNfType ,gcNfShiftBlockPage ,gcNfDeviceZone,gbNfExtAddr;

   Nand_Read_ID(dev_id,4,0x90);
   //Nand_Read_ID(dev_id2,4,0x91);  
     printf(" nand dev_id = 0x%x%x%x%x\n",dev_id[3],dev_id[2],dev_id[1],dev_id[0]);

   nand_dev_info->maker_code = dev_id[0];
   nand_dev_info->dev_id = dev_id[1];
   nand_dev_info->extra_1 = dev_id[2];
   nand_dev_info->extra_2 = dev_id[3];
   nand_dev_info->extra_3 = *((ulong *)dev_id2);


	for(gcNfDidType=0;  gcNfDidType<nand_dev_info->chip_table->max_devid;  gcNfDidType++)
		if(nand_dev_info->chip_table->DevidTable[gcNfDidType]==dev_id[1])
			break;
	if(gcNfDidType==nand_dev_info->chip_table->max_devid)
	{
		for(gcNfDidType=0;  gcNfDidType<nand_dev_info->chip_table->max_devid ;  gcNfDidType++)
			if(nand_dev_info->chip_table->LvDevidTable[gcNfDidType]==dev_id[1])
				break;		
	}

    if(gcNfDidType==nand_dev_info->chip_table->max_devid)
	{	// unknown device id
		return -1;
	}

    is_mlc = 0;
    for (i=0;i<nand_dev_info->chip_table->max_mlc;i++){
       if (dev_id2[0] == nand_dev_info->chip_table->MLCTable[i])
       {
           is_mlc = 1;
           break;
       }
    }


    for(gcNfMidType=0;  gcNfMidType<nand_dev_info->chip_table->max_maker;  gcNfMidType++)
		if(nand_dev_info->chip_table->MakerTable[gcNfMidType] == dev_id[0]){
   //         nand_dev_info->maker_name = (u8 *)(&NfMakerName.SAMSUNG) + gcNfMidType;
			break;
        }

	if(is_mlc)
	{	// MLC
		gcNfType = nand_dev_info->chip_table->MLCParaTable[gcNfDidType].NfType;
		nand_dev_info->block_shift = nand_dev_info->chip_table->MLCParaTable[gcNfDidType].ShiftBlockPage;
		gcNfDeviceZone = nand_dev_info->chip_table->MLCParaTable[gcNfDidType].DeviceZone;
		gbNfExtAddr = nand_dev_info->chip_table->MLCParaTable[gcNfDidType].ExtAddr;
	}
	else
	{
        gcNfType = nand_dev_info->chip_table->ParaTable[gcNfDidType].NfType;
        nand_dev_info->block_shift = nand_dev_info->chip_table->ParaTable[gcNfDidType].ShiftBlockPage;
        gcNfDeviceZone = nand_dev_info->chip_table->ParaTable[gcNfDidType].DeviceZone;
        gbNfExtAddr = nand_dev_info->chip_table->ParaTable[gcNfDidType].ExtAddr;
	}

	if(gcNfType==0xff)
	{	// unknown device type
		return	-2;
	}

	if(gcNfType==0)
	{
	  //  gcNfShiftPageByte = 9;
        nand_dev_info->page_shift = 9;
        nand_dev_info->col_cycles = 1;
	}
	else if((gcNfType==2)||(gcNfType==3))
	{
        nand_dev_info->page_shift = 11;
        nand_dev_info->col_cycles = 2;
	//	gcNfShiftPageByte = 11;
	}
    else { // device type defined for future
        // High 2 bit :col_cycles, Low 6 bit: page_shift
        nand_dev_info->page_shift = (gcNfType&0x3f);
        nand_dev_info->col_cycles = (gcNfType>>6);       
    }

    nand_dev_info->row_cycles = gbNfExtAddr + 2;

return 0;
}


#define IS_DMA_NAND_COMPLETE() ((inl(HW_APBH_CH4_SEMA+ (0x70*nand_current_cen))&0x00ff0000)==0)
#define NAND_PKG_NUM 6


void Nand_Reset();

#define MAX_RBB_WAIT 0x200000


void Nand_Reset(){
AS3310_GPMI * gpmi_ptr;
gpmi_ptr = AS3310_GetBase_GPMI();    	
gpmi_ptr->CTRL0[0] = 0x20820001;  // cmd         
gpmi_ptr->DATA[0] = 0xff;  // reset
}


void start_nand_dma_routine(ulong addr,int pkg_num){
int wait_dma;

    outl(addr ,HW_APBH_CH4_NXTCMDAR + (0x70*nand_current_cen));
    outl(pkg_num ,HW_APBH_CH4_SEMA + (0x70*nand_current_cen));

    wait_dma = 0;
    while (IS_DMA_NAND_COMPLETE()==0) {
    if (wait_dma++ > MAX_RBB_WAIT) {
        puts("DMA No Ready\n"); 
        break;
    }
    }
}

void Nand_Read_AS3310(uchar page_shift,ulong addr,ulong* buffer,ulong length){
unsigned long length_left;
int n_read;
int endread;
int i,j;
AS3310_GPMI * gpmi_ptr;
int will_use_01;
uchar debug[8];

ulong cmd_read;
ulong cmd_30;
ulong wait_dma;

// ulong temp;
// ulong temp2;

AS3310_DMA_PKG nand_dma_pkg[NAND_PKG_NUM];

 for (i=0;i<NAND_PKG_NUM-1;i++) {
     nand_dma_pkg[i].NEXT_PKG =  (ulong) ( &( nand_dma_pkg[i+1].NEXT_PKG ));
 }

	gpmi_ptr = AS3310_GetBase_GPMI();	
	endread = 0;
	length_left=length;	
    will_use_01 = 0;
    cmd_30 = 0x30;

	for(;;){// main loop

        i=0;

        if ((page_shift == 9)&&((addr&0x100) != 0)) 
            cmd_read = 0x01;
        else cmd_read = 0x00;

        // block read cmd
        nand_dma_pkg[0].CTRL = 0x000110c6;
        nand_dma_pkg[0].BUFFER = (ulong)(&cmd_read);
        nand_dma_pkg[0].CMD0 = 0x20820001 + (nand_current_cen<<20);

 //       puts("\nNand Send Read Addr: ");
        debug[i++] = addr&0xff;  // 1st col addr
  //      putb(debug[i-1]);putc(' ');

        if (page_shift > 9) {// page size > 512 Byte  2nd col addr
            debug[i++] = (addr&((1<<page_shift)-1))>>8;
  //          putb(debug[i-1]);putc(' ');
            }

        for (j=0;i<nand_cycles;) { // row addr
            debug[i++] =  (addr>>(page_shift + (j++)*8 ) ) & 0xff;
    //        putb(debug[i-1]);putc(' ');
        }

        // block read addr
        nand_dma_pkg[1].CTRL = 0x000010c6 + (nand_cycles<<16);
        nand_dma_pkg[1].BUFFER = (ulong)debug;
        nand_dma_pkg[1].CMD0 = 0x20840000 + (nand_current_cen<<20) + nand_cycles ;  // n cycles addr

        if (page_shift > 9) {// page size > 512 Byte , send 30
            // send 0x30
            nand_dma_pkg[2].CTRL = 0x000110c6;
            nand_dma_pkg[2].BUFFER = (ulong)(&cmd_30);
            nand_dma_pkg[2].CMD0 = 0x20820001 + (nand_current_cen<<20);
        }
        else{ // jump dma pkg[2]
            nand_dma_pkg[1].NEXT_PKG =  (ulong) ( &( nand_dma_pkg[3].NEXT_PKG ));
        }

        // wait rbb
        nand_dma_pkg[3].CTRL = 0x000010e4 ;
        nand_dma_pkg[3].BUFFER = (ulong)debug;
        nand_dma_pkg[3].CMD0 = 0x2b800000 + (nand_current_cen<<20) ; 


        if (page_shift == 9) {page_shift--;will_use_01 = 1;}
        n_read=(( (1<<page_shift) - (addr&((1<<page_shift)-1)))>>2);	// count bytes left in this page
        if ((page_shift == 8)&&(will_use_01 == 1)) page_shift++;

        if (length_left <= (n_read<<2)) {endread=1 ; n_read = (length_left>>2) ;}
        // check if this page is enough for needs
  //      puts("\nRead Bytes = 0x");puth(n_read<<2);putc('\n');

        // wait rbb
        nand_dma_pkg[4].CTRL = 0x000010c1 + (n_read<<18);
        nand_dma_pkg[4].BUFFER = (ulong)buffer;
        nand_dma_pkg[4].CMD0 = 0x21800000 + (nand_current_cen<<20) + (n_read<<2); // read xxxx bytes


        if (page_shift > 9)
            start_nand_dma_routine((ulong)(&(nand_dma_pkg[0].NEXT_PKG)),5);
        else
            start_nand_dma_routine((ulong)(&(nand_dma_pkg[0].NEXT_PKG)),4);


        buffer += n_read;
        addr = addr + 4*n_read;


        length_left=length_left-(n_read<<2);
        if (endread == 1) break; 

    }



}

/*
void Nand_Read_AS3310_partly_dma(uchar page_shift,ulong addr,ulong* buffer,ulong length){
unsigned long length_left;
int n_read;
int endread;
int i,j;
AS3310_GPMI * gpmi_ptr;
int will_use_01;
uchar debug[8];

int k;
char * bptr;
int * iptr;

ulong cmd_read;
ulong cmd_30;
ulong wait_dma;

// ulong temp;
// ulong temp2;

AS3310_DMA_PKG nand_dma_pkg[NAND_PKG_NUM];

 for (i=0;i<NAND_PKG_NUM-1;i++) {
     nand_dma_pkg[i].NEXT_PKG =  (ulong) ( &( nand_dma_pkg[i+1].NEXT_PKG ));
 }

	gpmi_ptr = AS3310_GetBase_GPMI();	
	endread = 0;
	length_left=length;	
    will_use_01 = 0;
    cmd_30 = 0x30;

	for(;;){// main loop

        i=0;

        if ((page_shift == 9)&&((addr&0x100) != 0)) 
            cmd_read = 0x01;
        else cmd_read = 0x00;

//        // block read cmd
//        nand_dma_pkg[0].CTRL = 0x000110c6;
//        nand_dma_pkg[0].BUFFER = (ulong)(&cmd_read);
//        nand_dma_pkg[0].CMD0 = 0x20820001 + (nand_current_cen<<20);

        gpmi_ptr->CTRL0[0] = 0x20820001 + (nand_current_cen<<20);
        gpmi_ptr->DATA[0] = cmd_read;

 //       puts("\nNand Send Read Addr: ");
        debug[i++] = addr&0xff;  // 1st col addr
  //      putb(debug[i-1]);putc(' ');

        if (page_shift > 9) {// page size > 512 Byte  2nd col addr
            debug[i++] = (addr&((1<<page_shift)-1))>>8;
  //          putb(debug[i-1]);putc(' ');
            }

        for (j=0;i<nand_cycles;) { // row addr
            debug[i++] =  (addr>>(page_shift + (j++)*8 ) ) & 0xff;
    //        putb(debug[i-1]);putc(' ');
        }


        for (k=0;k<nand_cycles;k++) { // nand addr
            while ((gpmi_ptr->STAT[0] & (1<<5))==0);
            gpmi_ptr->CTRL0[0] = 0x20840001 + (nand_current_cen<<20);
            outb(debug[k],&gpmi_ptr->DATA[0]);        
       //     printf(" %x ",debug[k]);
        }

  //      // block read addr
  //      nand_dma_pkg[1].CTRL = 0x000010c6 + (nand_cycles<<16);
  //      nand_dma_pkg[1].BUFFER = (ulong)debug;
  //      nand_dma_pkg[1].CMD0 = 0x20840000 + (nand_current_cen<<20) + nand_cycles ;  // n cycles addr

        if (page_shift > 9) {// page size > 512 Byte , send 30
            // send 0x30
           nand_dma_pkg[2].CTRL = 0x000110c6;
           nand_dma_pkg[2].BUFFER = (ulong)(&cmd_30);
           nand_dma_pkg[2].CMD0 = 0x20820001 + (nand_current_cen<<20);

     //       while ((gpmi_ptr->STAT[0] & (1<<5))==0);
     //       gpmi_ptr->CTRL0[0] = 0x20820001 + (nand_current_cen<<20);
     //       outb(cmd_30,&gpmi_ptr->DATA[0]);        
        }
        else{ // jump dma pkg[2]
            nand_dma_pkg[1].NEXT_PKG =  (ulong) ( &( nand_dma_pkg[3].NEXT_PKG ));
        }

        // wait rbb
        nand_dma_pkg[3].CTRL = 0x000010e0 ;
        nand_dma_pkg[3].BUFFER = (ulong)debug;
        nand_dma_pkg[3].CMD0 = 0x2b800000 + (nand_current_cen<<20) ; 


        if (page_shift == 9) {page_shift--;will_use_01 = 1;}
        n_read=(( (1<<page_shift) - (addr&((1<<page_shift)-1)))>>2);	// count bytes left in this page
        if ((page_shift == 8)&&(will_use_01 == 1)) page_shift++;

        if (length_left <= (n_read<<2)) {endread=1 ; n_read = (length_left>>2) ;}
        // check if this page is enough for needs
  //      puts("\nRead Bytes = 0x");puth(n_read<<2);putc('\n');

        // read
        nand_dma_pkg[4].CTRL = 0x000010c1 + (n_read<<17);
        nand_dma_pkg[4].BUFFER = (ulong)buffer;
        nand_dma_pkg[4].CMD0 = 0x21800000 + (nand_current_cen<<20) + (n_read<<1); // read xxxx bytes
 
        // read 2
        nand_dma_pkg[5].CTRL = 0x000010c1 + (n_read<<17);
        nand_dma_pkg[5].BUFFER = ((ulong)buffer) + (n_read<<1);
        nand_dma_pkg[5].CMD0 = 0x21800000 + (nand_current_cen<<20) + (n_read<<1); // read xxxx bytes

        start_nand_dma_routine((ulong)(&(nand_dma_pkg[2].NEXT_PKG)),2);
        printf("RBB Done\n");
        start_nand_dma_routine((ulong)(&(nand_dma_pkg[4].NEXT_PKG)),1);
  //      start_nand_dma_routine((ulong)(&(nand_dma_pkg[5].NEXT_PKG)),1);

        gpmi_ptr->CTRL0[0] = 0x21800000 + (n_read<<1) + (nand_current_cen<<20);
        delay(1000);
        bptr = ((ulong)buffer) + (n_read<<1);;
        iptr = ((ulong)buffer) + (n_read<<1);;

       for (k = 0; k<(n_read>>1); k++) {
           delay(10);
           *(iptr++) = gpmi_ptr->DATA[0];         
    //   for (k = 0; k< (n_read<<2); k++) {
    //       *(bptr++) = inb(&gpmi_ptr->DATA[0]); 
        }

        buffer += n_read;
        addr = addr + 4*n_read;


        length_left=length_left-(n_read<<2);
        if (endread == 1) break; 

    }



}
*/



void Nand_Read_ID(ulong* dev_id,int n_byte,uchar cmd){

int i ;
AS3310_DMA_PKG nand_dma_pkg[NAND_PKG_NUM];
int wait_dma;
int temp,tmp_cmd;

 for (i=0;i<NAND_PKG_NUM-1;i++) {
     nand_dma_pkg[i].NEXT_PKG =  (ulong) ( &( nand_dma_pkg[i+1].NEXT_PKG ));
 }

  /* w 0x90 or other */
  tmp_cmd = cmd;
  nand_dma_pkg[0].CTRL = 0x000110c6;
  nand_dma_pkg[0].BUFFER = (ulong)(&cmd);
  nand_dma_pkg[0].CMD0 = 0x20820001 + (nand_current_cen<<20);

  /* w 0x00  */
  temp = 0;
  nand_dma_pkg[1].CTRL = 0x000110c6;
  nand_dma_pkg[1].BUFFER = (ulong)(&temp);
  nand_dma_pkg[1].CMD0 = 0x20840001 + (nand_current_cen<<20);

  /* read n bytes  */
  nand_dma_pkg[2].CTRL = 0x000010c1 + (n_byte<<16);
  nand_dma_pkg[2].BUFFER = (ulong)dev_id;
  nand_dma_pkg[2].CMD0 =  0x21800000 + (nand_current_cen<<20) + n_byte;

  start_nand_dma_routine((ulong)(&(nand_dma_pkg[0].NEXT_PKG)),3);

}


/* Write NAND Flash*/

void Nand_Write_AS3310(uchar page_shift,ulong addr,ulong* buffer,ulong length){
unsigned long length_left;
int n_write;
int endwrite;
int i,j ;
uchar debug[8];
ulong *addr_ptr;

ulong cmd_write;
ulong cmd_write_end;
ulong wait_dma;

// ulong temp;
// ulong temp2;

AS3310_DMA_PKG nand_dma_pkg[NAND_PKG_NUM];

 for (i=0;i<NAND_PKG_NUM-1;i++) {
     nand_dma_pkg[i].NEXT_PKG =  (ulong) ( &( nand_dma_pkg[i+1].NEXT_PKG ));
 }

	endwrite = 0;
    cmd_write_end = 0x10;
    cmd_write = 0x80;
	length_left=length;		

    //puts("Nand Cycles: 0x");putb(nand_cycles);
    //puts("\tNand Page shift: 0x");putb(page_shift);
       
	for(;;){// main loop    
		i=0;

        // block write cmd
        nand_dma_pkg[0].CTRL = 0x000110c6;
        nand_dma_pkg[0].BUFFER = (ulong)(&cmd_write);
        nand_dma_pkg[0].CMD0 = 0x20820001 + (nand_current_cen<<20);


   //     puts("\nNand Send Write Addr: ");
        debug[i++] = addr&0xff;  // 1st col addr
  //      putb(debug[i-1]);putc(' ');

        if (page_shift > 9) {// page size > 512 Byte  2nd col addr
            debug[i++] = (addr&((1<<page_shift)-1))>>8;
    //        putb(debug[i-1]);putc(' ');
            }

        for (j=0;i<nand_cycles;) { // row addr
            debug[i++] =  (addr>>(page_shift + (j++)*8 ) ) & 0xff;
    //        putb(debug[i-1]);putc(' ');
        }

        // block write addr
        nand_dma_pkg[1].CTRL = 0x000010c6 + (nand_cycles<<16);
        nand_dma_pkg[1].BUFFER = (ulong)debug;
        nand_dma_pkg[1].CMD0 = 0x20840000 + (nand_current_cen<<20) + nand_cycles ;  // n cycles addr

        n_write=(( (1<<page_shift) - (addr&((1<<page_shift)-1)))>>2);	// count bytes left in this page

        if (length_left <= (n_write<<2)) {endwrite=1 ; n_write = (length_left>>2) ;}
		// check if this page is enough for needs

        nand_dma_pkg[2].CTRL = 0x000010c6 + (n_write<<18);
        nand_dma_pkg[2].BUFFER = (ulong)buffer;
        nand_dma_pkg[2].CMD0 = 0x20800000 + (nand_current_cen<<20) + (n_write<<2); // write xxxx bytes
        addr = addr + (n_write<<2);
        buffer = buffer + n_write;


        nand_dma_pkg[3].CTRL = 0x000110c6;
        nand_dma_pkg[3].BUFFER = (ulong)(&cmd_write_end);
        nand_dma_pkg[3].CMD0 = 0x20820001 + (nand_current_cen<<20); // end cmd


        // wait rbb
        nand_dma_pkg[4].CTRL = 0x000010e0 ;
        nand_dma_pkg[4].BUFFER = (ulong)debug;
        nand_dma_pkg[4].CMD0 = 0x2b800000 + (nand_current_cen<<20) ; 

        // Start DMA

        start_nand_dma_routine((ulong)(&(nand_dma_pkg[0].NEXT_PKG)),5);


        length_left=length_left-(n_write<<2);
        if (endwrite == 1) break; 

    }

}


/*
NAND Erase
*/

void NAND_Erase(uchar nand_row_cycles,ulong addr)
{
int i ,j;
uchar debug[5];
ulong cmd_erase,cmd_erase_end;
int wait_dma;
AS3310_DMA_PKG nand_dma_pkg[NAND_PKG_NUM];


    for (i=0;i<NAND_PKG_NUM-1;i++) {
        nand_dma_pkg[i].NEXT_PKG =  (ulong) ( &( nand_dma_pkg[i+1].NEXT_PKG ));
    }

    cmd_erase_end = 0xd0;
    cmd_erase = 0x60;


    //Nand_Reset();
    // 
    // block erase cmd
    nand_dma_pkg[0].CTRL = 0x000110c6;
    nand_dma_pkg[0].BUFFER = (ulong)(&cmd_erase);
    nand_dma_pkg[0].CMD0 = 0x20820001 + (nand_current_cen<<20);


    for (i=0,j=0; i < nand_row_cycles ; ) { // row addr
        debug[i++] =  (addr>>(nand_page_shift + (j++)*8 ) ) & 0xff;
 //       putb(debug[i-1]);putc(' ');
        }

    // block write addr
    nand_dma_pkg[1].CTRL = 0x000010c6 + (nand_row_cycles<<16);
    nand_dma_pkg[1].BUFFER = (ulong)debug;
    nand_dma_pkg[1].CMD0 =  0x20840000 + (nand_current_cen<<20) + nand_row_cycles;  // n cycles addr
    

    // block erase cmd
    nand_dma_pkg[2].CTRL = 0x000110c6;
    nand_dma_pkg[2].BUFFER = (ulong)(&cmd_erase_end);
    nand_dma_pkg[2].CMD0 = 0x20820001 + (nand_current_cen<<20);


    // wait rbb
    nand_dma_pkg[3].CTRL = 0x000010e0 ;
    nand_dma_pkg[3].BUFFER = (ulong)debug;
    nand_dma_pkg[3].CMD0 = 0x2b800000 + (nand_current_cen<<20) ; 

    // Start DMA

    start_nand_dma_routine((ulong)(&(nand_dma_pkg[0].NEXT_PKG)),4);

}



void Nand_Read_Spare_AS3310(ulong addr,char * buffer){
int n_read;
int i,j;
int will_use_01;
uchar debug[8];

ulong cmd_read;
ulong cmd_30;

// ulong temp;
// ulong temp2;

AS3310_DMA_PKG nand_dma_pkg[NAND_PKG_NUM];

 for (i=0;i<NAND_PKG_NUM-1;i++) {
     nand_dma_pkg[i].NEXT_PKG =  (ulong) ( &( nand_dma_pkg[i+1].NEXT_PKG ));
 }

    will_use_01 = 0;
    cmd_30 = 0x30;

        i=0;

        if ((nand_page_shift == 9)&&((addr&0x100) != 0)) 
            cmd_read = 0x01;
        else cmd_read = 0x00;

        // block read cmd
        nand_dma_pkg[0].CTRL = 0x000110c6;
        nand_dma_pkg[0].BUFFER = (ulong)(&cmd_read);
        nand_dma_pkg[0].CMD0 = 0x20820001 + (nand_current_cen<<20);

        //puts("\nNand Send Read Addr: ");
        debug[i++] = 0;  // 1st col addr
        //putb(debug[i-1]);putc(' ');

        if (nand_page_shift > 9) {// page size > 512 Byte  2nd col addr
            debug[i++] = (1<<(nand_page_shift-8));
        //    putb(debug[i-1]);putc(' ');
            }

        for (j=0;i<nand_cycles;) { // row addr
            debug[i++] =  (addr>>(nand_page_shift + (j++)*8 ) ) & 0xff;
        //    putb(debug[i-1]);putc(' ');
        }

        // block read addr
        nand_dma_pkg[1].CTRL = 0x000010c6 + (nand_cycles<<16);
        nand_dma_pkg[1].BUFFER = (ulong)debug;
        nand_dma_pkg[1].CMD0 = 0x20840000 + (nand_current_cen<<20) + nand_cycles ;  // n cycles addr

        if (nand_page_shift > 9) {// page size > 512 Byte , send 30
            // send 0x30
            nand_dma_pkg[2].CTRL = 0x000110c6;
            nand_dma_pkg[2].BUFFER = (ulong)(&cmd_30);
            nand_dma_pkg[2].CMD0 = 0x20820001 + (nand_current_cen<<20);
        }
        else{ // jump dma pkg[2]
            nand_dma_pkg[1].NEXT_PKG =  (ulong) ( &( nand_dma_pkg[3].NEXT_PKG ));
        }

        // wait rbb
        nand_dma_pkg[3].CTRL = 0x000010e4 ;
        nand_dma_pkg[3].BUFFER = (ulong)debug;
        nand_dma_pkg[3].CMD0 = 0x2b800000 + (nand_current_cen<<20) ; 


        if (nand_page_shift <= 9)  n_read = (16>>2);
        else  n_read = (64>>2);

        // check if this page is enough for needs
        //puts("\nRead Bytes = 0x");puth(n_read<<2);putc('\n');

        // wait rbb
        nand_dma_pkg[4].CTRL = 0x000010c1 + (n_read<<18);
        nand_dma_pkg[4].BUFFER = (ulong)buffer;
        nand_dma_pkg[4].CMD0 = 0x21800000 + (nand_current_cen<<20) + (n_read<<2); // read xxxx bytes


        if (nand_page_shift > 9)
            start_nand_dma_routine((ulong)(&(nand_dma_pkg[0].NEXT_PKG)),5);
        else
            start_nand_dma_routine((ulong)(&(nand_dma_pkg[0].NEXT_PKG)),4);


}


void Nand_Write_AS3310_Spare(ulong addr,char * buffer){
int n_write;
int i,j ;
uchar debug[8];

ulong cmd_write;
ulong cmd_write_end;

// ulong temp;
// ulong temp2;

AS3310_DMA_PKG nand_dma_pkg[NAND_PKG_NUM];

 for (i=0;i<NAND_PKG_NUM-1;i++) {
     nand_dma_pkg[i].NEXT_PKG =  (ulong) ( &( nand_dma_pkg[i+1].NEXT_PKG ));
 }

    cmd_write_end = 0x10;
    cmd_write = 0x80;

    //puts("Nand Cycles: 0x");putb(nand_cycles);
    //puts("\tNand Page shift: 0x");putb(page_shift);
       
    i=0;

    // block write cmd
    nand_dma_pkg[0].CTRL = 0x000110c6;
    nand_dma_pkg[0].BUFFER = (ulong)(&cmd_write);
    nand_dma_pkg[0].CMD0 = 0x20820001 + (nand_current_cen<<20);

    debug[i++] = 0 ;  // 1st col addr

    if (nand_page_shift > 9) {// page size > 512 Byte  2nd col addr
        debug[i++] = (1<< (nand_page_shift - 8));
    }

    for (j=0;i<nand_cycles;) { // row addr
        debug[i++] =  (addr>>(nand_page_shift + (j++)*8 ) ) & 0xff;
    }

        // block write addr
    nand_dma_pkg[1].CTRL = 0x000010c6 + (nand_cycles<<16);
    nand_dma_pkg[1].BUFFER = (ulong)debug;
    nand_dma_pkg[1].CMD0 = 0x20840000 + (nand_current_cen<<20) + nand_cycles ;  // n cycles addr

    if (nand_page_shift <= 9)  n_write = (16>>2);
    else  n_write = (64>>2);

    nand_dma_pkg[2].CTRL = 0x000010c6 + (n_write<<18);
    nand_dma_pkg[2].BUFFER = (ulong)buffer;
    nand_dma_pkg[2].CMD0 = 0x20800000 + (nand_current_cen<<20) + (n_write<<2); // write xxxx bytes

    nand_dma_pkg[3].CTRL = 0x000110c6;
    nand_dma_pkg[3].BUFFER = (ulong)(&cmd_write_end);
    nand_dma_pkg[3].CMD0 = 0x20820001 + (nand_current_cen<<20); // end cmd

    // wait rbb
    nand_dma_pkg[4].CTRL = 0x000010e0 ;
    nand_dma_pkg[4].BUFFER = (ulong)debug;
    nand_dma_pkg[4].CMD0 = 0x2b800000 + (nand_current_cen<<20) ; 

        // Start DMA

    start_nand_dma_routine((ulong)(&(nand_dma_pkg[0].NEXT_PKG)),5);
}

#endif //__HAVE_NAND_FLASH_



