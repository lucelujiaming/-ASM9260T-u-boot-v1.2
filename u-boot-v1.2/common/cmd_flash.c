/*
 * (C) Copyright 2000
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
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

/*
 * FLASH support
 */
#include <common.h>
#include <command.h>


#ifdef CONFIG_HAS_DATAFLASH
#include <dataflash.h>
#endif

#if (CONFIG_COMMANDS & CFG_CMD_FLASH)

extern flash_info_t flash_info[];	/* info for FLASH chips */


/* Nand configure */
extern uchar nand_cycles;
extern uchar nand_row_cycles;
extern uchar nand_col_cycles;
extern uchar nand_page_shift;
extern uchar nand_block_shift;
extern ulong nand_current_cen;
/*
 * The user interface starts numbering for Flash banks with 1
 * for historical reasons.
 */

/*
 * this routine looks for an abbreviated flash range specification.
 * the syntax is B:SF[-SL], where B is the bank number, SF is the first
 * sector to erase, and SL is the last sector to erase (defaults to SF).
 * bank numbers start at 1 to be consistent with other specs, sector numbers
 * start at zero.
 *
 * returns:	1	- correct spec; *pinfo, *psf and *psl are
 *			  set appropriately
 *		0	- doesn't look like an abbreviated spec
 *		-1	- looks like an abbreviated spec, but got
 *			  a parsing error, a number out of range,
 *			  or an invalid flash bank.
 */
static int
abbrev_spec (char *str, flash_info_t ** pinfo, int *psf, int *psl)
{
	flash_info_t *fp;
	int bank, first, last;
	char *p, *ep;

	if ((p = strchr (str, ':')) == NULL)
		return 0;
	*p++ = '\0';

	bank = simple_strtoul (str, &ep, 10);
	if (ep == str || *ep != '\0' ||
		bank < 1 || bank > CFG_MAX_FLASH_BANKS ||
		(fp = &flash_info[bank - 1])->flash_id == FLASH_UNKNOWN)
		return -1;

	str = p;
	if ((p = strchr (str, '-')) != NULL)
		*p++ = '\0';

	first = simple_strtoul (str, &ep, 10);
	if (ep == str || *ep != '\0' || first >= fp->sector_count)
		return -1;

	if (p != NULL) {
		last = simple_strtoul (p, &ep, 10);
		if (ep == p || *ep != '\0' ||
			last < first || last >= fp->sector_count)
			return -1;
	} else {
		last = first;
	}

	*pinfo = fp;
	*psf = first;
	*psl = last;

	return 1;
}

static int
flash_fill_sect_ranges (ulong addr_first, ulong addr_last,
			int *s_first, int *s_last,
			int *s_count )
{
	flash_info_t *info;
	ulong bank;
	int rcode = 0;

	*s_count = 0;

	for (bank=0; bank < CFG_MAX_FLASH_BANKS; ++bank) {
		s_first[bank] = -1;	/* first sector to erase	*/
		s_last [bank] = -1;	/* last  sector to erase	*/
	}

	for (bank=0,info=&flash_info[0];
	     (bank < CFG_MAX_FLASH_BANKS) && (addr_first <= addr_last);
	     ++bank, ++info) {
		ulong b_end;
		int sect;
		short s_end;

		if (info->flash_id == FLASH_UNKNOWN) {
			continue;
		}

		b_end = info->start[0] + info->size - 1;	/* bank end addr */
		s_end = info->sector_count - 1;			/* last sector   */


		for (sect=0; sect < info->sector_count; ++sect) {
			ulong end;	/* last address in current sect	*/

			end = (sect == s_end) ? b_end : info->start[sect + 1] - 1;

			if (addr_first > end)
				continue;
			if (addr_last < info->start[sect])
				continue;

			if (addr_first == info->start[sect]) {
				s_first[bank] = sect;
			}
			if (addr_last  == end) {
				s_last[bank]  = sect;
			}
		}
		if (s_first[bank] >= 0) {
			if (s_last[bank] < 0) {
				if (addr_last > b_end) {
					s_last[bank] = s_end;
				} else {
					puts ("Error: end address"
						" not on sector boundary\n");
					rcode = 1;
					break;
				}
			}
			if (s_last[bank] < s_first[bank]) {
				puts ("Error: end sector"
					" precedes start sector\n");
				rcode = 1;
				break;
			}
			sect = s_last[bank];
			addr_first = (sect == s_end) ? b_end + 1: info->start[sect + 1];
			(*s_count) += s_last[bank] - s_first[bank] + 1;
		} else if (s_last[bank] >= 0) {
			puts ("Error: cannot span across banks when they are"
			       " mapped in reverse order\n");
			rcode = 1;
			break;
		}
	}

	return rcode;
}

