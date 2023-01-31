/*
 * lcd_4bits.c
 *
 * Created: 2022-01-25 10:10:54
 *  Author: cedric.cloutier
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>

#define F_CPU	16000000
#include <util/delay.h>
#include "lcd_4bits.h"					// inclusion du fichier d'ent�te utile au fichier "lcd_4bits.c"

char lcd_ligne;							// variable globale dans le fichier lcd_4bits_new.c
unsigned char lcd_buffer[64];
unsigned char lcd_offset_in;
unsigned char lcd_offset_out;

/*********************************************************************************/
/*						Initialisation de l'affichage                            */
/*********************************************************************************/
void init_lcd(void){
	
	lcd_ligne = 1;
	lcd_offset_in = 0;
	lcd_offset_out = lcd_offset_in;
	
	LCD_DD_EN;						// PORTC5 en sortie
	LCD_DD_RS;						// PORTC4 en sortie
	LCD_DATA_DD;					// met les 4 MSb du PORTD en sortie pour le data bus du LCD

	/* s�quence d'initialisation selon la fiche technique en mode interface 4 bits */
	_delay_ms(16);
	lcd_ctrl(0x03);
	
	_delay_ms(6);
	lcd_ctrl(0x03);

	_delay_ms(5);
	lcd_ctrl(0x03);

	_delay_ms(3);
	lcd_ctrl(0x02);

	_delay_ms(3);
	lcd_ctrl(0x28);
	
	_delay_ms(3);
	lcd_ctrl(0x08);
	
	_delay_ms(1);
	lcd_ctrl(0x01);
	
	_delay_ms(15);
	lcd_ctrl(0x06);
	
	_delay_ms(5);
	lcd_ctrl(0x0c);
	_delay_ms(20);
}

/*****************************************************************************/
/*  lcd_ctrl(c)                                                              */
/*****************************************************************************/
/* Entr�e : c, caract�re � transmettre                                       */
/* Sortie : nil                                                              */
/*****************************************************************************/

void lcd_ctrl(unsigned char c){

	lcd_transmet_octet(CTRL,c);		// appel la fonction de transmission d'un octet
	// au LCD mais en lui sp�difiant que c'est un code de contr�le
}

/*****************************************************************************/
/*  lcd_transmet_octet(action,c)                                           */
/*****************************************************************************/
/* Entr�e : action, indique une commande(0) ou des donn�es (1)               */
/*          c, caract�re � transmettre                                       */
/* Sortie : nil                                                              */
/*****************************************************************************/
void lcd_transmet_octet( unsigned char action, unsigned char c){

	if(action){			// si l'action vaut 1 c'est un caract�re � afficher
		LCD_RS_1;
	}
	else{				// sinon c'est un code de contr�le pour le LCD
		LCD_RS_0;
	}
	LCD_EN_1;			// met le "EN" du LCD � 1
	
	LCD_DATA_OUT = (c & 0xf0)| (LCD_DATA_IN & 0x0f);	// place la donn�e de 4 bits partie haute
	LCD_EN_0;											// sur le PORTD PORTD4 � 7
	LCD_EN_1;											// descend "EN" pour que le LCD latch la donn�e
	// et on remonte "EN"
	
	LCD_DATA_OUT = ((c<<4) & 0xf0) | (LCD_DATA_IN & 0x0f);	// on place la donn�e de 4bits partie basse
	LCD_EN_0;												// sur le PORTD 4 � 7
	// descend "EN" pour que le LCD latch la donn�e
}

