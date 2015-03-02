
#ifndef _S3C2416_EXTERN_LCD_H_
#define _S3C2416_EXTERN_LCD_H_

#define BACKLIGHT_ON		1
#define BACKLIGHT_OFF		0

/*--------------------- ioctl() ---------------------------*/
#define S3C2416_FB_MAGIC		'V'

#define FB_GET_SATAUS		_IOR(S3C2416_FB_MAGIC, 1 , int)   //get the status of the lcd
#define FB_SET_BACKLIGHT	_IOW(S3C2416_FB_MAGIC, 2 , int)   //set the black light
#define FB_SET_CONTRAST	_IOW(S3C2416_FB_MAGIC, 3 , int)   //set contrast 

#define MAX_CONTRAST		63                             //the maxium contrast

#endif // _S3C2416_EXTERN_LCD_H_

