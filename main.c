/*
 * Projet Intégration Cédric Cloutier-Morin.c
 *
 * Created: 2022-01-24 09:01:11
 * Author : cedric.cloutier
 */ 
#define F_CPU 16000000

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>
#include <util/delay.h>

#include "main_616_H22.h"
#include "lcd_4bits.h"
#include "rs232.h"
#include "adc.h"
#include "setup_aht10.h"
#include "wifi.h"
#include "setup_timer.h"
#include "objets.h"

char *donnee_pub[10];




int main(void)
{
    /* Replace with your application code */
	uint8_t liaison_fait;
	//char donnee[10];
	
	//uint8_t wifi_answ= 0;
	//uint8_t wifi_state_local;
	//uint8_t old_mills;
	
	init_lcd();
	init_rs232(19200);
	setup_i2c(5000);
	setupTim0();					
	setupPWM1();
	setupPWM2();
	
	init_adc();
	init_wifi();
	
	DDRC |= (1<<DDC2);
	
	//lumiere(50);
	//moteur(20);
	mills = 0;
	sei();
	
	lcd_puts(3,1,"Bienvenue !");
	//rs232_puts("wub");
	//liaison_fait = CONNECTED;						//tester sans wifi
	while (1) 
    {

		liaison_fait = wifi_event(SSID,PASSWD);
		
		lcd_event();
		TX_event();
		RX_event();
		
		if(liaison_fait == CONNECTED){
			adc_event();	
			i2c_event();
		}
		
    }
	return 0;
}