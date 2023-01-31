/*
 * CFile1.c
 *
 * Created: 2022-01-25 10:46:44
 *  Author: cedric.cloutier
 */ 

#define F_CPU 16000000
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>

#include "setup_timer.h"
#include "rs232.h"
#include "lcd_4bits.h"
#include "adc.h"
#include "wifi.h"
#include "objets.h"

void setupTim0(void){
	TCCR0B |= ((1<<CS01) | (1<<CS00));   //prescale de 64
	OCR0A  =  250;						//250 * 4us = 1ms
	
	TIMSK0 |= (1<<OCIE0A);				//Interrupt Enable	pour Compare match A
	mills = 0;
}

void setupTpT0(void){
	DDRC |= (1<<DDC1);
}

void setupPWM1(void){
	MOTEUR_DD_PWM;
	MOTEUR_DD_ROTATION;
	MOTEUR_DD_ENABLE;
	MOTEUR_DISABLE;
	MOTEUR_HORAIRE;
	
	TCNT1 = 0;
	TCCR1B |= (1<<WGM13) | (1<<WGM12) | (0<<CS12) | (1<<CS11) | (0<<CS10);
	TCCR1A |= (0<<COM1A1) | (0<<COM1A0) | (1<<COM1B1) | (0<<COM1B0) | (1<<WGM11)| (1<<WGM10);

	OCR1A = RPM_MIN;
	OCR1B = OCR1A >> 5;
}

void setupPWM2(void){
	LED_DD_OUT;
	
	TCNT2 = 0;
	TCCR2B = (1<<CS20);
	TCCR2A = (1<<COM2B1) | (1<<WGM21)| (1<<WGM20);

	OCR2B = 0;
 // ne pas oublier TCCR2B |= ((1<<CS22) & ((1<<CS22) | (1<<CS20)))
}

ISR(TIMER0_COMPA_vect){
	
	mills++;
	
	if(lcd_etat == LCD_READY){
		lcd_etat = LCD_GO;
	}
	
	if(Tx_etat == TX_READY){
		Tx_etat = TX_GO;
	}
	
}