int do_flinfo ( cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	ulong bank;

#ifdef CONFIG_HAS_DATAFLASH
	dataflash_print_info();
#endif

	if (argc == 1) {	/* print info for all FLASH banks */
		for (bank=0; bank <CFG_MAX_FLASH_BANKS; ++bank) {
			printf ("\nBank # %ld: ", bank+1);

			flash_print_info (&flash_info[bank]);
		}
		return 0;
	}

	bank = simple_strtoul(argv[1], NULL, 16);
	if ((bank < 1) || (bank > CFG_MAX_FLASH_BANKS)) {
		printf ("Only FLASH Banks # 1 ... # %d supported\n",
			CFG_MAX_FLASH_BANKS);
		return 1;
	}
	printf ("\nBank # %ld: ", bank);
	flash_print_info (&flash_info[bank-1]);
	return 0;
}
int do_flerase (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	flash_info_t *info;
	ulong bank, addr_first, addr_last;
	int n, sect_first, sect_last;
	int rcode = 0;

	if (argc < 2) {
		printf ("Usage:\n%s\n", cmdtp->usage);
		return 1;
	}

	if (strcmp(argv[1], "all") == 0) {
		for (bank=1; bank<=CFG_MAX_FLASH_BANKS; ++bank) {
			printf ("Erase Flash Bank # %ld ", bank);
			info = &flash_info[bank-1];
			rcode = flash_erase (info, 0, info->sector_count-1);
		}
		return rcode;
	}

	if ((n = abbrev_spec(argv[1], &info, &sect_first, &sect_last)) != 0) {
		if (n < 0) {
			puts ("Bad sector specification\n");
			return 1;
		}
		printf ("Erase Flash Sectors %d-%d in Bank # %d ",
			sect_first, sect_last, (info-flash_info)+1);
		rcode = flash_erase(info, sect_first, sect_last);
		return rcode;
	}

	if (argc != 3) {
		printf ("Usage:\n%s\n", cmdtp->usage);
		return 1;
	}

	if (strcmp(argv[1], "bank") == 0) {
		bank = simple_strtoul(argv[2], NULL, 16);
		if ((bank < 1) || (bank > CFG_MAX_FLASH_BANKS)) {
			printf ("Only FLASH Banks # 1 ... # %d supported\n",
				CFG_MAX_FLASH_BANKS);
			return 1;
		}
		printf ("Erase Flash Bank # %ld ", bank);
		info = &flash_info[bank-1];
		rcode = flash_erase (info, 0, info->sector_count-1);
		return rcode;
	}

	addr_first = simple_strtoul(argv[1], NULL, 16);
	addr_last  = simple_strtoul(argv[2], NULL, 16);

	if (addr_first >= addr_last) {
		printf ("Usage:\n%s\n", cmdtp->usage);
		return 1;
	}

	rcode = flash_sect_erase(addr_first, addr_last);
	return rcode;
}

