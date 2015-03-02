/*
 * linux/drivers/video/s3c2416fb.c
 *	Copyright (c) SINO (china) ,2006-2008
 
 *      author :Lijianjun<lxyvslyr@yahoo.com.cn>
 *
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file COPYING in the main directory of this archive for
 * more details.
 *
 *	    s3c2416 LCD Controller Frame Buffer Driver
 *
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/string.h>
#include <linux/mm.h>
#include <linux/tty.h>
#include <linux/slab.h>
#include <linux/delay.h>
#include <linux/fb.h>
#include <linux/init.h>
#include <linux/dma-mapping.h>
#include <linux/wait.h>
#include <linux/platform_device.h>
#include <linux/clk.h>
#include <linux/font.h>
#include <linux/proc_fs.h>

#include <asm/io.h>
#include <asm/uaccess.h>
#include <asm/div64.h>

#include <asm/mach/map.h>

#ifdef CONFIG_PM
#include <linux/pm.h>
#endif

#ifdef CONFIG_VTM88560
#include "vtm88560.h"
#endif

#include "bcm5892fb.h"

extern int pinpad_lock;
extern int pinpad_flush_lcd;

static struct s3c2416fb_mach_info *mach_info;

static void fb_init_timer(struct s3c2416fb_info *fbi);

struct s3c2416_lcd current_lcd;

/* Debugging stuff */
#ifdef CONFIG_FB_ZA9L_DEBUG
static int debug	   = 1;
#else
static int debug	   = 0;
#endif

#define dprintk(msg...)	if (debug) { printk(KERN_DEBUG "s3c2416fb: " msg); }


#ifdef CONFIG_VTM88560
static struct s3c2416fb_mach_info s3c2416_lcd_cfg __initdata = {

	.width		= 128,
	.height		= 65,

	.xres		= {
		.min	= 8,
		.max	= 128,
		.defval	= 128,
	},

	.yres		= {
		.min	= 8,
		.max	= 64,
		.defval = 64,
	},

	.bpp		= {
		.min	= 1,
		.max	= 1,
		.defval = 1,
	},
};
#endif

extern struct proc_dir_entry *stronglion_root;
static struct proc_dir_entry *fb_proc;

static char proc_buf[10];
static char boot_status[10]="bootting"; //bootting, completed

#if 0
static void s3c2416fb_set_lcdaddr(struct s3c2416fb_info *fbi)
{
	struct fb_var_screeninfo *var = &fbi->fb->var;

}
#endif

/*
 *	s3c2416fb_check_var():
 *	Get the video params out of 'var'. If a value doesn't fit, round it up,
 *	if it's too big, return -EINVAL.
 *
 */
static int s3c2416fb_check_var(struct fb_var_screeninfo *var,
			       struct fb_info *info)
{
	struct s3c2416fb_info *fbi = info->par;

//	printk("check_var(var=%p, info=%p)\n", var, info);

	/* validate x/y resolution */

	if (var->yres > fbi->mach_info->yres.max)
	{	
		var->yres = fbi->mach_info->yres.max;
		
	}	
	else if (var->yres < fbi->mach_info->yres.min)
		var->yres = fbi->mach_info->yres.min;

	if (var->xres > fbi->mach_info->xres.max)
		var->yres = fbi->mach_info->xres.max;
	else if (var->xres < fbi->mach_info->xres.min)
		var->xres = fbi->mach_info->xres.min;

	/* validate bpp */

	if (var->bits_per_pixel > fbi->mach_info->bpp.max)
		var->bits_per_pixel = fbi->mach_info->bpp.max;
	else if (var->bits_per_pixel < fbi->mach_info->bpp.min)
		var->bits_per_pixel = fbi->mach_info->bpp.min;

	/* set r/g/b positions *///16bit is special.
	if (var->bits_per_pixel == 16) {
		var->red.offset		= 11;
		var->green.offset	= 5;
		var->blue.offset	= 0;
		var->red.length		= 5;
		var->green.length	= 6;
		var->blue.length	= 5;
		var->transp.length	= 0;
	} else {
		var->red.length		= var->bits_per_pixel;
		var->red.offset		= 0;
		var->green.length	= var->bits_per_pixel;
		var->green.offset	= 0;
		var->blue.length	= var->bits_per_pixel;
		var->blue.offset	= 0;
		var->transp.length	= 0;
	}
	return 0;
}

