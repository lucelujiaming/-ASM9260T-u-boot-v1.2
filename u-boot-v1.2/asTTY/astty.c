/********************************************************************************
* Copyright (c)  AlphaScale														
* All rights reserved.															
* File name: asTTY.c
* Description: a terminal for AlphaScale AS3310x serials CPU
* Version: 0.1
* Author: Sun Jinfei
* Date: 2006.5.28
* version: 0.2
* Date: 2006.5.28
**********************************************************************************/

#include     <stdio.h>      
#include     <stdlib.h>     
#include     <unistd.h>     
#include     <sys/types.h>  
#include     <sys/stat.h>   
#include     <fcntl.h>      
#include     <termios.h>    
#include     <errno.h> 
#include 	<signal.h>
#include 	"astty.h"

int speed_arr[] = {B500000, B230400,B115200, B38400, B19200, B9600, B4800, B2400, B1200, B300};
int name_arr[] = {500000, 230400, 115200, 38400,  19200,  9600,  4800,  2400,  1200,  300};
void set_speed(int fd, int speed)
{
	int   i;
	int   status;
	struct termios   Opt;
	tcgetattr(fd, &Opt);
	for ( i= 0;  i < sizeof(speed_arr) / sizeof(int);  i++)
	{
		if (speed == name_arr[i])
		{
			//int j = 0;
			tcflush(fd, TCIOFLUSH);
			cfsetispeed(&Opt, speed_arr[i]);
			cfsetospeed(&Opt, speed_arr[i]);
			//printf("i is %d\n", i);
			//printf("speed_arr[i] is %d\n",speed_arr[i]); 
			/*while (j<9)
			{
				printf("speed_arr[%d] is %d\n",j, speed_arr[j]);
				j++;
			}*/
			status = tcsetattr(fd, TCSAFLUSH, &Opt);
			if (status != 0)
				perror("tcsetattr fd1");
			return;
		}
		tcflush(fd,TCIOFLUSH);
	}
}



int set_Parity(int fd,int databits,int stopbits,int parity)
{
	struct termios options;
	if ( tcgetattr( fd,&options)  !=  0)
	{
		perror("SetupSerial 1");
		return(FALSE);
	}
	options.c_cflag &= ~CSIZE;
	switch (databits)
	{
	case 7:
		options.c_cflag |= CS7;
		break;
	case 8:
		options.c_cflag |= CS8;
		break;
	default:
		fprintf(stderr,"Unsupported data size\n");
		return(FALSE);
	}
	switch (parity)
	{
	case 'n':
	case 'N':
		options.c_cflag &= ~PARENB;   
		options.c_iflag &= ~INPCK;    
		break;
	case 'o':
	case 'O':
		options.c_cflag |= (PARODD | PARENB);  
		options.c_iflag |= INPCK;             
		break;
	case 'e':
	case 'E':
		options.c_cflag |= PARENB;     
		options.c_cflag &= ~PARODD;   
		options.c_iflag |= INPCK;		/* Disnable parity checking */
		break;
	case 'S':
	case 's':  /*as no parity*/
		options.c_cflag &= ~PARENB;
		options.c_cflag &= ~CSTOPB;
		break;
	default:
		fprintf(stderr,"Unsupported parity\n");
		return(FALSE);
	}

	switch (stopbits)
	{
	case 1:
		options.c_cflag &= ~CSTOPB;
		break;
	case 2:
		options.c_cflag |= CSTOPB;
		break;
	default:
		fprintf(stderr,"Unsupported stop bits\n");
		return(FALSE);
	}

	//printf("we are here\n");
	if (parity != 'n')
		options.c_iflag |= INPCK;
	options.c_cc[VTIME] = 5; // 15 seconds
	options.c_cc[VMIN] = 1;

	options.c_lflag  &= ~(ICANON | ECHO| ECHOE | ISIG);  /*Input*/
	options.c_oflag  &= ~OPOST;	  /*Output*/
	//options.c_lflag  |= ECHOK;


	tcflush(fd,TCIFLUSH); /* Update the options and do it NOW */
	if (tcsetattr(fd,TCSANOW,&options) != 0)
	{
		perror("SetupSerial 3");
		return(FALSE);
	}
	return(TRUE);
}

int OpenDev(char *Dev)
{
	int fd = open( Dev, O_RDWR| O_NOCTTY | O_NDELAY );		  //| O_NOCTTY | O_NDELAY
	if (-1 == fd)
	{
		perror("Can't Open Serial Port");
		return -1;
	}
	else
		return fd;

}

