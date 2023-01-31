/*
 * setup_timer.h
 *
 * Created: 2022-01-24 10:50:37
 *  Author: cedric.cloutier
 */ 


#ifndef SETUP_TIMER_H_
#define SETUP_TIMER_H_

#define MOTEUR_DISABLE			PORTB |= (1<<PORTB4)
#define MOTEUR_ENABLE			PORTB &= ~(1<<PORTB4)		
#define MOTEUR_DD_ENABLE		DDRD  |=  (1<<DDD4)

#define MOTEUR_DD_PWM			DDRB  |=  (1<<DDB2)

#define MOTEUR_DD_ROTATION		DDRB  |=  (1<<DDB3)
#define MOTEUR_HORAIRE          PORTB |= (1<<PORTB3)
#define MOTEUR_ANTIHORAIRE		PORTB &= (0<<PORTB3)

#define LED_DD_OUT				DDRD  |= (1<<DDD3)

#endif /* SETUP_TIMER_H_ */

void setupTim0(void);
void setupPWM1(void);
void setupPWM2(void);
void setupTpT0(void);

ISR (TIMER0_COMPA_vect);

uint32_t mills;