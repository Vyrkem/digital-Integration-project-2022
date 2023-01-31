/*
 * adc.c
 *
 * Created: 2022-01-26 08:48:39
 *  Author: cedric.cloutier
 */ 
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include "adc.h"
#include "setup_timer.h"
#include "rs232.h"
#include "wifi.h"
#include "lcd_4bits.h"


//ADC0

uint8_t etat_adc;

//static uint32_t temps_ref;

float reponse_adc;




void init_adc(void){
	ADMUX |= (1<< REFS0);								
	ADCSRA |= (1<<ADEN) | (1<<ADIE) | (1<<ADPS2);
	DIDR0|= (1<<ADC0D);
	etat_adc = ADC_STOP;
}

void adc_event(void){
	uint8_t adc_etat_local;
	char text[36];
	static uint32_t temps_ref;
	static uint32_t temps_pres;
	cli();
	adc_etat_local = etat_adc;
	temps_pres = mills;
	sei();
	
	switch(adc_etat_local){
		
		case ADC_STOP:
			if ((temps_pres-temps_ref)>DELAI_ADC){
				cli();
				etat_adc = ADC_GO;
				sei();
			}
			else;
		
			break;
		case ADC_GO:
			
			ADCSRA |= (1<<ADSC);
			cli();
			etat_adc = ADC_BUSY;
			sei();
			break;
			
		case ADC_BUSY:
		
			break;
		
		case ADC_CHANGE:
			etat_adc = ADC_STOP;
			sprintf(text,"UDP %s %d %.1f", IPNODERED, PORT5000,reponse_adc);		
			
			rs232_puts(text);
			//sprintf(text,"%.1f'         ",reponse_adc);
			//lcd_puts(2,1,text);
			reponse_adc*=2.54;
			sprintf(text,"UDP %s %d %.1f", IPNODERED, PORT5001,reponse_adc);
			rs232_puts(text);
			
			temps_ref= temps_pres;
			
			break;
			
		default:
			
			break;
	}

	
}



ISR(ADC_vect){
	
	static uint16_t somme_adc= 0;
	static uint8_t c= 0;
	if (c<16){
			somme_adc+=(ADC/2); 
			etat_adc = ADC_GO;
			c++;
	}
	else if(c==16){
			reponse_adc = 0;
			reponse_adc =(float) (somme_adc/16);
			somme_adc = 0;
			etat_adc = ADC_CHANGE;
			c = 0;
	}
	
	
}