int flash_sect_erase (ulong addr_first, ulong addr_last)
{
	flash_info_t *info;
	ulong bank;
	int s_first[CFG_MAX_FLASH_BANKS], s_last[CFG_MAX_FLASH_BANKS];
	int erased = 0;
	int planned;
	int rcode = 0;

	rcode = flash_fill_sect_ranges (addr_first, addr_last,
					s_first, s_last, &planned );

	if (planned && (rcode == 0)) {
		for (bank=0,info=&flash_info[0];
		     (bank < CFG_MAX_FLASH_BANKS) && (rcode == 0);
		     ++bank, ++info) {
			if (s_first[bank]>=0) {
				erased += s_last[bank] - s_first[bank] + 1;
				debug ("Erase Flash from 0x%08lx to 0x%08lx "
					"in Bank # %ld ",
					info->start[s_first[bank]],
					(s_last[bank] == info->sector_count) ?
						info->start[0] + info->size - 1:
						info->start[s_last[bank]+1] - 1,
					bank+1);
				rcode = flash_erase (info, s_first[bank], s_last[bank]);
			}
		}
		printf ("Erased %d sectors\n", erased);
	} else if (rcode == 0) {
		puts ("Error: start and/or end address"
			" not on sector boundary\n");
		rcode = 1;
	}
	return rcode;
}

int do_protect (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	flash_info_t *info;
	ulong bank, addr_first, addr_last;
	int i, p, n, sect_first, sect_last;
	int rcode = 0;
#ifdef CONFIG_HAS_DATAFLASH
	int status;
#endif
	if (argc < 3) {
		printf ("Usage:\n%s\n", cmdtp->usage);
		return 1;
	}

	if (strcmp(argv[1], "off") == 0) {
		p = 0;
	} else if (strcmp(argv[1], "on") == 0) {
		p = 1;
	} else {
		printf ("Usage:\n%s\n", cmdtp->usage);
		return 1;
	}

#ifdef CONFIG_HAS_DATAFLASH
	if ((strcmp(argv[2], "all") != 0) && (strcmp(argv[2], "bank") != 0)) {
		addr_first = simple_strtoul(argv[2], NULL, 16);
		addr_last  = simple_strtoul(argv[3], NULL, 16);

		if (addr_dataflash(addr_first) && addr_dataflash(addr_last)) {
			status = dataflash_real_protect(p,addr_first,addr_last);
			if (status < 0){
				puts ("Bad DataFlash sector specification\n");
				return 1;
			}
			printf("%sProtect %d DataFlash Sectors\n",
				p ? "" : "Un-", status);
			return 0;
		}
	}
#endif

	if (strcmp(argv[2], "all") == 0) {
		for (bank=1; bank<=CFG_MAX_FLASH_BANKS; ++bank) {
			info = &flash_info[bank-1];
			if (info->flash_id == FLASH_UNKNOWN) {
				continue;
			}
			printf ("%sProtect Flash Bank # %ld\n",
				p ? "" : "Un-", bank);

			for (i=0; i<info->sector_count; ++i) {
#if defined(CFG_FLASH_PROTECTION)
				if (flash_real_protect(info, i, p))
					rcode = 1;
				putc ('.');
#else
				info->protect[i] = p;
#endif	/* CFG_FLASH_PROTECTION */
			}
		}

#if defined(CFG_FLASH_PROTECTION)
		if (!rcode) puts (" done\n");
#endif	/* CFG_FLASH_PROTECTION */

		return rcode;
	}

	if ((n = abbrev_spec(argv[2], &info, &sect_first, &sect_last)) != 0) {
		if (n < 0) {
			puts ("Bad sector specification\n");
			return 1;
		}
		printf("%sProtect Flash Sectors %d-%d in Bank # %d\n",
			p ? "" : "Un-", sect_first, sect_last,
			(info-flash_info)+1);
		for (i = sect_first; i <= sect_last; i++) {
#if defined(CFG_FLASH_PROTECTION)
			if (flash_real_protect(info, i, p))
				rcode =  1;
			putc ('.');
#else
			info->protect[i] = p;
#endif	/* CFG_FLASH_PROTECTION */
		}

#if defined(CFG_FLASH_PROTECTION)
		if (!rcode) puts (" done\n");
#endif	/* CFG_FLASH_PROTECTION */

		return rcode;
	}

	if (argc != 4) {
		printf ("Usage:\n%s\n", cmdtp->usage);
		return 1;
	}

	if (strcmp(argv[2], "bank") == 0) {
		bank = simple_strtoul(argv[3], NULL, 16);
		if ((bank < 1) || (bank > CFG_MAX_FLASH_BANKS)) {
			printf ("Only FLASH Banks # 1 ... # %d supported\n",
				CFG_MAX_FLASH_BANKS);
			return 1;
		}
		printf ("%sProtect Flash Bank # %ld\n",
			p ? "" : "Un-", bank);
		info = &flash_info[bank-1];

		if (info->flash_id == FLASH_UNKNOWN) {
			puts ("missing or unknown FLASH type\n");
			return 1;
		}
		for (i=0; i<info->sector_count; ++i) {
#if defined(CFG_FLASH_PROTECTION)
			if (flash_real_protect(info, i, p))
				rcode =  1;
			putc ('.');
#else
			info->protect[i] = p;
#endif	/* CFG_FLASH_PROTECTION */
		}

#if defined(CFG_FLASH_PROTECTION)
		if (!rcode) puts (" done\n");
#endif	/* CFG_FLASH_PROTECTION */

		return rcode;
	}

	addr_first = simple_strtoul(argv[2], NULL, 16);
	addr_last  = simple_strtoul(argv[3], NULL, 16);

	if (addr_first >= addr_last) {
		printf ("Usage:\n%s\n", cmdtp->usage);
		return 1;
	}
	rcode = flash_sect_protect (p, addr_first, addr_last);
	return rcode;
}