/* Check if there is IO pending. */
int check_io(int fd1, int fd2, int tmout, char* buf, int* buflen)
{
	int n = 0, i;
	struct timeval tv;
	fd_set fds;
	//extern int io_pending; /* wkeys.c */

	tv.tv_sec = tmout / 1000;
	tv.tv_usec = (tmout % 1000) * 1000L;

	i = fd1;
	if (fd2 > fd1) i = fd2;

	FD_ZERO(&fds);
	//if (fd1 >= 0) FD_SET(fd1, &fds);
	//else fd1 = 0;
	//if (fd2 >= 0) FD_SET(fd2, &fds);
	//else fd2 = 0;
	FD_SET(fd1, &fds);
	FD_SET(fd2, &fds);

	//printf("going to select\n");
	if (select(i+1, &fds, NULL, NULL, NULL) > 0)
	{
		//printf("select didn't return 0\n");
		n = 1 * (FD_ISSET(fd1, &fds) > 0) + 2 * (FD_ISSET(fd2, &fds) > 0);
	}
	//printf("select finished\n");
	// if (buf)
	//{
	//	i = 0;
	//	if ((n & 1) == 1)
	//	{
	//		// printf("going to read\n");
	//		i = read(fd1, buf, 127);
	//		// printf("\nnumber read: %d\n",i);
	//	}
	//	buf[i > 0 ? i : 0] = 0;
	//	if (buflen)	*buflen = i;
	//}

	//printf("n = %d\n",n);
	return(n);
}
char getKey(void)
{
	char c;
	read(0, &c, 1);
	return c;
}
//main loop
//only return when command is pressed.
// command is Ctrl-A + x
// x could be any letter ('a'-'z', 'A'-'Z')
// return value is x
char do_terminal(int port_fd)
{
	char buf[128];
	char c;
	char *ptr;
	int blen;
	int x;
	int i;
	int escapeDected = 0;
	while (1)
	{
		x = check_io(port_fd, 0, 5000, buf, &blen);

		//send data from serial port to screen
		if ((x & 1) == 1)
		{
			i = read(port_fd, buf, 127);
			buf[i > 0 ? i : 0] = 0;
			blen = i;

			ptr = buf;
			while (blen-- > 0)
			{
			   //printf("char puted: %x\n", *ptr);
				putchar(*ptr++);
			

			}
			//fflush(stdout);


		}

		/* Read from the keyboard and send to serial port. */
		if ((x & 2) == 2)
		{

			//read(0,buf,1);
			//write(port_fd, buf, 1);
			c = getKey();
			if (escapeDected)
			{
				escapeDected = 0;
				return c;
			}
			if (c == 1)//Ctrl-A detected
			{
				escapeDected = 1;
				continue;
			}
			//printf("c is %d\n", c);
			write(port_fd, &c, 1);
		}
	}
}
void usage(void)
{
	printf("usage: asTTY <bootCode> <file>\n");
	printf("commands: 	\n");
	printf("	Ctrl-A + b: send bootcode \n");
	printf("	Ctrl-A + i: send bootinfo \n");
	printf("	Ctrl-A + s: send file  \n");
	printf("	Ctrl-A + c: send file with check sum enable \n");
	printf("	Ctrl-A + q: quit			\n");
}

//send len bytes in buf to port_fd
void sendBuf(int port_fd, char *buf, int len)
{
	int n = 0;
	int count = 0;
	char *ptr = buf;
	struct timeval tv;
	fd_set fds;
	//extern int io_pending; /* wkeys.c */

	if (len == 0)
		return;

	tv.tv_sec = 10;
	tv.tv_usec = 0;



	FD_ZERO(&fds);
	FD_SET(port_fd, &fds);


	//printf("going to select\n");
	while (1)
	{

		int retval;
		if ((retval = select(port_fd+1, NULL, &fds, NULL, NULL)) > 0)

		{
			// printf("select didn't return 0\n");
			//n = 1 * (FD_ISSET(fd1, &fds) > 0) + 2 * (FD_ISSET(fd2, &fds) > 0);
			if ((n = write(port_fd, ptr, 1))>=0)
			{	//printf("%d,%d ",n, count);
				count += n;
				ptr   += n;
				if (count >= len)
				{
					return;
				}
			}
		}
	   // else printf("select  returns %d\n", retval);

	}
}

