#include "si5351.h"

// Code can be improved. Time constraints forced me to do it. Sorry


/**
 * Signal an I2C start and write a slave's address to the I2C bus.
 * @param addr Slave's address in 8 bit mode (includes R/W bit.)
 */
static void i2c_start(uint8_t addr) {
    SSP1CON2bits.SEN = 1;
    while(PIR1bits.SSP1IF == 0);
    SSP1BUF = (addr);
    PIR1bits.SSP1IF = 0;
    while(PIR1bits.SSP1IF == 0);
}

/**
 * Write an 8 bit value into one of the SI53151's register and ends with
 * a stop condition.
 * @param addr Address of the register the value will be written to.
 * @param value 8 bit value to be written into the specified register.
 * @return Whether an ACK was received or not. 
 */
uint8_t si5351_write(uint8_t addr, uint8_t value) {
    INTCONbits.GIE = 0;
    
    i2c_start(SI5351_ADDR << 1);
    
    uint8_t ack = SSP1CON2bits.ACKSTAT;
    PIR1bits.SSP1IF = 0;
    if (ack == 1) {
        return ack;
    }
    
    SSP1BUF = addr;
    while (PIR1bits.SSP1IF == 0);
    ack = SSP1CON2bits.ACKSTAT;
    PIR1bits.SSP1IF = 0;
    if (ack == 1) {
        return ack;
    }
    
    
    SSP1BUF = value;
    while (PIR1bits.SSP1IF == 0);
    PIR1bits.SSP1IF = 0;
    if (ack == 1) {
        return ack;
    }
    
    SSP1CON2bits.PEN = 1;
    while(PIR1bits.SSP1IF == 0);
    PIR1bits.SSP1IF =  0;
    
    
    INTCONbits.GIE = 1;
    
    return ack;
}

/**
 * Verify an SI5351 is available on the I2C bus. 
 * @return 0 for an ACK, which means the device is on the I2C bus. Otherwise
 * 1 for a NACK, which means it most likely is not.
 */
uint8_t si5351_onbus() {
    INTCONbits.GIE = 0;
    
    i2c_start(SI5351_ADDR << 1);
    
    uint8_t result = SSP1CON2bits.ACKSTAT;
    PIR1bits.SSP1IF = 0;
    SSP1CON2bits.PEN = 1;
    while(PIR1bits.SSP1IF == 0);
    
    INTCONbits.GIE = 1;
    
    return result;
}

/**
 * Turn off all outputs(CLK0:3) and power them down to save power.
 */
void si5351_outoff() {
    si5351_write(0x03, 0xFF);
    si5351_write(0x10, 0x80);
    si5351_write(0x11, 0x80);
    si5351_write(0x12, 0x80);
}

void si5351_freqset(const uint8_t values[16]) {
    uint8_t addr = 0x1A;
    uint8_t count = 0;
    for(; count != 8; addr++, count++) {
        uint8_t val = values[count];
        si5351_write(addr, val);
    }
    addr = 0x2A;
    for(; count != 16; addr++, count++) {
        uint8_t val = values[count];
        si5351_write(addr, val);
    }
}