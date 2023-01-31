/*
 * rs232.h
 *
 * Created: 2022-01-24 10:51:05
 *  Author: cedric.cloutier
 */ 


#ifndef RS232_H_
#define RS232_H_


#define CR			        0x0d
#define ESPACE				0x20		
#define LF					0x0a
#define VRAI				1
#define FAUX				0


#define TX_GO				2
#define TX_BUSY				4
#define TX_READY			6
#define TX_STOP				8
#define RX_GO				10
#define RX_STOP				12

#define ROTATION			16
#define INTENSITE			18
#define TEST				20

#define ACCELERATION		202
#define LEDOFF				203
#define LEDON				204

#endif /* RS232_H_ */


void init_rs232(uint16_t baudrate);
void rs232_puts(char *text);
void rs232_print(void);
char analyse_reception(char *donnee);
void TX_event(void);
void RX_event(void);

ISR(USART_TX_vect);
ISR(USART_RX_vect);

//Variables Globales

uint8_t Tx_etat;
uint8_t Rx_etat;
uint8_t Tx_offset_in ;
uint8_t Tx_offset_out ;

//char *donne_pub[30];
