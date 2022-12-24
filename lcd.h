#ifndef __LCD_GUARD__
#define __LCD_GUARD__

#include <stdint.h>

void lcd_send(uint8_t rs, uint8_t value);
void lcd_init(uint8_t config);
void lcd_display(uint8_t config);
void lcd_write_string(char *string);
void lcd_move_cursor(uint8_t address);
void lcd_clear_display(void);

#endif