/* s3c2416_activate_var
 *
 * activate (set) the controller from the given framebuffer
 * information
*/

static void s3c2416fb_activate_var(struct s3c2416fb_info *fbi,
				   struct fb_var_screeninfo *var)
{
#if 0
        __REGL(PMU_CKEN_REG) |= 0x1<<13;    //enable the lcd ctrl module
//	chip_active_var(fbi);               //initial the hardware ,accrod with the special lcd.
#endif

}

/*
 *      s3c2416fb_set_par - Optional function. Alters the hardware state.
 *      @info: frame buffer structure that represents a single frame buffer
 *
 */
static int s3c2416fb_set_par(struct fb_info *info)
{
	struct s3c2416fb_info *fbi = info->par;
	struct fb_var_screeninfo *var = &info->var;

//	printk("s3c2416fb_set_par\n");

	if (var->bits_per_pixel == 16)
		fbi->fb->fix.visual = FB_VISUAL_TRUECOLOR;
	else
		fbi->fb->fix.visual = FB_VISUAL_PSEUDOCOLOR;

	fbi->fb->fix.line_length     = (var->width*var->bits_per_pixel)/8;

	/* activate this new configuration */

	s3c2416fb_activate_var(fbi, var);
	return 0;
}

#if 0
static void schedule_palette_update(struct s3c2416fb_info *fbi,
				    unsigned int regno, unsigned int val)
{
	unsigned long flags;
	unsigned long irqen;

	local_irq_save(flags);

	fbi->palette_buffer[regno] = val;

	if (!fbi->palette_ready) {
		fbi->palette_ready = 1;

		/* enable IRQ */
	}

	local_irq_restore(flags);
}
#endif

/* from pxafb.c */
static inline unsigned int chan_to_field(unsigned int chan, struct fb_bitfield *bf)
{
	chan &= 0xffff;
	chan >>= 16 - bf->length;
	return chan << bf->offset;
}

static int s3c2416fb_setcolreg(unsigned regno,
			       unsigned red, unsigned green, unsigned blue,
			       unsigned transp, struct fb_info *info)
{
	struct s3c2416fb_info *fbi = info->par;
	unsigned int val;

	/* dprintk("setcol: regno=%d, rgb=%d,%d,%d\n", regno, red, green, blue); */
//	printk("s3c2416fb_setcolreg\n");

	switch (fbi->fb->fix.visual) {
	case FB_VISUAL_TRUECOLOR:
		/* true-colour, use pseuo-palette */

		if (regno < 16) {
			u32 *pal = fbi->fb->pseudo_palette;

			val  = chan_to_field(red,   &fbi->fb->var.red);
			val |= chan_to_field(green, &fbi->fb->var.green);
			val |= chan_to_field(blue,  &fbi->fb->var.blue);

			pal[regno] = val;
		}
		break;

	case FB_VISUAL_PSEUDOCOLOR:
		if (regno < 256) {
			/* currently assume RGB 5-6-5 mode */

			val  = ((red   >>  0) & 0xf800);
			val |= ((green >>  5) & 0x07e0);
			val |= ((blue  >> 11) & 0x001f);

#if 0			
			writel(val, S3C2410_TFTPAL(regno));
			schedule_palette_update(fbi, regno, val);
#endif			
		}

		break;

	default:
		return 1;   /* unknown type */
	}

	return 0;
}


/**
 *      s3c2416fb_blank
 *	@blank_mode: the blank mode we want.
 *	@info: frame buffer structure that represents a single frame buffer
 *
 *	Blank the screen if blank_mode != 0, else unblank. Return 0 if
 *	blanking succeeded, != 0 if un-/blanking failed due to e.g. a
 *	video mode which doesn't support it. Implements VESA suspend
 *	and powerdown modes on hardware that supports disabling hsync/vsync:
 *	blank_mode == 2: suspend vsync
 *	blank_mode == 3: suspend hsync
 *	blank_mode == 4: powerdown
 *
 *	Returns negative errno on error, or zero on success.
 *
 */