/*****************************************************************************/
/*   lcd_gotoxy(x,y)                                                         */
/*****************************************************************************/
/* Entr�es : x : colonne du curseur (1 � 16)                                 */
/*           y : ligne du curseur (1 ou 2)                                   */
/*	 		     position initiale en haut � gauche, (1,1)                   */
/* Sortie  : retourne l'adresse de la DDRAM du lcd                           */
/*****************************************************************************/
uint8_t lcd_gotoxy(unsigned char x, unsigned char y){

	uint8_t adresse;
	switch (y){						// Si la valeur de la ligne vaut 2, on change l'adresse
		case 2 :					// du pointeur de l'affiche dans le DDRAM du LCD
		adresse = LCD_ADRESSE_LIGNE_2;
		lcd_ligne = 2;
		break;
		default :						// Si la valeur de la ligne est tout autre chose que 2
		lcd_ligne = 1;				// alors on replace la valeur de la ligne � 1
		adresse = LCD_ADRESSE_LIGNE_1;
		break;
	}
	adresse |= (0x80 + (x - 1));			// on soustrait 1 � X car la position 1 est pour le LCD la valeur 0
	return adresse;
}

/*****************************************************************************/
/*  lcd_putc(c)                                                              */
/*****************************************************************************/
/* Entr�e : c, caract�re � afficher                                          */
/* Sortie : nil                                                              */
/*****************************************************************************/
void lcd_putc(unsigned char c) {

	switch (c) {
		case 0x80 ... 0x9f:
		lcd_ligne = 1;
		lcd_ctrl(c);
		break;
		case 0xc0 ... 0xdf:
		lcd_ligne = 2;
		lcd_ctrl(c);
		break;
		
		case '\f' :								// si ce caract�re est re�u, on efface le LCD et on revient
		lcd_ctrl(CLS);						// � la ligne 1 position 1
		lcd_ligne = 1;
		break;
		
		case '\n' :								// si ce caract�re est re�u, on fait un changement de ligne
		if(lcd_ligne == 1){
			lcd_ligne = 2;
			lcd_ctrl(0xC0);
			}else{
			lcd_ligne = 1;
			lcd_ctrl(0x80);
		}
		break;
		
		case '\b' :								// si ce caract�re est re�u, on fait d�placer d'une position
		lcd_ctrl(BACKSPACE);				// vers la gauche le curseur
		break;
		
		case '\r' :								// si ce caract�re est re�u, on replace le curseur au d�but
		lcd_ctrl(lcd_gotoxy(1,lcd_ligne));	// de la ligne o� il se trouve
		break;
		
		default :								// si ce n'est aucun de ces caract�res sp�ciaux,on affiche le caract�re
		lcd_transmet_octet(DONNEE,c);
		break;
	}
}

/*******************************************************************************/
/*  lcd_puts(*text)                                                            */
/*******************************************************************************/
/* Entr�e : x, position colonne    y, position de ligne						   */
/*			*text, pointeur sur la cha�ne de caract�res se terminant avec \0   */
/* Sortie : nil                                                                */
/*******************************************************************************/
void lcd_puts(unsigned char x, unsigned char y,char *text) {
	unsigned char adresse;
	
	//lcd_offset_in = 0;
	//lcd_offset_out = 0;
	adresse = lcd_gotoxy(x,y);
	
	
	lcd_buffer[lcd_offset_in++] = adresse;
	lcd_offset_in &= 0x3f;
	while(*text){
		lcd_buffer[lcd_offset_in++] = *text++;
		lcd_offset_in &= 0x3f;
	}
	cli();
	lcd_etat = LCD_GO;
	sei();
	
}

void lcd_print(void){
	
	if (lcd_offset_out != lcd_offset_in){
		
		lcd_putc(lcd_buffer[lcd_offset_out++]);
		lcd_offset_out &=0x3f;
		
		cli();
		lcd_etat = LCD_READY;
		sei();
	}
	else{
		cli();
		lcd_etat = LCD_STOP;
		sei();
	}
}

/*--------------------------------------------------------------*/

void lcd_event(void){
cli();
uint8_t lcd_etat_local = lcd_etat;
sei();

	switch (lcd_etat_local){
		case LCD_GO:
			lcd_print();
		break;
		
		default:
		break;
	}
}