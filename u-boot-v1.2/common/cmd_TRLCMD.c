/*
Test u-boot for S3C2410
By Hoffer 2006-04

Including command: 
runram,showflash

Including function: 
Nand_read_TRL();TextToLong_TRL();

*/


#include <common.h>
#include <command.h>
#include <s3c2410.h>

#define PAGE_BYTES 512
#define FLASH_ERASE_DELAY 10000

static inline void delay (unsigned long loops)
{
	__asm__ volatile ("1:\n"
	  "subs %0, %1, #1\n"
	  "bne 1b":"=r" (loops):"0" (loops));
}


void runram_ready(){
	printf ("Led Shining...\nJump now\n");
}
void already_jumped(){
	printf ("Already_jumped\n");
}

void Led_Off(){
S3C24X0_GPIO * const gpio = S3C24X0_GetBase_GPIO();
gpio->GPFCON = 0x55AA;  // GPF7-4 Output ; GPF3-0 EINT
gpio->GPFDAT = 0xFF;

}

extern void	run_ram(); // in ASM language in Start.S

int
do_copyflash (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
    unsigned long addr,addrf,length,block,i_block,length_left;
    unsigned char buffer[PAGE_BYTES];
    int error,i,n_copy,sector;
    unsigned char * ramptr;

//    int show_num_tmp;

    error = 0;
    printf ("Doing CopyFlash command...\n");

    addr = TextToLong_TRL(argv[1]);
    addrf = TextToLong_TRL(argv[2]) ;
       if ((addr == -1 )||(addrf == -1)) { printf("Invalid address \n"); return 0;
    }
    length = TextToLong_TRL(argv[3]);

    ramptr = (unsigned char *)addr;
    printf("Source Address is %d = %d\n",ramptr,addr); //for debug

    block=((length-1)/PAGE_BYTES)+1;// PAGE_BYTES byte per block/page
    length_left=length;

    if (length <= 0) {return 0;
    }

    printf("Ereasing...Sector :");
    sector = (addrf>>14);
    do{
         Nand_erase_TRL(sector);
         printf("%d ",sector);
         sector++;
         delay(FLASH_ERASE_DELAY);
    }while(sector <= ((addrf+length)>>14));

    printf("Writing...\n");

//    show_num_tmp = 20; // for debug
    for(i_block=0;i_block<block;i_block++) {

        if (length_left<=PAGE_BYTES) {
            n_copy=length_left;
        }
        else n_copy=PAGE_BYTES;

        for (i=0;i<n_copy;i++){	 // read loop
            buffer[i] = *ramptr;
//            if ((i>255)&&(i<275)) printf("%d\t",buffer[i]); //for debug
//            else show_num_tmp=-1;
            ramptr++;
        }
        Nand_write_TRL(addrf,buffer,n_copy); //write to flash
        addrf = addrf + n_copy;
        length_left=length_left-n_copy;
    }
//    printf("\n");

    printf("%d pages copied.  Total error:%d\n",i_block,error);
	return 0;
}

U_BOOT_CMD(
	copyflash,	CFG_MAXARGS,		1,	do_copyflash,
 	"copyflash - \nCopy (AGV3) bytes data from addr(AGV1) in Ram to Flash addr(AGV2)\n",
	NULL
);


int
do_writeflash (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
    
    unsigned long addr,length;
    unsigned char buffer[64];
    int i,error;

    printf ("Doing WriteFlash command...\n");

    addr = TextToLong_TRL(argv[1]);
    if (addr == -1 ) { printf("Invalid address \n"); return 0;
    }
    length = TextToLong_TRL(argv[2]) ;

    for (i=3;i<3+length;i++) {
        buffer[i]= TextToLong_TRL(argv[i]);
    }
    error = Nand_write_TRL(addr,buffer+3,length);
    printf("Write complete total %d errors.\n\n",error);
	return 0;
}

U_BOOT_CMD(
	writeflash,	CFG_MAXARGS,		1,	do_writeflash,
 	"writeflash - \nWrite (AGV2) test bytes data to Flash addr(AGV1)\n",
	NULL
);


int
do_eraseflash (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
    int sector;
    sector = 4096;
    printf ("Doing EraseFlash command...\n");
    if (argc !=2 ) { printf("Invalid argrument number\n"); return 0;
    }
    sector = TextToLong_TRL(argv[1]);

    if (Nand_erase_TRL(sector)==1){
            printf ("Sector %d Erase Complete.\n",sector);
    }
    else {
            printf ("Sector %d Erase Error.\n",sector);
    }
return 0;
}

