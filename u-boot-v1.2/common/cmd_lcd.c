
#include <common.h>
#include <command.h>
#include <devices.h>


#define COL 240
#define ROW 320
#define PI 3.1415926535
#define M_PI 3.1415926535
#define M_PI_2 1.570796327
#define ERROR 0.1
#define LINE_WIDTH 0

#ifndef max
#  define 	max(a,b)	((a) > (b) ? (a) : (b))
#endif /* !max */
#ifndef min
#  define 	min(a,b)	((a) < (b) ? (a) : (b))
#endif /* !min */
#ifndef abs
#  define	abs(a)		((a) < 0 ? -(a) : (a))
#endif /* !abs */

typedef unsigned short LCDBIT ;
/*
float Cos(float x);
float Sin(float x);
*/


/*
float Sin(float x)
{
float x2;
while(x >  M_PI) x-= 2*M_PI;
while(x < -M_PI) x+= 2*M_PI;
if (x>=0){
	if(x>0.749*M_PI) x=	M_PI - x;
	else if(x>0.251*M_PI) return Cos(M_PI_2 - x);
}
else{
	if(x<-0.749*M_PI) x= -M_PI - x;
	else if(x<-0.251*M_PI) return -Cos(-M_PI_2 - x);
}
x2=x*x;
return x-x2*((float)1/6-x2/120);
}



float Cos(float x)
{
int negf=1;
float x2;
while(x >  M_PI) x-= 2*M_PI;
while(x < -M_PI) x+= 2*M_PI;
if (x>= 0){
	if(x>0.749*M_PI) {x=- M_PI + x;negf=-negf;}
	else if(x>0.249*M_PI) return Sin(M_PI_2 - x);
}
else{
	if(x<-0.749*M_PI) {x= M_PI + x;negf=-negf;}
	else if(x<-0.249*M_PI) return Sin(M_PI_2 - x);
}
x2=x*x;
return (1-x2*((float)1/2-x2/24))*negf;
}


float Tan(float x)
{
float cos;
cos=Cos(x);
if (cos==0) return 0;
else return Sin(x)/cos;
}
*/



int bzero(ulong * buffer)
{
long i;
volatile ulong * addr;
ulong length;

    addr = buffer;
    length = (ROW*COL*sizeof(LCDBIT))>>2;
    for (i=0;i<length;i++)
        *(addr++)=0;
    
}

void Draw_Line(long start_x,long start_y,long end_x,long end_y,LCDBIT FrameBuffer[][COL],LCDBIT *color){
char direction;
int step,i,j,m,l;
long start_endx,start_endy;
/*float k;*/

/*k=(float)(start_y - end_y)/(start_x-end_x);*/


start_endx = start_x-end_x;
if(start_endx == 0) start_endx=1;
start_endy = start_y-end_y;
if(start_endy == 0) start_endy=1;

if (abs(start_x-end_x)>abs(start_y - end_y)) {
	direction='x';
	if (start_x > end_x) step=-1;
	else step=1;
	for(i=start_x; (((i<=end_x)&&(i>=start_x))  ||  ((i>=end_x)&&(i<=start_x)));i=i+step){
			j = ((i-start_x)*(start_y - end_y)/start_endx)+start_y;
			for(l=-LINE_WIDTH;l<LINE_WIDTH+1;l++){
				if ((i+l<COL)&&(i+l >=0)) 
					for(m=-LINE_WIDTH;m<LINE_WIDTH+1;m++)
					if ((j+m<ROW)&&(j+m >=0)) FrameBuffer[j+m][i+l]=*color;
			}
  /*  *color++;	*/
	}
}

else {
	direction='y';
	if (start_y > end_y) step=-1;
	else step=1;
	for(j=start_y; (((j<=end_y)&&(j>=start_y))  ||  ((j>=end_y)&&(j<=start_y))) ;j=j+step){
			i = ((j-start_y)*(start_x-end_x)/start_endy)+start_x;
			for(l=-LINE_WIDTH;l<LINE_WIDTH+1;l++){
				if ((i+l<COL)&&(i+l >=0)) 
					for(m=-LINE_WIDTH;m<LINE_WIDTH+1;m++)
					if ((j+m<ROW)&&(j+m >=0)) FrameBuffer[j+m][i+l]=*color;
			}
		}	
  /*  *color++;*/
	}

}
/*
void Line_Cercle(int center_x,int center_y,int speed,LCDBIT FrameBuffer[][COL]){

int row,col;
unsigned int color;
int l,m;
float alpha,angle_step,k;
alpha=0;
angle_step = M_PI/speed;
while(getc()==0){

    color=32;

	if (alpha >= M_PI) alpha =0;
	alpha = alpha + angle_step;

	for (row=0;row<ROW;row++){
		k=Tan(alpha);
		col = (int)((row-center_y)/k+center_x);
			for(l=-LINE_WIDTH;l<LINE_WIDTH+1;l++){
				if ((col+l<COL)&&(col+l >=0)) 
					for(m=-LINE_WIDTH;m<LINE_WIDTH+1;m++)
					if ((row+m<ROW)&&(row+m >=0)) FrameBuffer[row+m][col+l]=color++;
			}
		}

	}
}
*/

