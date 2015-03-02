/*
*    test for LCD
*     lijianjun
*/
#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <termio.h>
#include <unistd.h>
#include <string.h>
#include <sys/mman.h>

#include "fb.h"

/*------the dot array(10x8) of char '1' ------------*/
/*
 0 0 0 0 0 0 0 0    0x0
 0 0 0 0 1 0 0 0    0x8
 0 0 0 1 1 0 0 0    0x18
 0 0 0 0 1 0 0 0    0x8
 0 0 0 0 1 0 0 0    0x8
 0 0 0 0 1 0 0 0    0x8
 0 0 0 0 1 0 0 0    0x8
 0 0 0 0 1 0 0 0    0x8
 0 0 0 0 1 0 0 0    0x8
 0 0 0 0 0 0 0 0    0x0
*/
#if 0
	/* 50 0x32 '2' */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x7c, /* 01111100 */
	0xc6, /* 11000110 */
	0x06, /* 00000110 */
	0x0c, /* 00001100 */
	0x18, /* 00011000 */
	0x30, /* 00110000 */
	0x60, /* 01100000 */
	0xc0, /* 11000000 */
	0xc6, /* 11000110 */
	0xfe, /* 11111110 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */

	/* 56 0x38 '8' */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x7c, /* 01111100 */
	0xc6, /* 11000110 */
	0xc6, /* 11000110 */
	0xc6, /* 11000110 */
	0x7c, /* 01111100 */
	0xc6, /* 11000110 */
	0xc6, /* 11000110 */
	0xc6, /* 11000110 */
	0xc6, /* 11000110 */
	0x7c, /* 01111100 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */

#endif
	
int main(void)
{
	int   fb;
	char  *pfb;
	char  *start;
	char  show_dots_2[16]={0x00,0x00,0x7c,0xc6,0x06,0x0c,0x18,0x30,0x60,0xc0,0xc6,0xfe,0x00,0x00,0x00,0x00}; //the char '2'
	char  show_dots_8[16]={0x00,0x00,0x7c,0xc6,0xc6,0xc6,0x7c,0xc6,0xc6,0xc6,0xc6,0x7c,0x00,0x00,0x00,0x00}; //the char '8'
	int flag=0;
	int icon_offset = 1024;        // 128*64/8 =1024;
	int print_icon_offset = (42)/8;     //the message icon's offset from icon_offset
	int print_bit_icon_offset = 7-(42)%8; //the bit of message icon offset in the byte.
	
	char mes_off = ~(1<< print_icon_offset);
	char mes_on = (1<< print_icon_offset);
	
	int sig_icon_offset = (20)/8;   //the signal icon's offset
	int sig_bit_icon_offset = 7-(20)%8;
	
	char sig_off = ~(1<< sig_bit_icon_offset);
	char sig_on = (1<< sig_bit_icon_offset);
	
	int last_icon_offset = (122)/8;   //the last icon's offset
	int last_bit_icon_offset = 7-(122)%8;
	
	char last_off = ~(1<< last_bit_icon_offset);
	char last_on = (1<< last_bit_icon_offset);
	
	int i;
	int contrast = MAX_CONTRAST;
	
	fb = open("/dev/fb0", O_RDWR);
	if(fb < 0) 
	{
		printf("GAL fbcon engine Please check kernel config.\n");
		return -1;
	}
	
	pfb = (char*)mmap(0,4096,PROT_READ|PROT_WRITE,MAP_SHARED,fb,0);
	
	if (pfb<0)
	{
	    printf("map the video ram faield\n");
	    return -1;
	}
	
	//clear the screen
	memset(pfb,0,1024+16);
#if 0
	while(1)
	{
		//clear the screen
		memset(pfb,0,1024+16);
		
		sleep(1);
		
		//light the screen
		memset(pfb,0xff,1024+16);

		sleep(1);
	}
#endif	

	while(1)
	{
	    if (flag == 0)
	    {
/*----------------show char , 10*8 font----------------------*/
	//the lcd is 128*64(bit) , 
	
        //show the char '2' on  lcd	,the  postion (0 row, 15 col)
         
	           start = pfb + (0 * 16  + (15+1)/8 );
               for (i=0;i<16;i++)
	           {
	               *start = show_dots_2[i];
	               start += 16;
	           }   

                 //show the char '2' on  lcd	,the  postion (31 row, 39 col)
	           start = pfb + (31 * 16   + (39+1)/8); 
               for (i=0;i<16;i++)
	           {
	               *start = show_dots_2[i];
	               start += 16;
	           }
		   
		   ioctl(fb,FB_SET_BACKLIGHT,BACKLIGHT_ON);       //turn on backlight
		   //the message icon turn on
		   *(pfb + icon_offset + print_icon_offset) |= mes_on;
		   //the signal icon turn on;
		   *(pfb + icon_offset + sig_icon_offset) |= sig_on;
		   *(pfb + icon_offset + last_icon_offset) |= last_on;
		   
		   flag = 1;
 	      }	   
	      else 
	      {
	           start = pfb + (0 * 16  + (15+1)/8 );
                   for (i=0;i<16;i++)
	           {
	               *start = show_dots_8[i];
	               start += 16;
	           }   

                   //show the char '8' on  lcd	,the  postion (31 row, 39 col)
	           start = pfb + (31 * 16   + (39+1)/8); 
                   for (i=0;i<16;i++)
	           {
	               *start = show_dots_8[i];
	               start += 16;
	           }
		   
		   ioctl(fb,FB_SET_BACKLIGHT,BACKLIGHT_OFF);       //turn off backlight
		   //the message icon turn off
		   *(pfb + icon_offset + print_icon_offset) &= mes_off;
		    //the signal icon turn off; 
		   *(pfb + icon_offset + sig_icon_offset) &= sig_off;
		   *(pfb + icon_offset + last_icon_offset) &= last_off;
		   
		   flag = 0;       
	      }
	      
	      ioctl(fb,FB_SET_CONTRAST,contrast);
	      
	      contrast-- ;
	      if (contrast < 0)
	          contrast = MAX_CONTRAST;
	      
	      sleep(1);
        }
	
	return 0;
}