static int s3c2416fb_blank(int blank_mode, struct fb_info *info)
{
        struct s3c2416fb_info *fbi = info->par;
	int i;
	
//	printk("blank(mode=%d, info=%p)\n", blank_mode, info);
//	printk("s3c2416fb_blank\n");

	if (mach_info == NULL)
		return -EINVAL;

	if (blank_mode == FB_BLANK_UNBLANK)
           for (i=0; i<fbi->map_size; i++)
               *(fbi->lcd_buffer+i) = *(fbi->last_lcd_buffer+i) = 0xff;
	else 
           for (i=0; i<fbi->map_size; i++)
               *(fbi->lcd_buffer+i) = *(fbi->last_lcd_buffer+i) = 0x0;

	return 0;
}

static int s3c2416fb_debug_show(struct device *dev, struct device_attribute *attr, char *buf)
{
	return snprintf(buf, PAGE_SIZE, "%s\n", debug ? "on" : "off");
}

static int s3c2416fb_debug_store(struct device *dev, struct device_attribute *attr,
					   const char *buf, size_t len)
{
	if (mach_info == NULL)
		return -EINVAL;

	if (len < 1)
		return -EINVAL;

	if (strnicmp(buf, "on", 2) == 0 ||
	    strnicmp(buf, "1", 1) == 0) 
	{
		debug = 1;
		printk(KERN_DEBUG "s3c2416fb: Debug On");
	} 
	else if (strnicmp(buf, "off", 3) == 0 ||
		   strnicmp(buf, "0", 1) == 0) 
	{
		debug = 0;
		printk(KERN_DEBUG "s3c2416fb: Debug Off");
	} else 
	{
		return -EINVAL;
	}

	return len;
}


static DEVICE_ATTR(debug, 0666,
		   s3c2416fb_debug_show,
		   s3c2416fb_debug_store);

		   
//static unsigned long backlight_status = BACKLIGHT_OFF;

#ifdef CONFIG_VERIFY_EXE_FILE
extern int is_pinpad_proc(struct task_struct *the_proc);
#endif

static int s3c2416fb_ioctl(struct fb_info *info, unsigned int cmd,unsigned long arg)
{
      struct s3c2416fb_info *fbi = info->par;

      del_timer_sync(&fbi->timer.fb_timer);
      switch (cmd)
      {
            case FB_SET_BACKLIGHT:
	        if (current_lcd.lcd_driver->set_backlight)
		{
		    current_lcd.lcd_driver->set_backlight(info,arg); 
//		    backlight_status = arg;
		}    
	        break;
			
	    case FB_SET_CONTRAST:
	        if (current_lcd.lcd_driver->set_contrast)
		    current_lcd.lcd_driver->set_contrast(info,arg);    
	        break;
			
	   case FB_GET_SATAUS:
	        if (current_lcd.lcd_driver->get_status)
		    current_lcd.lcd_driver->get_status(info);
          	break;		

#ifdef CONFIG_VERIFY_EXE_FILE
	   case FB_PINPAD_FLUSH:
	   	if (is_pinpad_proc(current))
	   	{
      	       		if (current_lcd.lcd_driver->flush_fb)	
            	   	 	current_lcd.lcd_driver->flush_fb(info,0);		
	   	}	   
	   	break;
#endif		
			
	    default:
	        printk("s3c2416fb:no such command\n");
	        break;
      }
	  
      fb_init_timer(fbi);              //for next review
      
      return 0;
}
		   
		   		   
static struct fb_ops s3c2416fb_ops = {
	.owner		= THIS_MODULE,
	.fb_check_var	= s3c2416fb_check_var,
	.fb_set_par	= s3c2416fb_set_par,
	.fb_blank		= s3c2416fb_blank,
	.fb_ioctl       	= s3c2416fb_ioctl,
	.fb_setcolreg	= s3c2416fb_setcolreg,
	.fb_fillrect	= cfb_fillrect,
	.fb_copyarea	= cfb_copyarea,
	.fb_imageblit	= cfb_imageblit,
	
};


/*
 * s3c2416fb_map_video_memory():
 *	Allocates the DRAM memory for the frame buffer.  This buffer is
 *	remapped into a non-cached, non-buffered, memory region to
 *	allow palette and pixel writes to occur without flushing the
 *	cache.  Once this area is remapped, all virtual memory
 *	access to the video memory should occur at the new region.
 */
