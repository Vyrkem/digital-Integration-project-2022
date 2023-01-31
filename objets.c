/*
 * objets.c
 *
 * Created: 2022-02-28 10:31:24
 *  Author: cedric.cloutier
 */
#include <stdio.h>
#include <stdlib.h>
#include <avr/io.h>
#include "setup_timer.h"
#include "objets.h"

void moteur(int valeur){
	const uint16_t look_up[21] = {RPM_MIN,15449,8875,6127,4707,3821,3216,2776,2442,2180,1968,1794,1649,1525,1418,1325,1244,1172,1108,1051,RPM_MAX};
	valeur /= 5;
	if(valeur == 0){
		MOTEUR_DISABLE;
		}else{
		if(valeur < 0){
			valeur *= -1;
			MOTEUR_ANTIHORAIRE;
			}else{
			MOTEUR_HORAIRE;
		}
		OCR1A = look_up[valeur];
		OCR1B = OCR1A >> 5;
		MOTEUR_ENABLE;
	}
}

void lumiere(int valeur){
	if(valeur==0){
		TCNT2 = 0;
		TCCR2B &=  ~(1 << CS22) ;
		OCR2B = 0;
		}
	else{
		
		TCNT2 = 0;
		TCCR2B |= (1 << CS22);
		OCR2B = (INTENSITE_MAX/100.0)*valeur;
	}
}

void test(){
	lumiere(60);
	moteur(100);
}