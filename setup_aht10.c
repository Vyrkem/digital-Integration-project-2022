/*
 * setup_aht10.c
 *
 * Created: 2022-01-26 09:34:06
 *  Author: cedric.cloutier
 */ 
#include <avr/io.h>
#include <avr/interrupt.h>

#define F_CPU	16000000
#include <util/delay.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>


#include "setup_timer.h"
#include "rs232.h"
#include "lcd_4bits.h"
#include "setup_aht10.h"
#include "wifi.h"

/*
#define START_I2C	TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN) | (1 << TWIE)
#define STOP_I2C	TWCR = (1 << TWINT) | (1 << TWSTO) | (1 << TWEN) | (1 << TWIE)
#define SEND_I2C	TWCR = (1 << TWINT) | (1 << TWEA) | (1 << TWEN) | (1 << TWIE)
#define ACK_I2C		TWCR = (1 << TWINT) | (1 << TWEA) | (1<<TWIE)
#define NACK_I2C	TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWIE)
*/



uint8_t nouvelle_communication;
uint8_t trigger_communication;
uint8_t read_complete;
uint32_t old_time_i2c;

union aht_device{
		long long data64;
		struct{
			uint32_t temp: 20;
			uint32_t humide: 20;
			uint32_t status_aht: 8;
			uint32_t vide: 16;
		}donnee;
};
union aht_device capteur;

void setup_i2c(uint16_t F_scl){
	nouvelle_communication = VRAI;
	trigger_communication = FAUX;
	read_complete = FAUX;
	old_time_i2c = mills;
	
	uint8_t prescaler = 1;
	uint8_t compteur_prescaler = 0;
	
    DDRC &= ~((1<<DDC5)|(1<<DDC4));   //SDA SCL IN 
	PORTC |= (1<<PORTC5)|(1<<PORTC4); //Pullup

	while(!((((F_CPU/F_scl)-16)/2/prescaler)<256)){
		if(prescaler<64){
			prescaler*=4;	
		}
		if(compteur_prescaler<3){
			compteur_prescaler++;
		}
	}
	
	TWSR = compteur_prescaler;
	
	uint32_t calcul_bitrate = ((F_CPU/F_scl)-16)/2/prescaler;			
	TWBR = calcul_bitrate;										
	
	TWCR = (1<<TWEN)|(1<<TWIE);
	
}

void i2c_event(void){
	uint32_t new_time_i2c = mills;
	float temperature;
	float humidite;
	char temp_humi[40];
	
	if(nouvelle_communication)
	{
		if((new_time_i2c-old_time_i2c)>DELAI_I2C){
			old_time_i2c = mills;
			TWCR = (1<<TWINT)|(1<<TWSTA)|(1<<TWEN)|(1<<TWIE);
		}
	}
	else if(trigger_communication)
	{
		if((new_time_i2c-old_time_i2c)>DELAI_TRIGGER_I2C){
			old_time_i2c = mills;
			capteur.data64 = 0;
			TWCR = (1<<TWINT)|(1<<TWSTA)|(1<<TWEN)|(1<<TWIE);
		}
	}
	else if(read_complete)
	{
		humidite = capteur.donnee.humide/MEG*100;
		temperature = ((capteur.donnee.temp/MEG)*200)-50;
		
		sprintf(temp_humi,"UDP %s %d %.1f",IPNODERED,PORT5002,humidite);
		rs232_puts(temp_humi);

		sprintf(temp_humi,"UDP %s %d %.1f",IPNODERED,PORT5003,temperature);
		rs232_puts(temp_humi);
		
		sprintf(temp_humi,"%.1fC  %.1f%%Hr     ",temperature, humidite);
		lcd_puts(2,2,temp_humi);
		
		
		old_time_i2c = mills;
		nouvelle_communication = VRAI;
		trigger_communication = FAUX;
		read_complete = FAUX;
	}
}

ISR (TWI_vect)
{
	static uint8_t oct_cnt = 0;
	
	
	switch(TWSR & 0xF8)
	{
		case START_BIT:
			if(nouvelle_communication){
				TWDR = (AHT10_ADD<<1);
				nouvelle_communication = FAUX;
			}
			else if(trigger_communication){	
				TWDR = (AHT10_ADD<<1)+1;		// W
				trigger_communication = FAUX;
			}
			TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWIE);
			break;
			
		case ADDW_ACK:					 
			TWDR = TRIGGER_MESUREMENT;
			oct_cnt=1;
			TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWIE);
			break;
		
		case ADDR_ACK:					
			TWCR = (1<<TWINT)|(1<<TWEA)|(1<<TWEN)|(1<<TWIE);
			break;
		
		case DATA_ACK:
			switch(oct_cnt){
				case 1 :
					TWDR = MEASURE_DATA0;
					oct_cnt++;
					TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWIE);
					break;
				case 2 :
					TWDR = MEASURE_DATA1;
					oct_cnt++;
					TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWIE);
					break;
				case 3 :
					oct_cnt = 0;
					old_time_i2c = mills;
					trigger_communication = VRAI;
					TWCR = (1<<TWINT)|(1<<TWSTO)|(1<<TWEN)|(1<<TWIE);
					break;
			}
			break;
		
		case READ_ACK:
			
			switch(oct_cnt){
				
				case 0:
					if(TWDR != STATUS_AHT10){
						trigger_communication = VRAI;
						old_time_i2c = mills;
						TWCR = (1<<TWINT)|(1<<TWSTO)|(1<<TWEN)|(1<<TWIE);
					}
					else{
						oct_cnt++;
						capteur.data64 = TWDR;
						TWCR = (1<<TWINT)|(1<<TWEA)|(1<<TWEN)|(1<<TWIE);
					}
				break;
					
				case 1:
					oct_cnt++;
					capteur.data64 = (capteur.data64<<8) + TWDR;
					TWCR = (1<<TWINT)|(1<<TWEA)|(1<<TWEN)|(1<<TWIE);
				break;						
				
				case 2:
					oct_cnt++;
					capteur.data64 = (capteur.data64<<8) + TWDR;
					TWCR = (1<<TWINT)|(1<<TWEA)|(1<<TWEN)|(1<<TWIE);
				break;

				case 3:
					oct_cnt++;
					capteur.data64 = (capteur.data64<<8) + TWDR;
					TWCR = (1<<TWINT)|(1<<TWEA)|(1<<TWEN)|(1<<TWIE);
				break;
					
				case 4:
					oct_cnt++;
					capteur.data64 = (capteur.data64<<8) + TWDR;
					TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWIE);
				break;
					
			}
			break;
		
		case READ_NOACK:
		
			oct_cnt = 0;
			capteur.data64 = (capteur.data64<<8) + TWDR;
			read_complete = VRAI;
			TWCR = (1<<TWINT)|(1<<TWSTO)|(1<<TWEN)|(1<<TWIE);
		break;
	}
	
}