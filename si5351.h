#ifndef __I2C_GUARD__
#define	__I2C_GUARD__

#include <xc.h>

/** 
 * Defined as a 7 bit address.
 * Must be shifted before writing or reading.
 */
#define SI5351_ADDR 0x60

uint8_t si5351_write(uint8_t addr, uint8_t value);
uint8_t si5351_onbus();
void si5351_outoff();
void si5351_freqset(const uint8_t values[16]);

#endif