static int __init s3c2416fb_map_video_memory(struct s3c2416fb_info *fbi)
{
        int i,j;   

	dprintk("map_video_memory(fbi=%p)\n", fbi);

//	info->lcd_buffer = (u_char *)kmalloc(fbinfo->fix.smem_len,GFP_KERNEL);
	
	fbi->map_size = PAGE_ALIGN(fbi->fb->fix.smem_len + PAGE_SIZE);
	fbi->map_cpu  = dma_alloc_writecombine(fbi->dev, fbi->map_size,
					       &fbi->map_dma, GFP_KERNEL);

	fbi->map_size = fbi->fb->fix.smem_len;

	if (fbi->map_cpu) 
	{
		/* prevent initial garbage on screen */
		dprintk("map_video_memory: clear %p:%08x\n",
			fbi->map_cpu, fbi->map_size);
		memset(fbi->map_cpu, 0x00, fbi->map_size);

		fbi->screen_dma		= fbi->map_dma;
		fbi->fb->screen_base	= fbi->map_cpu;
		fbi->fb->fix.smem_start  = fbi->screen_dma;

		dprintk("map_video_memory: dma=%08x cpu=%p size=%08x\n",
			fbi->map_dma, fbi->map_cpu, fbi->fb->fix.smem_len);
			
		fbi->lcd_buffer = fbi->map_cpu;
		memset(fbi->lcd_buffer, 0 ,fbi->map_size );
#if 0   //test the fb
		
		for (i=0;i<8;i++)
   		{//{0x1c,0x3c,0x6c,0xcc,0xfe,0x0c,0x1e,0x00}; '4'  //fill with '4'
		 //0x18,0x38,0x18,0x18,0x18,0x18, 0x7e, 0x00,      /'1'
	
			*(fbi->lcd_buffer+i*128 +0 +0*16)= 0x1c; 
			*(fbi->lcd_buffer+i*128 +0 +1*16 )=0x3c;
			*(fbi->lcd_buffer+i*128 +0 +2*16 )=0x6c;
			*(fbi->lcd_buffer+i*128 +0 +3*16 )=0xcc;
			*(fbi->lcd_buffer+i*128 +0 +4*16 )=0xfe;
			*(fbi->lcd_buffer+i*128 +0 +5*16 )=0x0c;
			*(fbi->lcd_buffer+i*128 +0 +6*16 )=0x1e;
			*(fbi->lcd_buffer+i*128 +0 +7*16 )=0x00;	
		 
			for(j=1;j<15;j++)
			{

				*(fbi->lcd_buffer+i*128 +j +0*16)= 0xff; 
				*(fbi->lcd_buffer+i*128 +j +1*16 )=0xff;
				*(fbi->lcd_buffer+i*128 +j +2*16 )=0xff;
				*(fbi->lcd_buffer+i*128 +j +3*16 )=0xff;
				*(fbi->lcd_buffer+i*128 +j +4*16 )=0xff;
				*(fbi->lcd_buffer+i*128 +j +5*16 )=0xff;
				*(fbi->lcd_buffer+i*128 +j +6*16 )=0xff;
				*(fbi->lcd_buffer+i*128 +j +7*16 )=0xff;		
			}

			*(fbi->lcd_buffer+i*128 +15 +0*16)= 0x18; 
			*(fbi->lcd_buffer+i*128 +15 +1*16 )=0x38;
			*(fbi->lcd_buffer+i*128 +15 +2*16 )=0x18;
			*(fbi->lcd_buffer+i*128 +15 +3*16 )=0x18;
			*(fbi->lcd_buffer+i*128 +15 +4*16 )=0x18;
			*(fbi->lcd_buffer+i*128 +15 +5*16 )=0x18;
			*(fbi->lcd_buffer+i*128 +15 +6*16 )=0x7e;
			*(fbi->lcd_buffer+i*128 +15 +7*16 )=0x00;			
		 }  

		//all icon on;
		memset(fbi->lcd_buffer+1024, 0xff, 16);
#if 0		 
		 for(i=0;i<fbi->map_size;i++)
		 {
		     if ( i%16 ==0 )
		        printk("\n");
		     printk("%2x ",*(fbi->lcd_buffer+i));
		 }   
	 
		 printk("\n");	
#endif
#endif		
		fbi->last_lcd_buffer = (u_char*) kmalloc(fbi->map_size,GFP_KERNEL);	
		if (!fbi->last_lcd_buffer)
		{
		     printk("lcd:kmalloc last_lcd_buffer mem failed\n");
		     dma_free_writecombine(fbi->dev,fbi->map_size,fbi->map_cpu, fbi->map_dma);
		}  
		
		memset(fbi->last_lcd_buffer,0x00,fbi->map_size);
	}

	return fbi->map_cpu ? 0 : -ENOMEM;
}