int flash_sect_protect (int p, ulong addr_first, ulong addr_last)
{
	flash_info_t *info;
	ulong bank;
	int s_first[CFG_MAX_FLASH_BANKS], s_last[CFG_MAX_FLASH_BANKS];
	int protected, i;
	int planned;
	int rcode;

	rcode = flash_fill_sect_ranges( addr_first, addr_last, s_first, s_last, &planned );

	protected = 0;

	if (planned && (rcode == 0)) {
		for (bank=0,info=&flash_info[0]; bank < CFG_MAX_FLASH_BANKS; ++bank, ++info) {
			if (info->flash_id == FLASH_UNKNOWN) {
				continue;
			}

			if (s_first[bank]>=0 && s_first[bank]<=s_last[bank]) {
				debug ("%sProtecting sectors %d..%d in bank %ld\n",
					p ? "" : "Un-",
					s_first[bank], s_last[bank], bank+1);
				protected += s_last[bank] - s_first[bank] + 1;
				for (i=s_first[bank]; i<=s_last[bank]; ++i) {
#if defined(CFG_FLASH_PROTECTION)
					if (flash_real_protect(info, i, p))
						rcode = 1;
					putc ('.');
#else
					info->protect[i] = p;
#endif	/* CFG_FLASH_PROTECTION */
				}
			}
#if defined(CFG_FLASH_PROTECTION)
			if (!rcode) putc ('\n');
#endif	/* CFG_FLASH_PROTECTION */
		}

		printf ("%sProtected %d sectors\n",
			p ? "" : "Un-", protected);
	} else if (rcode == 0) {
		puts ("Error: start and/or end address"
			" not on sector boundary\n");
		rcode = 1;
	}
	return rcode;
}


/**************************************************/

U_BOOT_CMD(
	flinfo,    2,    1,    do_flinfo,
	"flinfo  - print FLASH memory information\n",
	"\n    - print information for all FLASH memory banks\n"
	"flinfo N\n    - print information for FLASH memory bank # N\n"
);

U_BOOT_CMD(
	erase,   3,   1,  do_flerase,
	"erase   - erase FLASH memory\n",
	"start end\n"
	"    - erase FLASH from addr 'start' to addr 'end'\n"
	"erase N:SF[-SL]\n    - erase sectors SF-SL in FLASH bank # N\n"
	"erase bank N\n    - erase FLASH bank # N\n"
	"erase all\n    - erase all FLASH banks\n"
);

