/*
 * wifi.c
 *
 * Created: 2022-01-26 09:36:48
 *  Author: cedric.cloutier
 */ 
#define F_CPU 16000000
#include <util/delay.h>

#include <avr/io.h>
#include <avr/interrupt.h>

#include <stdlib.h>
#include <stdio.h>

#include "wifi.h"
#include "rs232.h"
#include "lcd_4bits.h"

uint8_t wifi_etat;
char phrase[30];
char IP_address[16];


void init_wifi(void){
	cli();
	wifi_etat = WIFI_STOP;
	sei();
}

uint8_t wifi_event(char *ssid,char *password){
	
	uint8_t local_wifi_etat;
	unsigned char local_rx_etat;
	
	cli();
	local_wifi_etat = wifi_etat;
	local_rx_etat = Rx_etat;
	sei();
	
	
	switch(local_wifi_etat){
		
		case WIFI_STOP :
		cli();
		wifi_etat = WIFI_QAP;
		sei();
		rs232_puts("QAP");
		break;
		
		
		case WIFI_QAP :
		if (local_rx_etat == RX_GO){
			if (analyse_reception(0) == OK){
				cli();
				wifi_etat = WIFI_STATION;
				Rx_etat = RX_STOP;
				sei();
				rs232_puts("STATION");
			}
		}
		break;
		
		
		case WIFI_STATION :
		if (local_rx_etat == RX_GO){
			if (analyse_reception(0) == OK){
				cli();
				wifi_etat = WIFI_CONNECTING;
				Rx_etat = RX_STOP;
				sei();
				
				sprintf(phrase,"JAP %s %s",SSID,PASSWD);
				rs232_puts(phrase);
			}
		}
		break;
		
		
		case WIFI_CONNECTING :
		if (local_rx_etat == RX_GO){
			if (analyse_reception(0) == CONNECTED){
				cli();
				wifi_etat = WIFI_LOCAL_IP;
				Rx_etat = RX_STOP;
				sei();
				
				rs232_puts("LIP");
				
				}else if (analyse_reception(0) == NOTCONNECTED){
				cli();
				Rx_etat = RX_STOP;
				sei();
				sprintf(phrase,"JAP %s %s",SSID,PASSWD);
			}
		}
		break;
		
		
		case WIFI_LOCAL_IP :
		if (local_rx_etat == RX_GO){
			
			if (analyse_reception(IP_address) == IP_RECUE){
				cli();
				wifi_etat = WIFI_CONNECTED;
				Rx_etat = RX_STOP;
				sei();
				lcd_puts(2,1,IP_address);
				return CONNECTED;
			}
		}
		break;
		
		
		case WIFI_CONNECTED :
		if (local_rx_etat == RX_GO){
			if (analyse_reception(0) == DECONNEXION){
				cli();
				wifi_etat = WIFI_STOP;
				Rx_etat = RX_STOP;
				sei();
				
				lcd_puts(2,1,"DECONNEXION !");
				cli();
				Tx_offset_in = 0;
				Tx_offset_out = 0;
				sei();
				}else{
				return CONNECTED;
			}
		}
		return CONNECTED;
		break;
	}
	return 0;
}
