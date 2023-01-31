/*
 * rs232.c
 *
 * Created: 2022-01-26 09:34:40
 *  Author: cedric.cloutier
 */ 

#define F_CPU       16000000
#include <avr/io.h>
#include <avr/interrupt.h>
#include <string.h>
#include <stdlib.h>

#include "rs232.h"
#include "wifi.h"
#include "objets.h"


char Rx_buffer[64];
uint8_t Rx_offset_in = 0;
uint8_t Tx_buffer[64];
char *donnee[30];



void init_rs232(uint16_t baudrate){
	
	UBRR0 = (F_CPU/16/baudrate)-1;
	
	
	UCSR0B |= ((1<<RXEN0) | (1<<TXEN0));						//activation du TX et RX
	
	UCSR0B |= ((1<<RXCIE0) | (1<<TXCIE0));						//activation Interrupt RX et TX
	
}


void rs232_puts(char *text){
	
	while(*text){
		Tx_buffer[Tx_offset_in++] = *text++;
		Tx_offset_in &= 0x3f;
	}
	Tx_buffer[Tx_offset_in++] = ESPACE;							// ajout de " "
	Tx_offset_in &= 0x3f;
	Tx_buffer[Tx_offset_in++] = CR;								// ajout de CR
	Tx_offset_in &= 0x3f;
	Tx_buffer[Tx_offset_in++] = LF;								// ajout de LF
	Tx_offset_in &= 0x3f;
	cli();
	Tx_etat =TX_GO;
	sei();
}

void rs232_print(void){
	
	if(Tx_offset_in != Tx_offset_out){
		cli();
		Tx_etat = TX_BUSY;
		sei();
		UDR0 = Tx_buffer[Tx_offset_out++];
		Tx_offset_out &= 0x3f;
	}
	else {
		cli();
		Tx_etat = TX_STOP;
		sei();
	}	
}

void TX_event(void){
	cli();
	uint8_t tx_etat_local = Tx_etat;
	sei();

	switch (tx_etat_local){
		case TX_GO:
		rs232_print();
		break;
		
		default:
		break;
	}
}
void RX_event(void){
	char codeWifi = 0;
	cli();
	uint8_t rx_etat_local = Rx_etat;
	sei();
	
	char donnee[20];
	//char codeTest[30];
	
	switch(rx_etat_local){
		case RX_GO:
			codeWifi = analyse_reception(donnee);
			if(codeWifi == ROTATION){
			moteur(atoi(donnee));
			}
		
			else if(codeWifi == INTENSITE){
			lumiere(atoi(donnee));
			}
			else if(codeWifi == TEST){
				test();
			}
		
		break;
			
		default:
		break;	
	}
}

char analyse_reception(char *donnee){ 
	
	
	
	if(strstr(Rx_buffer,"MOTEUR")){
		strcpy(donnee, Rx_buffer+7);
		return ROTATION;
	}
	if(strstr(Rx_buffer, "LIP")){
		strcpy(donnee, Rx_buffer+4);
		return IP_RECUE;
	}
	if(strstr(Rx_buffer, "INTENSITE")){
		strcpy(donnee, Rx_buffer+9);
		return INTENSITE;	
	}
	if (strstr(Rx_buffer, "JAP 1")){
		return CONNECTED;
	}
	if (strstr(Rx_buffer, "JAP 0")){
		return NOTCONNECTED;
	}	
	if(strstr(Rx_buffer, "DECONNEXION")){
		return DECONNEXION;
	}
	if (strstr(Rx_buffer, "OK")){
		return OK;	
	}
	if(strstr(Rx_buffer, "TEST")){
		return TEST;
	}
	else{	
		return 0;
	}
}

ISR(USART_TX_vect){
	Tx_etat = TX_READY;
}

ISR(USART_RX_vect){
	Rx_etat =RX_STOP;
	
	uint8_t rx_reg = UDR0; 
	
	if(rx_reg == LF){
		Rx_buffer[Rx_offset_in++] = 0;
		Rx_offset_in = 0;
		Rx_etat =RX_GO;
	}
	else{
		Rx_buffer[Rx_offset_in++] = rx_reg;
		Rx_offset_in &= 0x3f;
	}
}