static inline void s3c2416fb_unmap_video_memory(struct s3c2416fb_info *fbi)
{
	kfree(fbi->last_lcd_buffer);
	dma_free_writecombine(fbi->dev,fbi->map_size,fbi->map_cpu, fbi->map_dma);
}

static void s3c2416_flush_fb(struct s3c2416fb_info *fbi)
{   
      struct fb_info	   *fbinfo = fbi->fb; 

#ifdef CONFIG_VERIFY_EXE_FILE
//	if ((pinpad_lock) && !pinpad_flush_lcd)	//if LCD is locked by pinpad,don't flush it automatically
	if (pinpad_lock)
		goto out;
#endif
		  
      //no pixel change since last scan 
      if (memcmp(fbi->lcd_buffer, fbi->last_lcd_buffer,fbi->map_size) == 0 )  
         goto out;

//     printk("flush_fb--1\n");
      if (current_lcd.lcd_driver->flush_fb)	
            current_lcd.lcd_driver->flush_fb(fbinfo,0);

//	  pinpad_flush_lcd = 0;
	
out:
      fb_init_timer(fbi);              //for next review
}


static void fb_init_timer(struct s3c2416fb_info *fbi)
{
     init_timer(&fbi->timer.fb_timer);
     
     fbi->timer.fb_timer.data = (unsigned long)fbi ;
     
     fbi->timer.fb_timer.expires = jiffies + 5;     //every 50ms, we review and flush the fb buffer
     fbi->timer.fb_timer.function = (void *)s3c2416_flush_fb;
     
     add_timer(&fbi->timer.fb_timer);    
}

#if 0
static void s3c2416fb_write_palette(struct s3c2416fbfb_info *fbi)
{
	unsigned int i;
	unsigned long ent;

	fbi->palette_ready = 0;

	for (i = 0; i < 256; i++) {
		if ((ent = fbi->palette_buffer[i]) == PALETTE_BUFF_CLEAR)
			continue;

	}
}
#endif

#ifdef CONFIG_FACTORY_LOGO
static char logo_info[20]="waiting...";

#if 1
const unsigned char SclLogo[16*64] = {
#include "scl_logo.h"
}; 
#endif

extern const struct font_desc logo_font_vga_8x8;

struct timer_list Bar_timer;

#define MAX_BAR_BLOCK	15

static int bar_counter = 0;

static void init_barTimer(struct s3c2416fb_info *fbi);

static void next_bar(struct s3c2416fb_info *fbi)
{
	int k;
	char* lstart;
	char* cstart;	

	struct fb_info	   *fbinfo = fbi->fb;	
	int bytes_one_line = (fbinfo->var.xres * fbinfo->var.bits_per_pixel +7) / 8;

	lstart = fbi->lcd_buffer + 7*bytes_one_line*8;
	
	cstart =lstart + bar_counter ;
	for (k=0;k<8;k++)
	{
		*cstart = 0x7e;
		cstart += bytes_one_line;
	}
	
	init_barTimer(fbi);
	
	return;
}

static void init_barTimer(struct s3c2416fb_info *fbi)
{
	if ((bar_counter >= MAX_BAR_BLOCK) || (strcmp(boot_status, "completed") == 0))
	{
		bar_counter = 0;
		strcpy(boot_status, "completed");
		
		return ;
	}	
		
	bar_counter++;

	init_timer(&Bar_timer);
     
	Bar_timer.data = (unsigned long)fbi ;
     
	Bar_timer.expires = jiffies + 80;     //every 800ms, we add the block of process bar
	Bar_timer.function = (void *)next_bar;
     
	add_timer(&Bar_timer);
}

static void show_process_bar(struct s3c2416fb_info *fbi)
{
	int k;
	char* lstart;
	char* cstart;	

	struct fb_info	   *fbinfo = fbi->fb;	
	int bytes_one_line = (fbinfo->var.xres * fbinfo->var.bits_per_pixel +7) / 8;

	lstart = fbi->lcd_buffer + 7*bytes_one_line*8;
	
	cstart =lstart + bar_counter ;
	for (k=0;k<8;k++)
	{
		*cstart = 0x7e;
		cstart += bytes_one_line;
	}

	init_barTimer(fbi);
}

