#include "i2c.h"

uint8_t si5351_onbus() {
    INTCONbits.GIE = 0;
    
    SSP1CON2bits.SEN = 1;
    while(PIR1bits.SSP1IF == 0);
    SSP1BUF = (SI5351_ADDR << 1);
    PIR1bits.SSP1IF = 0;
    while(PIR1bits.SSP1IF == 0);
    
    uint8_t result = SSP1CON2bits.ACKSTAT;
    PIR1bits.SSP1IF = 0;
    SSP1CON2bits.PEN = 1;
    while(PIR1bits.SSP1IF == 0);
    
    INTCONbits.GIE = 1;
    
    return result;
}
