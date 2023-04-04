typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned long u32;
typedef unsigned char uchar;

#define 	FALSE 			0
#define		TRUE 			1
#define 	BUFFSIZE 		2048
#define 	TTY_SET			1
#define 	TTY_RESTORE 	0
#define 	VERSION 		"1.2"

#define    	DEV				"/dev/ttyS0"
#define		SEND_OK			0xa5
#define		SEND_ERROR		0x0f

#define 	BOOTBAND        115200
#define 	BDRATE_DEFAULT	BOOTBAND	
#define 	BDRATE_BOOT		BOOTBAND

int			port_fd;

#define _UART_BOOT_AUTOMATIC_ 1

#define 	BOOT_INFO_HEADER	0	
#define 	FILE_WITH_SIZE		1
#define 	RAW_FILE            2


#define BOOT_INFO_MAGIC     0x544F4F42// BOOT
#define HEADER_SIZE     0x30
#define LOCATION_NAND   0
#define LOCATION_I2C    1
#define LOCATION_NOR    2
#define LOCATION_UART   3
#define LOCATION_USB    4
#define LOCATION_ROM    5


typedef struct boot_info_struct {
    u32     Magic;   //  0x544F4F42 BOOT
    u32     m_u32StartMode; // 0 == Nand , 1 = I2C  , 2 = NorFlash , 3 = UART , 4 = USB , 5 = ROM default = USB
    u32     src_addr;               // the address we got the code
    u32     dest_addr;              // dest address , where the app will run
    u32     n_bytes;                // code length to recloate
    u32     starting_sector;        // starting sector, for nand
    u32     starting_offset;        // entry offset
    int		(*run)(int, char *[]);  // main entry
    u32    Nand_table_location; // 0 == Nand , 1 = I2C  , 2 = NorFlash , 3 = UART , 4 = USB , 5 = ROM default = USB
    u32    Nand_table_offset;   // where to locate the nand_table
    u8     sdram_row;   
    u8     sdram_col; 
    u8     sdram_lagecy;    // 0x20 or 0x30
    u8     resv_8; 
    u32    resv_32;    
        }__attribute__((packed)) boot_info;

#define 	SEND_SIZE		0x10000

//#define 	BOOTBAND        38400
////#define 	BOOTBAND        115200
//#define     SDRAM_BOOT
//#define     SRAM_BOOT
//
//#ifdef SRAM_BOOT
//#define 	SEND_SIZE		0x6000
//#define 	BDRATE_DEFAULT	BOOTBAND	
//#define 	BDRATE_BOOT		BOOTBAND
//
//#else // SDRAM
//#define 	SEND_SIZE		0x18000
//#define 	BDRATE_DEFAULT	115200	
//#define 	BDRATE_BOOT		BOOTBAND
//
//#endif
//