static void show_strong_lion_logo(struct s3c2416fb_info *fbi)
{
	int j,k;
	char* lstart;
	char* cstart;	

	struct fb_info	   *fbinfo = fbi->fb;	
	int bytes_one_line = (fbinfo->var.xres * fbinfo->var.bits_per_pixel +7) / 8;

	for (j=0;j<47;j++)
	{
		lstart = fbi->lcd_buffer + j*bytes_one_line;
		for(k=0;k<16;k++)
		{
	           	cstart =lstart + k;
			*cstart =  *(char *)(SclLogo+j*16+k);
		}
	}
}

static void show_info(struct s3c2416fb_info *fbi)
{
	int j,k;
	int len;
	char* lstart;
	char* cstart;
	
	struct fb_info	   *fbinfo = fbi->fb;

	int bytes_one_line = (fbinfo->var.xres * fbinfo->var.bits_per_pixel +7) / 8;
	
	if (fbi->lcd_buffer)
	{	
		lstart = fbi->lcd_buffer + 6*bytes_one_line*8;
		len =strlen(logo_info);
		for (j=0;j<len;j++)
		{
			char c = logo_info[j];
	           	cstart =lstart + j;
	
			for (k=0;k<8;k++)
			{
				*cstart = *(char *)(logo_font_vga_8x8.data + c*8 + k);
				cstart += bytes_one_line;
			}
		}
	}
	
	return ;
}

static void welcome(struct s3c2416fb_info *fbi)
{
	struct fb_info	   *fbinfo = fbi->fb;

	if (current_lcd.lcd_driver->set_backlight)
	{
		current_lcd.lcd_driver->set_backlight(fbinfo,BACKLIGHT_ON); //turn on the backlight 
//		backlight_status = BACKLIGHT_ON; 
	}	
		
	show_strong_lion_logo(fbi);
	show_info(fbi);
	show_process_bar(fbi);
	
	return ;
}
#endif

static char driver_name[]="s3c2416fb";

static int proc_read_fb(char *page,char **start,off_t off, 
			int count,int *eof,void *data)
{
	int len;

       len = sprintf(page,"%s", boot_status);

	*eof = 1;
	
//	printk("len is %d\n",len);
	return len;  
}

static int proc_write_fb(struct file *file,const char *buffer,
                           unsigned long count, void *data)
{
	if (count > 10)
		return count;

	memset(fb_proc->data, 0, 10);

	if (copy_from_user(fb_proc->data,buffer,count))
		return -EFAULT;

//	printk("fb_proc->data :%s\n",fb_proc->data);
	
	if   ( (strlen(fb_proc->data)== strlen("completed")) && (strcmp("completed",fb_proc->data) == 0) )
	{
		strcpy(boot_status, "completed");
	}

	return count;
}   

