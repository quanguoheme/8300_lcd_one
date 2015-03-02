/*
 * linux/drivers/s3c2416fb.h
 * 
 *
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file COPYING in the main directory of this archive for
 * more details.
 *
 *	  S3C2416 LCD Frame Buffer Driver. don't use interal LCD controller
 *
 * Copyright (C) 2010-2011 strong lion, Inc.
 *
 * author: LiJianJun <lxyvslyr@yahoo.com.cn>
 */

#ifndef __S3C2416_FB_H__
#define  __S3C2416_FB_H__

#include <linux/timer.h>

struct s3c2416fb_val {
	unsigned int	defval;
	unsigned int	min;
	unsigned int	max;
};

struct s3c2416fb_timer{
//use timer0 as the fb timer
	struct timer_list fb_timer;
        int flush_interval;              //every flush_interval ,s3c2416 flush the fb.
};

struct s3c2416fb_mach_info {
	unsigned char	fixed_syncs;	/* do not update sync/border */

	/* Screen size */
	int		width;
	int		height;

	/* Screen info */
	struct s3c2416fb_val xres;
	struct s3c2416fb_val yres;
	struct s3c2416fb_val bpp;

	/* GPIOs */
	unsigned long	gpcup;
	unsigned long	gpcup_mask;
	unsigned long	gpccon;
	unsigned long	gpccon_mask;
	unsigned long	gpdup;
	unsigned long	gpdup_mask;
	unsigned long	gpdcon;
	unsigned long	gpdcon_mask;

};

struct s3c2416_lcd;

struct s3c2416fb_info {
	struct fb_info		*fb;
	struct device		*dev;
	struct s3c2416_lcd	*lcd_dev;
	struct clk		*clk;

	struct s3c2416fb_mach_info *mach_info;

	/* raw memory addresses */
	dma_addr_t		map_dma;	/* physical */
	u_char *			map_cpu;	/* virtual */
	u_int			map_size;

	/* addresses of pieces placed in raw buffer */
	u_char *			screen_cpu;	/* virtual address of buffer */
	dma_addr_t		screen_dma;	/* physical address of buffer */
	unsigned int		palette_ready;

	/* keep these registers in case we need to re-write palette */
	u32			palette_buffer[256];
	u32			pseudo_pal[16];
	
	u_char *		lcd_buffer;        //buffer storeing the date send to lcd
	u_char *		last_lcd_buffer;   //the last frame lcd_buffer 
	struct s3c2416fb_timer     timer;
};

#define PALETTE_BUFF_CLEAR (0x80000000)	/* entry is clear/invalid */


typedef unsigned char BACKLIGHT_STATUS ;
#define BACKLIGHT_ON		1
#define BACKLIGHT_OFF		0


struct s3c2416_lcd_device {
};

struct s3c2416_lcd_driver {
//	int	(*init)(struct fb_info *fbi); 
	void	(*flush_fb)(struct fb_info *fbi,int forced);   
	    
	int       (*write)(int addr ,unsigned char* wbuffer); 
	int       (*read)(int addr ,unsigned char* rbuffer); 
	void	  (*sleep_down)(void);
	void	  (*fbwake_up)(void);
	void      (*set_backlight)(struct fb_info *fbi,BACKLIGHT_STATUS stause); 
	void      (*set_contrast)(struct fb_info *fbi,unsigned long contrast); 
	void      (*get_status)(struct fb_info *fbi);
};


struct s3c2416_lcd
{
//   struct s3c2416fb_info* fb_info;
   struct s3c2416_lcd_device *lcd_device;
   struct s3c2416_lcd_driver *lcd_driver;   
};

int s3c2416fb_init(void);

/*--------------------- ioctl() ---------------------------*/
#define S3C2416_FB_MAGIC		'V'

#define FB_GET_SATAUS		_IOR(S3C2416_FB_MAGIC, 1 , int)   //get the status of the lcd
#define FB_SET_BACKLIGHT	_IOW(S3C2416_FB_MAGIC, 2 , int)   //set the black light
#define FB_SET_CONTRAST		_IOW(S3C2416_FB_MAGIC, 3 , int)   //set contrast



#endif // __S3C2416_FB_H__
