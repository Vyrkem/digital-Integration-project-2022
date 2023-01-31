/*
 * lcd_4bits.h
 *
 * Created: 2022-01-24 10:49:38
 *  Author: cedric.cloutier
 */ 

#ifndef  __LCD
#define  __LCD

#define  DONNEE						1
#define  CTRL						0

#define LCD_GO						12
#define LCD_READY					14
#define	LCD_STOP					16

#define LCD_ADRESSE_LIGNE_1			0x00
#define LCD_ADRESSE_LIGNE_2			0x40

#define LCD_DATA_OUT				PORTD
#define LCD_DATA_IN					PIND
#define LCD_DATA_DD					DDRD |= (1<<PORTD7)| (1<<PORTD6) | (1<<PORTD5) | (1<<PORTD4)

#define LCD_DD_EN					DDRB |= (1<<PORTB1)
#define LCD_DD_RS					DDRB |= (1<<PORTB0)

#define LCD_EN_0					PORTB &= ~(1<<PORTB1)
#define LCD_EN_1					PORTB |= (1<<PORTB1)
#define LCD_RS_0					PORTB &= ~(1<<PORTB0)
#define LCD_RS_1					PORTB |= (1<<PORTB0)

#define DISPLAY_ON_CURSEUR_OFF		0x0C
#define CLS							0x01
#define HOME						0x02
#define BACKSPACE					0x10


#endif

void init_lcd(void);
void lcd_transmet_octet( unsigned char registre, unsigned char c);
void lcd_ctrl(unsigned char c);
uint8_t lcd_gotoxy(unsigned char x, unsigned char y);
void lcd_putc(unsigned char c);
void lcd_puts(unsigned char x,unsigned char y,char *text);
void lcd_print(void);
void lcd_event(void);

/* variable globale pour le programme principal */
uint8_t lcd_etat;