static int __init s3c2416fb_probe(struct platform_device *pdev)
{
	struct s3c2416fb_info *info;
	struct fb_info	   *fbinfo;
	int ret;
	int i;
	printk("=============>come into s3c2416fb_probe<===================\n");				  
	fb_proc = create_proc_entry("boot_status",0666,stronglion_root);
	if (fb_proc == NULL)
	{
		ret = -ENOMEM;
		goto go_on;
	}
   
	fb_proc->data = &proc_buf;
	fb_proc->read_proc = &proc_read_fb;
	fb_proc->write_proc = &proc_write_fb;
//	fb_proc->owner = THIS_MODULE;  
//	fb_proc->proc_fops->owner= THIS_MODULE;  

go_on:
	
	mach_info = (struct s3c2416fb_mach_info *)kmalloc(sizeof(struct s3c2416fb_mach_info), GFP_KERNEL);
	if (mach_info) 
	{
	    memcpy(mach_info, &s3c2416_lcd_cfg, sizeof(struct s3c2416fb_mach_info));
	    pdev->dev.platform_data = mach_info;
	}
	else
	{
	    printk("kmalloc for mach info failed\n");
	}
#if 0	
	mach_info = pdev->dev.platform_data;
	if (mach_info == NULL) {
		dev_err(&pdev->dev,"no platform data for lcd, cannot attach\n");
		return -EINVAL;
	}

	mregs = &mach_info->regs;
#endif
	fbinfo = framebuffer_alloc(sizeof(struct s3c2416fb_info), &pdev->dev);
	if (!fbinfo) 
	   return -ENOMEM;

	info = fbinfo->par;
	info->fb = fbinfo;
	info->mach_info = mach_info;
	
	platform_set_drvdata(pdev, fbinfo);

	dprintk("devinit\n");

	strcpy(fbinfo->fix.id, driver_name);

//	memcpy(&info->regs, &mach_info->regs, sizeof(info->regs));
//
//	info->mach_info		    = pdev->dev.platform_data;

//	fbinfo->fix.type	    = FB_TYPE_TEXT;
//	fbinfo->fix.type	    = FB_TYPE_PLANES;
	fbinfo->fix.type	    = FB_TYPE_PACKED_PIXELS;
	fbinfo->fix.type_aux	    = 0;
	fbinfo->fix.xpanstep	    = 0;
	fbinfo->fix.ypanstep	    = 0;
	fbinfo->fix.ywrapstep	    = 0;
	fbinfo->fix.accel	    = FB_ACCEL_NONE;

	fbinfo->var.nonstd	    = 0;
	fbinfo->var.activate	    = FB_ACTIVATE_NOW;
	fbinfo->var.height	    = mach_info->height;
	fbinfo->var.width	    = mach_info->width;
	fbinfo->var.accel_flags     = 0;
	fbinfo->var.vmode	    = FB_VMODE_NONINTERLACED;

	fbinfo->fbops		    = &s3c2416fb_ops;
	fbinfo->flags		    = FBINFO_FLAG_DEFAULT;
	fbinfo->pseudo_palette      = &info->pseudo_pal;

	fbinfo->var.xres	    = mach_info->xres.defval;
	fbinfo->var.xres_virtual    = mach_info->xres.defval;
	fbinfo->var.yres	    = mach_info->yres.defval;
	fbinfo->var.yres_virtual     = mach_info->yres.defval;
	fbinfo->var.bits_per_pixel  = mach_info->bpp.defval;

	fbinfo->var.upper_margin   = 0;
	fbinfo->var.lower_margin   = 0;
	fbinfo->var.vsync_len	    = 0;

	fbinfo->var.left_margin	    = 0;
	fbinfo->var.right_margin    = 0;
	fbinfo->var.hsync_len	    = 0;

	fbinfo->var.red.offset      = 11;
	fbinfo->var.green.offset    = 5;
	fbinfo->var.blue.offset     = 0;
	fbinfo->var.transp.offset   = 0;
	fbinfo->var.red.length      = 5;
	fbinfo->var.green.length    = 6;
	fbinfo->var.blue.length     = 5;
	fbinfo->var.transp.length   = 0;
	//for text lcd;
//	fbinfo->fix.smem_len = mach_info->xres.max *mach_info->yres.max;
	fbinfo->fix.line_length     = (fbinfo->var.width*fbinfo->var.bits_per_pixel)/8;
#if 1	
        //for graphic lcd;
	fbinfo->fix.smem_len        =	mach_info->xres.max *
					mach_info->yres.max *
					mach_info->bpp.max / 8 + 16;
#endif

//	printk("fbinfo->fix.smem_len :%d\n",fbinfo->fix.smem_len);	
	for (i = 0; i < 256; i++)
		info->palette_buffer[i] = PALETTE_BUFF_CLEAR;

	dprintk("got LCD region\n");	
	
	/* Initialize video memory */
	ret = s3c2416fb_map_video_memory(info);
	if (ret) 
	{
		dprintk( KERN_ERR "Failed to allocate video RAM: %d\n", ret);
		ret = -ENOMEM;
		goto dealloc_fb;
	}
	dprintk("got video memory\n");
	
	ret = s3c2416fb_check_var(&fbinfo->var, fbinfo);
	ret = register_framebuffer(fbinfo);
	
	if (ret < 0) {
		dprintk(KERN_ERR "Failed to register framebuffer device: %d\n", ret);
		goto free_video_memory;
	}

	/* create device files */
	device_create_file(&pdev->dev, &dev_attr_debug);
	printk(KERN_INFO "fb%d: %s frame buffer device\n",fbinfo->node, fbinfo->fix.id);
	
	current_lcd.lcd_driver = kmalloc(sizeof(struct s3c2416_lcd_driver),GFP_KERNEL);
//	current_lcd.lcd_device = kmalloc(sizeof(struct s3c2416_lcd_device),GFP_KERNEL);
	info->lcd_dev = &current_lcd;


#ifdef CONFIG_VTM88560
        vtm88560_init(fbinfo);
#endif 

	if (current_lcd.lcd_driver->set_contrast)	
            current_lcd.lcd_driver->set_contrast(fbinfo,55); //set the constrast 55 
	    
	if (current_lcd.lcd_driver->set_backlight)	
		current_lcd.lcd_driver->set_backlight(fbinfo,BACKLIGHT_ON); //turn off the backlight      

#ifdef CONFIG_FACTORY_LOGO	
	welcome(info);    
#endif	
	    
	fb_init_timer(info);                                  //start fb flush
	return 0;

free_video_memory:
	s3c2416fb_unmap_video_memory(info);
	
dealloc_fb:
	framebuffer_release(fbinfo);
	kfree(mach_info);
	
	return ret;
}