U_BOOT_CMD(
	eraseflash,	CFG_MAXARGS,		1,	do_eraseflash,
 	"eraseflash - Erase Flash sector(AGV1)\n",
	NULL
);
/*

int
do_showflash (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
    unsigned long addr,length;
    unsigned char buffer[64];
    int i;

    printf ("Doing ShowFlash command...\n");
    if (argc !=3 ) { printf("Invalid argrument number\n"); return 0;
    }
    addr = TextToLong_TRL(argv[1]);
    length = TextToLong_TRL(argv[2]) ;

    printf ("Reading Flash from %d, read_length = %d\n",(int)addr,(int)length);

    if (length > 64 ) length = 64;
    Nand_read_TRL(addr,buffer,length);
    for (i=0;i<length;i++) {
        printf("%d\t",buffer[i]);
    }
    printf("\n\n");
	return 0;
}

U_BOOT_CMD(
	showflash,	CFG_MAXARGS,		1,	do_showflash,
 	"showflash - show ARGV2 Bytes data in flash start from ARGV1\n",
	NULL
);
*/
int
do_runram (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	printf ("Prepare Jumping...\n");
	run_ram();
	printf ("This message should not been seen.");
	return 0;
}

U_BOOT_CMD(
	runram,	1,		1,	do_runram,
 	"runram - run program located in 0x32000000\n",
	NULL
);


int
do_md_TRL (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
u_char buffer[32];
ulong addrf,length,length_left;
int n_print;

 if ((argc < 2 )||(argc > 3 )) { printf("Invalid argrument number\n"); return 0;
    }

    addrf = TextToLong_TRL(argv[1]);
    if (argc == 3) length = TextToLong_TRL(argv[2]);
    else length = 32;
    length_left = length;
	printf ("mem display from:%08x\n",addrf);

    while (length_left > 0) {
            if (length_left<=32) {
                n_print=length_left;
            }
            else n_print=32;
           Nand_read_TRL(addrf,buffer,n_print);
           MemDisp_TRL(buffer,n_print,4);
           length_left = length_left - n_print;
           addrf = addrf + n_print;
    }    
	return 0;
}

U_BOOT_CMD(
	mdtrl,	CFG_MAXARGS,		1,	do_md_TRL,
 	"mdtrl - mem display \n",
	NULL
);






// Lib Part by Hoffer --------2006-04---- for flash test

#define CMD_SEL_A		0x00
#define CMD_SEL_B		0x01
#define CMD_SEL_C		0x50

#define CMD_WRITE		0x80
#define CMD_WRITEEND	0x10

#define CMD_ERASE		0x60
#define CMD_ERASEEND	0xD0
#define CMD_READSTAT	0x70

#define CONF_PAGE_BYTES PAGE_BYTES

#define Nand_FLASH_CMD_ADDR	(*((volatile u16 *)0x4e000004))
#define Nand_FLASH_ADDR	(*(volatile u8 *)0x4e000008)
#define Nand_FLASH_STAT_ADDR	(*(volatile u16 *)0x4e000010)
#define Nand_FLASH_DATA_ADDR	(*(volatile u8 *)0x4e00000c)


void Nand_read_TRL(  unsigned long rd_start,unsigned char *buf,unsigned long length)
{
unsigned long temp,length_left;

int n_read;
int i_block,block;
int i ;

i=0;
block=((length-1)>>8)+1;
length_left=length;

for(i_block=0;i_block<block;i_block++){// main loop

    if ((rd_start&0x100)==0) Nand_FLASH_CMD_ADDR = 0x00;
    else Nand_FLASH_CMD_ADDR = 0x01;// start command

    Nand_FLASH_ADDR = (rd_start&0xff); // given address
    Nand_FLASH_ADDR = ((rd_start>>9)&0xff);
    Nand_FLASH_ADDR = ((rd_start>>17)&0xff);
    Nand_FLASH_ADDR = ((rd_start>>25)&0xff);

    do{
        temp = Nand_FLASH_STAT_ADDR;
    }
    while ((temp&0x01)==0);// status check

    if (length_left<=CONF_PAGE_BYTES) {
        n_read=length_left;
    }
    else n_read=CONF_PAGE_BYTES;

        for (i=0;i<n_read;i++){	 // read loop
            *buf = Nand_FLASH_DATA_ADDR;
            rd_start++;
            buf++;
        }
        length_left=length_left-CONF_PAGE_BYTES;
    }

}



int Nand_write_TRL( unsigned long wr_start,unsigned char *buf,unsigned long length){

unsigned long temp,length_left;

int n_write;
int i_block,block;
int i,error;
unsigned char check_byte;

//printf("write start addr = %d\n",wr_start); //debug

// blank check
Nand_read_TRL(wr_start,&check_byte,1);
if (check_byte != 255) {
     printf("Error:sector not erased.\n");return 1;
}



i=0;
error=0;
block=((length-1)/CONF_PAGE_BYTES)+1;
length_left=length;

for(i_block=0;i_block<block;i_block++){// main loop

    if (length_left<=CONF_PAGE_BYTES) {
        n_write=length_left;
    }
    else n_write=CONF_PAGE_BYTES;

    if ((wr_start&0x100)==0) Nand_FLASH_CMD_ADDR = CMD_WRITE;
    else Nand_FLASH_CMD_ADDR = CMD_WRITE+1;// start command

    Nand_FLASH_ADDR = (wr_start&0xff); // given address
    Nand_FLASH_ADDR = ((wr_start>>9)&0xff);
    Nand_FLASH_ADDR = ((wr_start>>17)&0xff);
    Nand_FLASH_ADDR = ((wr_start>>25)&0xff);

    for (i=0;i<n_write;i++){	// write loop
         Nand_FLASH_DATA_ADDR = *buf;
         wr_start++;
         buf++;
     }

     Nand_FLASH_CMD_ADDR = CMD_WRITEEND; // write end command

     length_left=length_left-CONF_PAGE_BYTES;

    do{
        temp = Nand_FLASH_STAT_ADDR;
    }
    while ((temp&0x01)==0);// status check

    temp = Nand_FLASH_DATA_ADDR;

    if ((temp&0x01)!=0) error++;

    }

return error;
}




