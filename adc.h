/*
 * adc.h
 *
 * Created: 2022-01-24 10:51:26
 *  Author: cedric.cloutier
 */ 


#ifndef ADC_H_
#define ADC_H_

#define ADC_STOP			80
#define ADC_GO				82
#define ADC_BUSY			84
#define ADC_CHANGE			86

#define DELAI_ADC			1500

#endif /* ADC_H_ */

void init_adc(void);
void adc_event(void);
ISR(ADC_vect);