/*
void lcd_flush(unsigned long * buffer){
int i,busy_count;
//unsigned long * buffer;
//buffer = (unsigned long *)(LCDIF_DATA_BASE_ADDR);
busy_count=0;
//printf("Show Pic at 0x%x\n",buffer);

    // VSYNC
    outl( 0x10000 ,HW_LCDIF_CTRL+8); // STOP LCD
    outl( 0x40000 ,HW_LCDIF_CTRL+8); // CLEAR DATA MODE
    outl( 0x10000 ,HW_LCDIF_CTRL+4); // RUN LCD
    
    outl(0x43004200,HW_LCDIF_DATA);
    while ((0x01000000 & inl(HW_LCDIF_CTRL)) ==0 ) { } 
    outl(0x45004400,HW_LCDIF_DATA); // first half frame
    
    outl( 0x10000 ,HW_LCDIF_CTRL+8); // STOP LCD
    outl( 0x40000 ,HW_LCDIF_CTRL+4); // SET DATA MODE
    outl(0xffff ,HW_LCDIF_CTRL+4); // SET Count 
    outl( 0x10000 ,HW_LCDIF_CTRL+4); // RUN LCD
        
        for (i =0 ; i< COL*ROW/2 ;i++) {
            while ((0x01000000 & inl(HW_LCDIF_CTRL)) ==0 ) { 
                busy_count++;
                if (busy_count > 0x100) {
    //                printf(".");
                    outl( 0x10000 ,HW_LCDIF_CTRL+8); // STOP LCD
                    outl(0xffff ,HW_LCDIF_CTRL+4); // SET Count 
                    outl( 0x10000 ,HW_LCDIF_CTRL+4); // RUN LCD
   
                    busy_count =0;                    
                    break;
                }
            }
            if (i%0x1000==0) {
                    outl( 0x10000 ,HW_LCDIF_CTRL+8); // STOP LCD
                    outl(0xffff ,HW_LCDIF_CTRL+4); // SET Count 
                    outl( 0x10000 ,HW_LCDIF_CTRL+4); // RUN LCD
            }
            outl(*(buffer++),HW_LCDIF_DATA);
        }


}
  */



void Move_Edge(int *Ax,int *Ay,int step){
		if (*Ax==0){
			if(*Ay >= ROW - step) {*Ay=ROW;*Ax=1;}
			else (*Ay)+=step;		
		}
		else if(*Ay==ROW){
			if(*Ax >= COL - step) {*Ax=COL;*Ay=ROW-1;}
			else (*Ax)+=step;			
		}
		else if(*Ax==COL){
			if(*Ay <= step) {*Ax=COL-1;*Ay=0;}
			else (*Ay)-=step;			
		}
		else if(*Ay==0){
			if(*Ax <= step) {*Ax=0;*Ay=1;}
			else (*Ax)-=step;			
		}
}


