#include <xc.h>

#include "config.h"
#include "lcd.h"

void lcd_send(uint8_t rs, uint8_t value) {
    LCD_RS = rs;
    LATC = ((LATC & LCD_DATA_MASK_OFF) | ((value >> 2) & LCD_DATA_MASK_ON));
    LCD_EN = 1;
    __delay_ms(2);
    LCD_EN = 0;
    LATC = ((LATC & LCD_DATA_MASK_OFF) | ((value << 2) & LCD_DATA_MASK_ON));
    LCD_EN = 1;
    __delay_ms(2);
    LCD_EN = 0;
}

/**
 * 
 * @param config
 */
void lcd_init(uint8_t config) {    
    lcd_send(0, 0x02);
    lcd_send(0, 0x28);
    lcd_send(0, config);
    lcd_send(0, 0x14);
    lcd_send(0, 0x01);
}

void lcd_write_string(char *string) {
    while(*string != '\0') {
        lcd_send(1, *string);
        string++;
    }
}

void lcd_move_cursor(uint8_t address) {
    lcd_send(0, (0x80 | address));
}

void lcd_clear_display(void) {
    lcd_send(0, 0x01);
}