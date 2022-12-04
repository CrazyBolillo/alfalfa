/*
 * File:   main.c
 * Author: Antonio
 *
 * Created on November 15, 2022, 9:39 AM
 */
// CONFIG1
#pragma config FOSC = INTOSC    // Oscillator Selection (INTOSC oscillator: I/O function on CLKIN pin)
#pragma config WDTE = OFF       // Watchdog Timer Enable (WDT disabled)
#pragma config PWRTE = OFF      // Power-up Timer Enable (PWRT disabled)
#pragma config MCLRE = ON       // MCLR Pin Function Select (MCLR/VPP pin function is MCLR)
#pragma config CP = OFF         // Flash Program Memory Code Protection (Program memory code protection is disabled)
#pragma config CPD = OFF        // Data Memory Code Protection (Data memory code protection is disabled)
#pragma config BOREN = OFF      // Brown-out Reset Enable (Brown-out Reset disabled)
#pragma config CLKOUTEN = OFF   // Clock Out Enable (CLKOUT function is disabled. I/O or oscillator function on the CLKOUT pin)
#pragma config IESO = OFF       // Internal/External Switchover (Internal/External Switchover mode is disabled)
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enable (Fail-Safe Clock Monitor is disabled)

// CONFIG2
#pragma config WRT = OFF        // Flash Memory Self-Write Protection (Write protection off)
#pragma config PLLEN = OFF       // PLL Enable (4x PLL enabled)
#pragma config STVREN = ON      // Stack Overflow/Underflow Reset Enable (Stack Overflow or Underflow will cause a Reset)
#pragma config BORV = LO        // Brown-out Reset Voltage Selection (Brown-out Reset Voltage (Vbor), low trip point selected.)
#pragma config LVP = OFF        // Low-Voltage Programming Enable (High-voltage on MCLR/VPP must be used for programming)

#include <xc.h>
#include "config.h"
#include "lcd.h"
#include "si5351.h"

const char FREQ_VALUES_STR[41][4] = {
    "660", "661", "662", "663", "664", "665", "666", "667", "668", "669",
    "670", "671", "672", "673", "674", "675", "676", "677", "678", "679",
    "680", "681", "682", "683", "684", "685", "686", "687", "688", "689",
    "690", "691", "692", "693", "694", "695", "696", "697", "698", "699",
    "700"
};

const uint8_t FREQ_VALUES_SI5351[41][16] = {
    {0x02, 0x71, 0, 0x0F, 0xE3, 0, 0x02, 0x4D, 0, 0x01, 0, 0x09, 0, 0, 0, 0},
    {0x30, 0xD4, 0, 0XF, 0xE4, 0, 0x02, 0xB0, 0, 0x01, 0, 0x9, 0, 0, 0, 0},
};

/**
 * Stores the UI state. De-bounce fields as the name implies are only used
 * for de-bouncing and have no effect. Action fields are the fields which
 * actually cause the UI to change when set.
 * 
 * Interrupts set de-bounce bits and the main loop de-bounces them and
 * updates the UI accordingly.
 */
struct {
    uint8_t debounce_freq_up:   1;
    uint8_t debounce_freq_down: 1;
    uint8_t debounce_freq_set:  1;
    uint8_t action_freq_up:     1;
    uint8_t action_freq_down:   1;
    uint8_t action_freq_set:    1;
    uint8_t i2c_error:          1;
    uint8_t output_enable:      1;
} ui_state;

uint8_t ui_freq;

void handle_clicks();
void si5351_setup();