void change_color(LCDBIT *color,int *color_control){
if(*color_control ==0){
*color = *color - 1 + (1<<6) ;
    if (((*color) & 0x1f) <2) {
        *color_control = 1;
        *color = 0x07c0;
    }
}

else if(*color_control ==1){
*color = *color - (1<<6) + (1<<11);
    if ((((*color)>>6) & 0x1f) <2) {
        *color_control = 2;
        *color = 0xf800;
    }
}

else {
*color = *color - (1<<11) + 1;
    if (((*color)>>11) <2) {
        *color_control = 0;
        *color = 0x1f;
    }
}

}
/*
void Triangle(LCDBIT FrameBuffer[][COL],LCDBIT FrameBuffer2[][COL]){

LCDBIT **FrameBuffer_temp;
int pingpong;
int Ax,Ay,Bx,By,Cx,Cy;
int Ax0,Ay0,Bx0,By0,Cx0,Cy0;
uchar temp;
LCDBIT black,color,color0,color_control0;
int A_step=1;
int B_step=3;
int C_step=5;
int i;
int color_control;

	Ax=0,Ay=0,Bx=240,By=160,Cx=0,Cy=320;
	color=0x1f;
    color_control = 0;
    black=0;
    pingpong = 0;

while(1){

    if (pingpong == 0)     FrameBuffer_temp = FrameBuffer;
    else   FrameBuffer_temp = FrameBuffer2;

    bzero((ulong * )FrameBuffer_temp);

    temp = serial_getc();
    if (temp!=0) return;    

	Move_Edge(&Ax,&Ay,A_step);
	Move_Edge(&Bx,&By,B_step);
	Move_Edge(&Cx,&Cy,C_step);

    change_color(&color,&color_control);
//color = color + 3 + (2<<5) + (1<<11);
Ax0=Ax;Ay0=Ay;Bx0=Bx;By0=By;Cx0=Cx;Cy0=Cy;color0=color;color_control0=color_control;

	Draw_Line(Ax,Ay,Bx,By,FrameBuffer_temp,&color);
	Draw_Line(Bx,By,Cx,Cy,FrameBuffer_temp,&color);
	Draw_Line(Cx,Cy,Ax,Ay,FrameBuffer_temp,&color);

    change_color(&color,&color_control);
//color = color + 3 + (2<<5) + (1<<11);

for (i=0;i<5;i++) {
	Move_Edge(&Ax,&Ay,A_step);
	Move_Edge(&Bx,&By,B_step);
	Move_Edge(&Cx,&Cy,C_step);
}
    Draw_Line(Ax,Ay,Bx,By,FrameBuffer_temp,&color);
	Draw_Line(Bx,By,Cx,Cy,FrameBuffer_temp,&color);
	Draw_Line(Cx,Cy,Ax,Ay,FrameBuffer_temp,&color);

    change_color(&color,&color_control);
//color = color + 3 + (2<<5) + (1<<11);

for (i=0;i<5;i++) {
	Move_Edge(&Ax,&Ay,A_step);
	Move_Edge(&Bx,&By,B_step);
	Move_Edge(&Cx,&Cy,C_step);
}
    Draw_Line(Ax,Ay,Bx,By,FrameBuffer_temp,&color);
	Draw_Line(Bx,By,Cx,Cy,FrameBuffer_temp,&color);
	Draw_Line(Cx,Cy,Ax,Ay,FrameBuffer_temp,&color);

    while(IS_DMA_LCD_FRAME_FLUSH_COMPLETE()==0) ;
    DMA_LCD_FRAME_FLUSH(FrameBuffer_temp);
    pingpong = 1 - pingpong;

    Ax=Ax0;Ay=Ay0;Bx=Bx0;By=By0;Cx=Cx0;Cy=Cy0;color=color0;color_control=color_control0;


	}

}


void do_cmd_lcd(cmd_tbl_t * cmd, int flag, int argc, char *argv[]){
 //   volatile LCDBIT *FrameBuffer[COL];
    volatile ushort **FrameBuffer,**FrameBuffer2;
    ushort color;
    color = 0xffff;
    FrameBuffer = (volatile LCDBIT **)LCDIF_DATA_BASE_ADDR;
    FrameBuffer2 = (volatile LCDBIT **)LCDIF_DATA_BASE_ADDR2;


    if (!((argv[1][0]=='F')||(argv[1][0]=='f'))) 
{

        bzero((ulong * )FrameBuffer);
    if ((argv[1][0]=='L')||(argv[1][0]=='l')) {
            Draw_Line(0,160,240,160,FrameBuffer,&color);
    }else  if((argv[1][0]=='B')||(argv[1][0]=='b'))
        Draw_Line(120,0,120,320,FrameBuffer,&color);
    else if((argv[1][0]=='T')||(argv[1][0]=='t'))
     Triangle(FrameBuffer,FrameBuffer2);
}
//   else   DMA_LCD_FRAME_FLUSH(LCDIF_DATA_BASE_ADDR);

}


U_BOOT_CMD(
	lcd, 2, 1,	do_cmd_lcd,
	"lcd   - test lcd \n",
	"[ 'l','t' ]\n"
	"    - test lcd "
	" choose 'l' or 't' \n"
);*/