U_BOOT_CMD(
	protect,  4,  1,   do_protect,
	"protect - enable or disable FLASH write protection\n",
	"on  start end\n"
	"    - protect FLASH from addr 'start' to addr 'end'\n"
	"protect on  N:SF[-SL]\n"
	"    - protect sectors SF-SL in FLASH bank # N\n"
	"protect on  bank N\n    - protect FLASH bank # N\n"
	"protect on  all\n    - protect all FLASH banks\n"
	"protect off start end\n"
	"    - make FLASH from addr 'start' to addr 'end' writable\n"
	"protect off N:SF[-SL]\n"
	"    - make sectors SF-SL writable in FLASH bank # N\n"
	"protect off bank N\n    - make FLASH bank # N writable\n"
	"protect off all\n    - make all FLASH banks writable\n"
);

#endif	/* CFG_CMD_FLASH */



void Write_flash_cmd(cmd_tbl_t *cmdtp, int flag,int argc,char* argv[])
{
	ulong addr;
	ulong data;		
	if (argc != 3) {puts("Invalid arg. e.g.#nandwrite ADDR data");putc(0x0a);return;}	
	addr = simple_strtoul(argv[1],NULL,16);
	data = simple_strtoul(argv[2],NULL,16);	
	Nand_Write_AS3310(nand_page_shift,addr,&data,4);
}

void Erase_flash_cmd(cmd_tbl_t *cmdtp, int flag,int argc,char* argv[])
{
    ulong addr,start_block,end_block,length;
 
	if ((argc < 2)||(argc > 3)) {puts("Invalid arg. e.g.#nanderase Address [Length]");putc(0x0a);return;}	
	addr = simple_strtoul(argv[1],NULL,16);

    if (argc == 3) {    
         length = simple_strtoul(argv[2],NULL,16);
    }
    else length = 1;

         start_block = addr>>(nand_block_shift + nand_page_shift);
         end_block = (addr + length - 1)>>(nand_block_shift + nand_page_shift);
         while (start_block <= end_block ) {
              NAND_Erase(nand_row_cycles,start_block<<(nand_block_shift + nand_page_shift));
              printf("erasing block :0x%08x\n",start_block);
              start_block++ ;
         }

}

#define NAND_READ_CMD_L 256
void Read_flash_cmd(cmd_tbl_t *cmdtp, int flag,int argc,char* argv[])
{
	uchar buffer[NAND_READ_CMD_L];
	ulong addr,i;
    volatile ulong *target_addr;
	int length;	
	
	if ((argc < 3)||(argc > 4)) {puts("Invalid arg. e.g.#nandread Src_addr Length [Target_addr]");putc(0x0a);return;}
	addr = simple_strtoul(argv[1],NULL,16);
	length = simple_strtoul(argv[2],NULL,16);
    if (argc == 4){
    target_addr = (volatile ulong *)simple_strtoul(argv[3],NULL,16);
    }


	if ((length > NAND_READ_CMD_L)&&(argc == 3)) length = NAND_READ_CMD_L;
    if (argc == 3) {
        target_addr = (volatile ulong *)buffer;
    }
    Nand_Read_AS3310(nand_page_shift,addr,(ulong *)target_addr,length);

	if (argc == 3) MemDisp_TRL(buffer,length,4);
		
}