void main(void) {
    
    ui_state.i2c_error = 1;
    
    /**
     * Clock configuration.
     * SPLLEN = 0 (PLL OFF)
     * IRCF = 1101 (4 MHz)
     * SCS = 00 (Select clock based on FOSC from CONFIG1)
     */
    OSCCON = 0x68;
     
    ANSELA = 0x00;
    LATA = 0x00;
    TRISA = 0x0F;
    
    ANSELC = 0x00;
    LATC = 0x00;
    TRISC = 0x03;
    
    /**
     * Interrupt configuration. 
     * Peripheral Interrupts Enabled
     * Interrupt On Change Enabled.
     * PORTA2:0 interrupts enabled on positive edge.
     * 
     * Once initialization is done GIE will be enabled.
     */
    INTCON = 0x48;
    IOCAP = 0x07;
    
    /**
     * I2C configuration.
     * MSSP enabled in I2C mode. 
     * Master mode and clock rate of 100 kHz.
     */
    SSP1CON1 = 0x28;
    SSP1CON2 = 0x00;
    SSP1STAT = 0x00;
    SSP1ADD  = 0x09;
    
    __delay_ms(10);
    lcd_init(0x0C);
    lcd_clear_display();
    lcd_write_string("CK0: ");
    lcd_move_cursor(0x40);
    lcd_write_string("SET: 40.");
    lcd_write_string(FREQ_VALUES_STR[ui_freq]);
    lcd_write_string(" MHz");    
    lcd_move_cursor(0x05);
    
    if ((ui_state.i2c_error = si5351_onbus()) == 0) {
        lcd_write_string("OFF");
        si5351_setup();
        INTCONbits.GIE = 1;
    }
    else {
        lcd_write_string("I2C ERROR");
    }
    
    while(1) {
        if (ui_state.i2c_error == 0) {
            handle_clicks();
        }
        else {
            if ((ui_state.i2c_error = si5351_onbus()) == 0) {
                lcd_move_cursor(0x05);
                lcd_write_string("OFF      ");
                si5351_setup();
                INTCONbits.GIE = 1;
            }
            else {
                __delay_ms(1000);
            }
        }
    }
    
    return;
}

void handle_clicks() {
    if (ui_state.debounce_freq_up == 1) {
        __delay_ms(DEBOUNCE_WAIT);
        if (UP_BTN == 1) {
            while (UP_BTN == 1);
            ui_state.action_freq_up = 1;
        }
        ui_state.debounce_freq_up = 0;
    }
    if (ui_state.debounce_freq_down == 1) {
        __delay_ms(DEBOUNCE_WAIT);
        if (DOWN_BTN == 1) {
            while (DOWN_BTN == 1);
            ui_state.action_freq_down = 1;
        }
        ui_state.debounce_freq_down = 0;
    }
    if (ui_state.debounce_freq_set == 1) {
        __delay_ms(DEBOUNCE_WAIT);
        if (SET_BTN == 1) {
            while (SET_BTN == 1);
            ui_state.action_freq_set = 1;
        }
        ui_state.debounce_freq_set = 0;
    }
    if ((ui_state.action_freq_up == 1) || (ui_state.action_freq_down == 1)) {
        if (ui_state.action_freq_up == 1) {
            if (ui_freq == 40)
                ui_freq = 0;
            else
                ui_freq++;

            ui_state.action_freq_up = 0;
        }
        if (ui_state.action_freq_down == 1) {
            if (ui_freq == 0)
                ui_freq = 40;
            else
                ui_freq--;

            ui_state.action_freq_down = 0;
        }

        lcd_move_cursor(0x48);
        lcd_write_string(FREQ_VALUES_STR[ui_freq]);
        lcd_write_string(" MHz");
    }
    if (ui_state.action_freq_set == 1) {
        lcd_move_cursor(0x5);
        lcd_write_string("40.");
        lcd_write_string(FREQ_VALUES_STR[ui_freq]);
        lcd_write_string(" MHz");
        
        si5351_freqset(FREQ_VALUES_SI5351[ui_freq]);

        if (ui_state.output_enable == 0) {
            si5351_write(0x10, 0x0C);
            si5351_write(0x03, 0xFE);
            ui_state.output_enable = 1;
        }
        
        ui_state.action_freq_set = 0;
    }
}

/**
 * Disable the output on all clocks and power them down. Setup CLK0 for its
 * eventual use.
 */
void si5351_setup() {
    si5351_write(0x03, 0xFF);
    si5351_write(0x10, 0x8C); // CLK0 uses MS0 and has 2mA drive strength
    si5351_write(0x11, 0x80);
    si5351_write(0x12, 0x80);
    si5351_write(0x95, 0x00); // Turn off Spread Spectrum
    
    /* 
     * VXCO Parameters. X on reset. Only god knows why Clock Builder
     * writes 0s to them. 
     */
    si5351_write(0xA2, 0x00);
    si5351_write(0xA3, 0x00);
    si5351_write(0xA4, 0x00);
}

void __interrupt() handle_int(void) {
    if (UP_BTN_INT == 1) {
        UP_BTN_INT = 0;
        ui_state.debounce_freq_up = 1;
    }
    if (DOWN_BTN_INT == 1) {
        DOWN_BTN_INT = 0;
        ui_state.debounce_freq_down = 1;
    }
    if (SET_BTN_INT == 1) {
        SET_BTN_INT = 0;
        ui_state.debounce_freq_set = 1;
    }
}