//send boot code
//operation: send the first 2KB Code, and then the whole file
// before sending the 2KB code, the baud rate will be set to 38400
// after sending the 2KB Code, the baud rate will be chage to 115200
void SendBC(char *file_name)
{
	int fd;
	struct stat statBuf;
	char buf[BUFFSIZE];
	int remain;
	int count;
	int n;

	close(port_fd);
	port_fd = OpenDev(DEV);
	set_speed(port_fd, BDRATE_BOOT);//change the baudrate to 38400
	printf("baud rate changed to %d\n", BDRATE_BOOT);
	printf("\x0a\x0dsending  boot code... ");

	if (-1 == (fd = open(file_name, O_RDONLY)))
	{
		printf("can not open file: %s\n",file_name);
		exit(1);
	}
	fstat(fd, &statBuf);

	//printf("file size is %d\n", statBuf.st_size);
	/*if (statBuf.st_size != 2048)
	{
		printf("wrong file size. must be 2048 bytes.\n");
		exit(1);
	}*/


	if (BUFFSIZE != read(fd, buf, BUFFSIZE))
	{
		printf("read file error!\n");
		exit(1);
	}

	remain = BUFFSIZE;

	sendBuf(port_fd, buf, BUFFSIZE);

	putc('#', stdout);
	putc('\n',stdout);

	close(port_fd);
	port_fd = OpenDev(DEV);
	set_speed(port_fd, BDRATE_DEFAULT);//change the baudrate to 115200
	printf("baud rate changed to %d\n", BDRATE_DEFAULT);


	printf("sending file: stage 1 finished.\r\n");

	count = 0;
	lseek(fd,0, SEEK_SET);

	while ((n = read(fd, buf, BUFFSIZE)) > 0)
	{
		sendBuf(port_fd, buf, n);
		putc('#', stdout);
		count += n;
	}
	while (count < SEND_SIZE)
	{
		sendBuf(port_fd ,buf, 1);
		//putc('#', stdout);
		count += 1;
	}


	printf("sending file: stage 2 finished.\r\n");
	close(fd);
	puts(" done.\x0a\x0d");

}


//send file
//operation: first, send the file length, sleep 1 second, the the file
void SendFile(int port_fd,char *file_name)
{
	int fd;
	struct stat statBuf;
	char buf[BUFFSIZE];
	int remain;
	int error_count = 0;
	int send_count = 0;
	int n;
	unsigned char checkSum;
	unsigned char send_result;

	fd_set fds;
    FD_ZERO(&fds);
	FD_SET(port_fd, &fds);

	printf("\x0a\x0dsending  file... ");

	if (-1 == (fd = open(file_name, O_RDONLY)))
	{
		printf("can not open file: %s\n",file_name);
		exit(1);
	}
	fstat(fd, &statBuf);

	printf("file size is %d(0x%x)\n", statBuf.st_size, statBuf.st_size);
	/*if (statBuf.st_size != 2048)
	{
		printf("wrong file size. must be 2048 bytes.\n");
		exit(1);
	}*/

	//we must round up the size to nearest 16 Bytes no less than the real size
	int roundedSize = statBuf.st_size;
	if (roundedSize & 0xf != 0)
	{
		roundedSize += 16;
	}
	printf("size to be sent is %d(0x%x)\n", roundedSize & 0xfffffff0, roundedSize & 0xfffffff0);
	buf[0] = (char) (roundedSize & 0xf0);
	buf[1] = (char) ((roundedSize >> 8) & 0xff);
	buf[2] = (char) ((roundedSize >> 16) & 0xff);
	buf[3] = (char) ((roundedSize >> 24) & 0xff);

	sendBuf(port_fd, buf, 4);//send file length

	sleep(1);

	
	while ((n = read(fd, buf, 16)) > 0)
	{
		int i;
		checkSum = 0;
       	if (n == 16)
		{
            for (i=0; i<16; i++)
            	checkSum += buf[i];
		}
		else if (n < 16)
		{
			printf("read buff < 16 bytes\n");
			for (i=n; i<16; i++)//if the buff is less than 16B, pad 0 behind it
				buf[i] = 0;
			for (i=0; i<16; i++)
            	checkSum += buf[i];
		}
		sendBuf(port_fd, buf, 16);
		sendBuf(port_fd, &checkSum, 1);//send checkSum
		select(port_fd + 1, &fds, NULL, NULL, NULL);
		if (1 != read(port_fd, &send_result, 1))
			printf("read error\n");

		while (send_result != SEND_OK)
		{
			printf("send error! send_result is: %x\n", send_result);

			error_count ++;
			sendBuf(port_fd, buf, 16);
			sendBuf(port_fd, &checkSum, 1);//send checkSum
			select(port_fd + 1, &fds, NULL, NULL, NULL);
			if (1 != read(port_fd, &send_result, 1))
				printf("read error\n");

		}

		printf("send OK! send_result is: %x\n", send_result);
        //sendBuf(port_fd, buf, n);
		putc('#', stdout);
		send_count += 16;
		//count += n;
	}


	//printf("sending file: stage 2 finished.\r\n");
	close(fd);
	printf(" done. sent: %d(0x%x)B. totol error: %d\n", send_count, send_count, error_count);

}


