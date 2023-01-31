/*
 * setup_aht10.h
 *
 * Created: 2022-01-24 10:54:13
 *  Author: cedric.cloutier
 */ 


#ifndef SETUP_AHT10_H_
#define SETUP_AHT10_H_

#define START_BIT			0x08
#define AHT10_ADD			0x38

#define ADDW_ACK			0x18
#define DATA_ACK			0x28

#define ADDR_ACK			0x40
#define READ_ACK			0x50
#define READ_NOACK			0x58

#define STATUS_AHT10		0x19

#define TRIGGER_MESUREMENT	0xAC
#define MEASURE_DATA0		0x33
#define MEASURE_DATA1		0

#define DELAI_I2C			5000
#define DELAI_TRIGGER_I2C	200

#define VRAI				1
#define FAUX				0

#endif /* SETUP_AHT10_H_ */

void setup_i2c(uint16_t F_scl);
void i2c_event(void);

ISR(TWI_vect);