#ifndef __CONF_GUARD__
#define __CONF_GUARD__

#include <xc.h>

#define _XTAL_FREQ 4000000
#define DEBOUNCE_WAIT 32

#define SET_BTN PORTAbits.RA2
#define DOWN_BTN PORTAbits.RA1
#define UP_BTN PORTAbits.RA0

#define SET_BTN_INT IOCAFbits.IOCAF2
#define DOWN_BTN_INT IOCAFbits.IOCAF1
#define UP_BTN_INT IOCAFbits.IOCAF0

#define LCD_RS LATAbits.LATA4
#define LCD_EN LATAbits.LATA5
#define LCD_DATA_MASK_ON 0x3C
#define LCD_DATA_MASK_OFF 0xC3

#endif