//send file
//operation: first, send the file length, sleep 1 second, the the file
void SendFile2(int port_fd,char *file_name,int send_type)
{
	int fd;
	struct stat statBuf;
	char buf[BUFFSIZE];
	int remain;
    boot_info * uart_boot_header;
	
	int send_count = 0;
	int n;
	unsigned char checkSum;
	unsigned char send_result;
	printf("\x0a\x0dsending  file... ");

	if (-1 == (fd = open(file_name, O_RDONLY)))
	{
		printf("can not open file: %s\n",file_name);
		exit(1);
	}
	fstat(fd, &statBuf);

	printf("file size is %d(0x%x)\n", statBuf.st_size, statBuf.st_size);
	
    if (send_type == FILE_WITH_SIZE) {// ctrl+a s

        buf[0] = (char) (statBuf.st_size & 0xff);
        buf[1] = (char) ((statBuf.st_size >> 8) & 0xff);
        buf[2] = (char) ((statBuf.st_size >> 16) & 0xff);
        buf[3] = (char) ((statBuf.st_size >> 24) & 0xff);

        sendBuf(port_fd, buf, 4);//send file length
        sleep(1);
    }

	if (send_type == BOOT_INFO_HEADER) {// ctrl+a i
        n = read(fd, buf, HEADER_SIZE );
        uart_boot_header = (boot_info *)buf;
        uart_boot_header->n_bytes = (((statBuf.st_size - HEADER_SIZE + 3 /* DWORD aligin */)>>2)<<2);
        sendBuf(port_fd, buf, n);
        putc('*', stdout);		
        sleep(1);
    }

   
	if ((send_type == RAW_FILE)||(send_type == FILE_WITH_SIZE)
#if _UART_BOOT_AUTOMATIC_
        ||(send_type == BOOT_INFO_HEADER)
#endif
        ) {// ctrl+a b,s
         while ((n = read(fd, buf, BUFFSIZE)) > 0) { 
             if (n<BUFFSIZE) { 
                 n = (((n + 3) >>2) <<2);
             }
         	sendBuf(port_fd, buf, n);        
             putc('#', stdout);	    	
         }
    }
	//printf("sending file: stage 2 finished.\r\n");
	close(fd);
	printf(" done.\n");

}


//set the tty attribute on PC in which this program is running
//@CMD: if TTY_SET, set the attribute
// 		if TTY_RESTORE, restore the attibute 
void ttySet(int CMD)
{
	static struct termios oldTermAttr;
	struct termios tty;
	switch (CMD)
	{
	case TTY_SET:
		tcgetattr(0, &tty);
		oldTermAttr = tty;//keep the original info

		//tty.c_oflag &= ~OPOST;
		tty.c_lflag &= ~(XCASE|ECHONL|NOFLSH);
		tty.c_lflag &= ~(ICANON | ISIG | ECHO);
		tty.c_iflag &= ~(ICRNL|INLCR);
		tty.c_cflag |= CREAD;
		tty.c_cc[VTIME] = 5;
		tty.c_cc[VMIN] = 1;
		tcsetattr(0, TCSADRAIN, &tty);
		break;
	case TTY_RESTORE:
		tcsetattr(0, TCSADRAIN, &oldTermAttr);
		break;
	}


}

int main(int argc, char **argv)
{
	//int port_fd;
	int nread;
	char buff[1];
	char c;
	char *dev = DEV;
	//setvbuf(stdin, 0, _IONBF, 0);

	// SendFile(STDOUT_FILENO, argv[2]);
	//return 0;
	printf("astty: terminale for as331x CPU. Version: %s\n", VERSION);

	if (argc != 3)
	{
		usage();
		exit(1);
	}

	port_fd = OpenDev(dev);

	if (port_fd>0)
		set_speed(port_fd,BDRATE_DEFAULT);
	else
	{
		printf("Can't Open Serial Port!\n");
		exit(0);
	}
	if (set_Parity(port_fd,8,1,'N')== FALSE)
	{
		printf("Set Parity Error\n");
		exit(1);
	}

	// SendFile(port_fd, argv[1]);//send the 2KB code
	//return;
	//signal(SIGINT, SIG_IGN);//ignore Ctrl-C
	ttySet(TTY_SET);
	//printf("ttyInit finished\n");
	setvbuf(stdout, 0, _IONBF, 0);//shutdown terminal line buffering
	while (1)
	{
		c = do_terminal(port_fd);
		switch (c)
		{
		case 'b'://send boot code file
			//SendBC(argv[1]);
			SendFile2(port_fd, argv[1],RAW_FILE);//chek sum disabled
			break;
        case 'i'://send boot info file
			SendFile2(port_fd, argv[1],BOOT_INFO_HEADER);
			break;
		case 'c':
			SendFile(port_fd, argv[2]);//check sum enabled
			break;
		case 's':
			SendFile2(port_fd, argv[2],FILE_WITH_SIZE);//chek sum disabled
			break;

		case 'q'://quit
			ttySet(TTY_RESTORE);
			close(port_fd);
			return(0);
			break;
		default:
			continue;

		}

		//printf("do_terminal returned %c\n", c);
	}


	close(port_fd);
	exit(0);
}