int Nand_erase_TRL(int sector){

unsigned long temp;


    Nand_FLASH_CMD_ADDR = CMD_ERASE;

    Nand_FLASH_ADDR = ((sector<<5)&0xff);
    Nand_FLASH_ADDR = ((sector>>3)&0xff);
    Nand_FLASH_ADDR = ((sector>>11)&0xff);

    Nand_FLASH_CMD_ADDR = CMD_ERASEEND;

//printf("flash erase waiting...\n");

//    temp = Nand_FLASH_STAT_ADDR;
    do{
        temp = Nand_FLASH_STAT_ADDR;
    }
    while ((temp&0x01)==0);

//printf("flash erase wait complete...\n");

//    Nand_FLASH_CMD_ADDR = CMD_READSTAT;

    temp = Nand_FLASH_DATA_ADDR;

    if ((temp&0x01)==0) {return 1;
    }
    else return 0;

}




long TextToLong_TRL(char *S){
    int i,neg;
    long value;

    if (S[0]=='-') {neg = -1;
    S=S+1;
    }
    else neg = 1;

    if ((S[0]<'0')&&(S[0]>'9')) {
        return 0;
    }

    i=0;
    value=0;
    if ((S[0]=='0')&&((S[1]=='x')||(S[1]=='X'))) {
        // Hex
        i=2;
        while (1) {
            if ((S[i]>='0')&&(S[i]<='9')) {
                  value = (value<<4) +  S[i] - '0';
                  i++;
            }
            else if ((S[i]>='a')&&(S[i]<='f')) {
                  value = (value<<4) +  S[i] - 'a' + 10;
                  i++;
            }
            else if ((S[i]>='A')&&(S[i]<='F')) {
                  value = (value<<4) +  S[i] - 'A' + 10;
                  i++;
            }
            else return value*neg;
        }
    }
    else  if ((S[0]=='0')&&((S[1]=='o')||(S[1]=='O'))) {
        // Oct
        i=2;
        while (1) {
            if ((S[i]>='0')&&(S[i]<='7')) {
                  value = (value<<3) +  S[i] - '0';
                  i++;
            }
            else return value*neg;
        }
    }
    else {
        // Dec
        i=0;
        while (1) {
            if ((S[i]>='0')&&(S[i]<='9')) {
                  value = value*10 +  S[i] - '0';
                  i++;
            }
            else return value*neg;
        }
    }
}



#define DISP_LINE_LEN	16
int MemDisp_TRL(u_char *buf,int total_length,int size)
{
	ulong	addr, length;
	ulong	i, nbytes, linebytes;
	u_char	*cp;
//	int	size;
	int rc = 0;
    addr = (ulong)buf;
	/* We use the last specified parameters, unless new ones are
	 * entered.
	 */

    /*
	addr = dp_last_addr;
	size = dp_last_size;
	length = dp_last_length;

	if (argc < 2) {
		printf ("Usage:\n%s\n", cmdtp->usage);
		return 1;
	}
*/


	/* Print the lines.
	 *
	 * We buffer all read data, so we can make sure data is read only
	 * once, and all accesses are with the specified bus width.
	 */
	nbytes = total_length;
	do {
		char	linebuf[DISP_LINE_LEN];
		uint	*uip = (uint   *)linebuf;
		ushort	*usp = (ushort *)linebuf;
		u_char	*ucp = (u_char *)linebuf;

		printf("%08lx:", addr);
		linebytes = (nbytes>DISP_LINE_LEN)?DISP_LINE_LEN:nbytes;

		for (i=0; i<linebytes; i+= size) {
			if (size == 4) {
				printf(" %08x", (*uip++ = *((uint *)addr)));
			} else if (size == 2) {
				printf(" %04x", (*usp++ = *((ushort *)addr)));
			} else {
				printf(" %02x", (*ucp++ = *((u_char *)addr)));
			}
			addr += size;
		}

		puts ("    ");
		cp = linebuf;
		for (i=0; i<linebytes; i++) {
			if ((*cp < 0x20) || (*cp > 0x7e))
				putc ('.');
			else
				printf("%c", *cp);
			cp++;
		}
		putc ('\n');
		nbytes -= linebytes;
/*
		if (ctrlc()) {
			rc = 1;
			break;
		}
*/
	} while (nbytes > 0);
/*
	dp_last_addr = addr;
	dp_last_length = length;
	dp_last_size = size;
    */
	return (rc);
}