#define DISP_LINE_LEN	16
int MemDisp_TRL(uchar *buf,int total_length,int size)
{
	ulong	addr, length;
	ulong	i, nbytes, linebytes;
	uchar	*cp;
	uchar long2hex[9];

	int rc = 0;
  addr = (ulong)buf;
	long2hex[8]=0;

	/* Print the lines.
	 *
	 * We buffer all read data, so we can make sure data is read only
	 * once, and all accesses are with the specified bus width.
	 */
	nbytes = total_length;
	do {
		char	linebuf[DISP_LINE_LEN];
		ulong	*uip = (ulong   *)linebuf;
		ushort	*usp = (ushort *)linebuf;
		uchar	*ucp = (uchar *)linebuf;
/*
		puts("0x");
		Long2HEX(long2hex,addr);
		puts(long2hex);
		puts (": ");
        */
		printf("0x%08x :",addr);
		linebytes = (nbytes>DISP_LINE_LEN)?DISP_LINE_LEN:nbytes;

		for (i=0; i<linebytes; i+= size) {
	  /*  	Long2HEX(long2hex, (*uip++ = *((ulong *)addr)));
			puts(long2hex);
			puts ("  ");*/
				printf(" %08x", (*uip++ = *((ulong *)addr)));

			addr += size;
		}

		puts ("   ");
		cp = linebuf;
		for (i=0; i<linebytes; i++) {
			if ((*cp < 0x20) || (*cp > 0x7e))
				putc ('.');
			else
				putc(*cp);
			cp++;
		}
 		printf("\n");
		nbytes -= linebytes;

	} while (nbytes > 0);

	return (rc);
}



/*
recieve length byte data from mem to nand flash
*/
void Mem2Nand_command(cmd_tbl_t *cmdtp, int flag,int argc,uchar* argv[]){
ulong addr,length;
uchar temp[4];
ulong *src_addr;
ulong *lptr;
uchar *bptr;
int pageleft,n_write;
int i;

	if (argc != 4) {puts("Invalid arg. e.g.#m2nand srcaddr flashaddr Length");putc(0x0a);return;}

    src_addr = (ulong *)simple_strtoul(argv[1],NULL,16);
	addr = simple_strtoul(argv[2],NULL,16);	
	length = simple_strtoul(argv[3],NULL,16);
	addr = addr&0xfffffffc; //min data with = 32bit

    Nand_Write_AS3310(nand_page_shift,addr,src_addr,length);

printf("Flash_Copy_DONE!\n");
}




U_BOOT_CMD(
	nande,   3,   1,  Erase_flash_cmd,
	"nande   - erase NAND FLASH memory\n",
	"start end\n"
	"    - erase FLASH from addr 'start' to addr 'end'\n"
	"erase bank N\n    - erase FLASH bank # N\n"
);

U_BOOT_CMD(
	nandr,   4,   1,  Read_flash_cmd,
	"nandr   - read NAND FLASH memory\n",
	"nandr startaddr length N\n"
);

U_BOOT_CMD(
	nandw,   4,   1,  Write_flash_cmd,
	"nandw   - write NAND FLASH memory\n",
	"nandw startaddr data N\n"
);

U_BOOT_CMD(
	m2nand,   4,   1,  Mem2Nand_command,
	"m2nand   - copy mem to NAND FLASH\n",
	"m2nand srcaddr flashaddr Length(all HEX) \n"
);





void Write_Nor_cmd(cmd_tbl_t *cmdtp, int flag,int argc,char* argv[])
{
	ulong addr;
	ushort data;		
	if (argc != 3) {puts("Invalid arg. e.g.#norwrite ADDR data");putc(0x0a);return;}	
	addr = simple_strtoul(argv[1],NULL,16);
	data = (ushort)simple_strtoul(argv[2],NULL,16);	
	if(Nor_Write_AS3310_single(addr,data))  printf("Write success.\n");
    else printf("Write error.\n");
}


