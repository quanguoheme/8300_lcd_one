/*
 * Copyright (C) 2010-2011 strong-lion, Inc.
 *
 * author: LiJianJun <lxyvslyr@yahoo.com.cn>
 *
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file COPYING in the main directory of this archive for
 * more details.
 *
 *
 */

#ifndef __VTM88560_H__
#define __VTM88560_H__

#include <linux/fb.h>

/* LCD interface:
	LCD_CS:	GPIOE[0]
	LCD_RES:GPIOE[1]

	LCD_CD:	GPIOE[2]
	LCD_SCK:GPIOE[3]   //maxium: 20M 
	LCD_SDA:GPIOE[4]

	LCD back light: GPIOE[10], (This pin also control keypad back light).
*/

#define BACKLIGHT_ON		1
#define BACKLIGHT_OFF		0


#define PAGE_NUM		8
#define COLUMN_NUM		132


#define BACKLIGHT_ON		1
#define BACKLIGHT_OFF		0
	

#define SELECT_LCD		0
#define DISELETT_LCD		1

#define LCD_CMD			0
#define LCD_DATA		1


// LCD module commands:

//segment direction, 0: normal , 1:  reverse
#define SetADCselect(x)   		( lcd_write_cmd( (0xa0 | ((x)&0x01))) )

#define SetColU(x)   			( lcd_write_cmd( (0x10 | (((x)>>4)&0x0f))) )
#define SetColL(x)   			( lcd_write_cmd( (0x00 | ((x)&0x0f))) )

//0x00: 2x,3x,4x . 0x01: 5x . 0x11: 6x	
#define SetbooterRatio(x)		do{ lcd_write_cmd(0xf8); ndelay(200);  lcd_write_cmd((x)&0x3); } while(0)

#define SetDispLine(x)			( lcd_write_cmd( (0x40 | ((x)&0x3f))) )
#define SetPage(x)   			( lcd_write_cmd( (0xb0 | ((x)&0x0f)) ) )

//the brightness of lcd
#define SetV0Rate(x)			( lcd_write_cmd( (0x20 | ((x)&0x07))) )
#define SetV0OutVolmue(x)   do{ lcd_write_cmd(0x81); ndelay(200); lcd_write_cmd((x)&0x3f); } while(0)

////0: off , 1: on	(double bytes command)	
#define SetStaticIndictor(x)   ( lcd_write_cmd( (0xac | ((x)&0x01)) ) )
#define SetStaticIndictorMod(x)   ( lcd_write_cmd( ((x)&0x03) ) )

  /*0010 1 VC VR VF*/
////0: off , 1: on		bit0:booster circuit	 , bit1:voltage regulator circuit	bit2:voltage follower circuit	
#define SetPowerCtl(x)		( lcd_write_cmd( (0x28 | ((x)&0x07)) ) )


////0: normal , 1: all on
#define SetAllOn(x)   			( lcd_write_cmd( (0xa4 | ((x)&0x01))) )
#define SetInverDis(x)   		( lcd_write_cmd( (0xa6 | ((x)&0x01))) )

////0: off , 1: on
#define SetDisEnable(x)   		( lcd_write_cmd( (0xae | ((x)&0x01))) )

#define LcdWriteAutoInc()  	( lcd_write_cmd( 0xe0) )
#define LcdWriteNoAutoInc() 	( lcd_write_cmd( 0xee) )

#define LcdReset()      		( lcd_write_cmd( 0xe2) )
#define LcdNOP()        		( lcd_write_cmd( 0xe3) )
//0: 1/9 bias , 1: 1/7 bias
#define SetBias(x)   			( lcd_write_cmd( (0xa2 | ((x)& 0x01))) )

//0: normal , 1:  reverse
#define SetComCtrl(x)			( lcd_write_cmd( (0xc0 | ((x<<3) & 0x08))) )


#define MIN_CONTRAST		63

/*
 * Function Prototypes
 */
//void lcd_write_data(int addr ,unsigned char* wbuffer);
//void lcd_init(struct fb_info *info);
//void lcd_read_data(int addr, unsigned char *rbuffer);
//void lcd_backlight(unsigned int status);
//void lcd_terminate(void);
void vtm88560_init(struct fb_info *info);

#endif // __VTM88560_H__