/* s3c2416fb_stop_lcd
 *
 * shutdown the lcd controller
*/

static void s3c2416fb_stop(struct s3c2416fb_info *info)
{	
	del_timer(&info->timer.fb_timer);
	
	if (current_lcd.lcd_driver->sleep_down)
		current_lcd.lcd_driver->sleep_down();
}

/*
 *  Cleanup
 */
static int s3c2416fb_remove(struct platform_device *pdev)
{
	struct fb_info	   *fbinfo = platform_get_drvdata(pdev);
	struct s3c2416fb_info *info = fbinfo->par;

	s3c2416fb_stop(info);
	msleep(1);

	s3c2416fb_unmap_video_memory(info);

	unregister_framebuffer(fbinfo);
       kfree(mach_info);
	
	return 0;
}

#ifdef CONFIG_PM

/* suspend and resume support for the lcd controller */

static int s3c2416fb_suspend(struct platform_device *dev, pm_message_t state)
{
	struct fb_info	   *fbinfo = platform_get_drvdata(dev);
	struct s3c2416fb_info *info = fbinfo->par;
#if 1
	del_timer_sync(&info->timer.fb_timer);
	
	/* sleep before disabling the clock, we need to ensure
	 * the LCD DMA engine is not going to get back on the bus
	 * before the clock goes off again (bjd) */

	if (current_lcd.lcd_driver->sleep_down)
		current_lcd.lcd_driver->sleep_down(); 

#endif

	return 0;
}

static int s3c2416fb_resume(struct platform_device *dev)
{
	struct fb_info	   *fbinfo = platform_get_drvdata(dev);
	struct s3c2416fb_info *info = fbinfo->par;
	int i;
#if 1

	if (current_lcd.lcd_driver->fbwake_up)
		current_lcd.lcd_driver->fbwake_up(); 

      if (current_lcd.lcd_driver->flush_fb)
      	{
 //    		memset(info->last_lcd_buffer, 0, info->map_size);
      		current_lcd.lcd_driver->flush_fb(fbinfo,1);	
      	}		
	
	fb_init_timer(info);
#endif
	
	return 0;
}

#else
#define s3c2416fb_suspend NULL
#define s3c2416fb_resume  NULL
#endif

static struct platform_driver s3c2416fb_driver = {
	.probe		= s3c2416fb_probe,
	.remove		= s3c2416fb_remove,
	.suspend	= s3c2416fb_suspend,
	.resume		= s3c2416fb_resume,
	.driver		= {
		.name	= "s3c2416-extern_lcd",
		.owner	= THIS_MODULE,
	},
};

int __devinit s3c2416fb_init(void)
{
	printk("=======================>2416<=====================\n");
	return platform_driver_register(&s3c2416fb_driver);
}

static void __exit s3c2416fb_cleanup(void)
{
	platform_driver_unregister(&s3c2416fb_driver);
}


module_init(s3c2416fb_init);
module_exit(s3c2416fb_cleanup);

MODULE_AUTHOR("lxyvslyr@yahoo.com.cn");
MODULE_DESCRIPTION("Framebuffer driver for the s3c2416");
MODULE_LICENSE("GPL");