void Erase_Nor_cmd(cmd_tbl_t *cmdtp, int flag,int argc,char* argv[])
{
	ulong addr,endaddr,length;	
	if ((argc > 3)||(argc < 2)) {puts("Invalid arg. e.g.#norerase address [length]");putc(0x0a);return;}	
	addr = simple_strtoul(argv[1],NULL,16);

    if (argc == 3) 	length = simple_strtoul(argv[2],NULL,16);
    else length = 1;

    endaddr = addr + length;
        while (addr < endaddr ) {
//        if (addr < 0x10000 ) {
            NOR_MBM29LV160TE_Erase(addr);
            flash_delay(1000000);
            addr += 0x2000;
//        }
 //       else{
 //           NOR_MBM29LV160TE_Erase(addr);
 //           flash_delay(1000000);
 //           addr += 0x10000;
 //       }
    }

/*    if(NOR_MBM29LV160TE_Erase(addr))  printf("Erase success.\n");
    else printf("Erase error.\n");
 */   
}


void flash_delay(unsigned long time){
    while(time-->0);
}


/*
recieve length byte data from mem to nand flash
*/
void Mem2Nor_command(cmd_tbl_t *cmdtp, int flag,int argc,uchar* argv[]){
ulong addr,length;
ulong *src_addr;
ulong erase_addr;
int errors;

	if (argc != 4) {puts("Invalid arg. e.g.#m2nor srcaddr flashaddr Length");putc(0x0a);return;}

    src_addr = (ulong *)((simple_strtoul(argv[1],NULL,16))&0xfffffffe);
	addr = simple_strtoul(argv[2],NULL,16);	
	length = simple_strtoul(argv[3],NULL,16);
	addr = addr&0xfffffffc; //min data with = 32bit

//    Erase_Nor_cmd(cmdtp,flag,3,argv+1);
/*
    erase_addr = addr;
    while (erase_addr < addr + length) {
        if (erase_addr < 0x10000 ) {
            NOR_MBM29LV160TE_Erase(erase_addr);
            flash_delay(100000);
            erase_addr += 0x1000;
        }
        else{
            NOR_MBM29LV160TE_Erase(erase_addr);
            erase_addr += 0x2000;
            flash_delay(100000);
        }
    }
   */ 
    errors = Nor_Write_AS3310(addr,src_addr,length);

printf("Flash_Copy_DONE! Total error:%d\n",errors);
}


int do_nandrspare(cmd_tbl_t *cmdtp,int flag,int argc,char* argv[])
{
	uchar buffer[NAND_READ_CMD_L];
	ulong addr;
	ulong length;
    volatile ulong *target_addr;

    
    if ((argc < 3)||(argc > 4)) {puts("Invalid args.");puts(cmdtp->usage);putc('\n');return 0;}  

    addr = simple_strtoul(argv[1],NULL,16);
    length = simple_strtoul(argv[2],NULL,16);

    if (argc == 4){
        target_addr = (volatile ulong *)simple_strtoul(argv[3],NULL,16);
    }

    if ((length > NAND_READ_CMD_L)&&(argc == 3)) length = NAND_READ_CMD_L;
    if (argc == 3) {
        target_addr = (volatile ulong *)buffer;
    }

    Nand_Read_Spare_AS3310(addr,(char *)target_addr);

    if (argc < 4) MemDisp_TRL(buffer,length,4);    
	return 0;	
}


/*
copy length byte data from mem to nand flash
*/
int do_m2nand_ecc(cmd_tbl_t *cmdtp,int flag,int argc,char* argv[]){
ulong addr,length;
char * src_addr;
int page_i;
int pages_write;
int i;
int sector;
char spare_buf[64];
char * spare_ptr;

	if (argc != 4) {puts("Invalid args.");puts(cmdtp->usage);putc('\n');return 0;}
    src_addr = (char *)simple_strtoul(argv[1],NULL,16);
	addr =  simple_strtoul(argv[2],NULL,16);	
	length =  simple_strtoul(argv[3],NULL,16);

    if (addr!= ((addr>>nand_page_shift)<<nand_page_shift) ) {
        puts("flash addr is not page aligined\n");
        return -1;
    }
    
    page_i = (addr>>nand_page_shift);
    pages_write = ((length - 1) >> nand_page_shift) + 1;
    spare_ptr = spare_buf;
    memset(spare_ptr,0xff ,64);
    for (i = 0; i<pages_write ; i++) {

        if ( nand_page_shift <= 9 ) {
            // small page
            EccEncode((ulong)src_addr,(ulong)(spare_ptr+4));
        }
        else{ // big page
            for (sector = 0; sector < 4 ; sector ++) {
                EccEncode((ulong)(src_addr + (sector<<9)),(ulong)(spare_ptr+(sector<<4)+4));
            }
        }
        Nand_Write_AS3310(nand_page_shift,addr,(ulong *)src_addr,(1<<nand_page_shift));
        Nand_Write_AS3310_Spare(addr,spare_ptr);
        src_addr += (1<<nand_page_shift);
        addr += (1<<nand_page_shift);
        putc('.');
    }

puts("Copy_DONE!\n");
return 0;
}


/*
copy length byte data from mem to nand flash spare
*/
int do_m2spare(cmd_tbl_t *cmdtp,int flag,int argc,char* argv[]){
ulong addr;
char * src_addr;

	if (argc != 3) {puts("Invalid args.");puts(cmdtp->usage);putc('\n');return 0;}

    src_addr = (char *)simple_strtoul(argv[1],NULL,16);
	addr = simple_strtoul(argv[2],NULL,16);;	

    Nand_Write_AS3310_Spare(addr,src_addr);

puts("Copy_DONE!\n");
return 0;
}


int do_EccEn(cmd_tbl_t *cmdtp,int flag,int argc, char *argv[]){
    ulong src_addr;
    ulong parity_addr;

    if (argc <3) {puts("Invalid args.");puts(cmdtp->usage);putc('\n');return 0;}
    src_addr = simple_strtoul(argv[1],NULL,16);
    parity_addr = simple_strtoul(argv[2],NULL,16);
    EccEncode(src_addr, parity_addr);

return 0;
}

int do_EccDeCh(cmd_tbl_t *cmdtp,int flag,int argc, char *argv[]){
    int * src_addr;
    int * parity_addr;
    int error_code;

    if (argc <3) {puts("Invalid args.");puts(cmdtp->usage);putc('\n');return 0;}
    src_addr = (int *)simple_strtoul(argv[1],NULL,16);
    parity_addr = (int *)simple_strtoul(argv[2],NULL,16);
    puts("ECC Code:0x");
    error_code = EccDecodeCheck(src_addr, parity_addr);
    printf("%x\n",error_code);

return 0;
}





U_BOOT_CMD(eccen,3,1, do_EccEn,
	"eccen   - do_EccEn src parity\n",
	"do_EccEn\n");

U_BOOT_CMD(eccde,3,1, do_EccDeCh,
	"eccde   - do_EccDe src parity\n",
	"do_EccDe\n");



U_BOOT_CMD(m2nandecc,   4,   1, do_m2nand_ecc,
	"m2nandecc   - copy mem to nand add ecc info\n",
    "m2nandecc srcaddr flashaddr[must page aligined] length\n"
	);

U_BOOT_CMD(m2spare,   4,   1, do_m2spare,
   "m2spare   - copy 64 bytes mem to nand spare\n",
   " #m2spare srcaddr flashaddr\n"
	);

U_BOOT_CMD(nandrspare,  4,   1, do_nandrspare,
    "nandrspare   - Read Nand Flash (Spare Space)\n",
     " #nandrspare Src_addr Length [Target_addr]\n"
         );


U_BOOT_CMD(
	norerase,   3,   1,  Erase_Nor_cmd,
	"norerase   - erase NOR FLASH memory\n",
	"erase addr N\n    - erase FLASH addr [length]# N\n"
);

U_BOOT_CMD(
	norwrite,   4,   1,  Write_Nor_cmd,
	"norwrite   - write NOR FLASH memory\n",
	"norwrite startaddr data N\n"
);


U_BOOT_CMD(
	m2nor,   4,   1,  Mem2Nor_command,
	"m2nor   - copy mem to NOR FLASH\n",
	"m2nor srcaddr flashaddr Length(all HEX) \n"
);

