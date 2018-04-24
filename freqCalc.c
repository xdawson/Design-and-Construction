#include "STM32F4xx.h"
#include "freqCalc.h"
#include "digitalIO.h"
#include <stdlib.h>

// TODO: rename
typedef struct FreqVals {
	int lastReadVal;
	int hasStartedTiming;
	double timerCount;
	double averages[10];
} FreqVals;

FreqVals *vals;

void initFreqCalc(void) {
	RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
	
	GPIOA->MODER &= ~GPIO_MODER_MODE6_Msk;
	GPIOA->MODER |= GPIO_MODER_MODE6_1;
	
	GPIOA->AFR[0] &= ~GPIO_AFRL_AFRL6;
	GPIOA->AFR[0] &= ~GPIO_AFRL_AFRL6_0;
	GPIOA->AFR[0] |= GPIO_AFRL_AFRL6_1;
	GPIOA->AFR[0] &= ~GPIO_AFRL_AFRL6_2;
	GPIOA->AFR[0] &= ~GPIO_AFRL_AFRL6_3;
	TIM3->CCMR1 |= TIM_CCMR1_CC1S_0;
	
	TIM3->CCER &= ~TIM_CCER_CC1P;
	TIM3->CCER &= ~TIM_CCER_CC1NP; 
	
	TIM3->CCMR1 |= TIM_CCMR1_CC2S_1;
	
	TIM3->CCER |= TIM_CCER_CC2P;
	TIM3->CCER |= TIM_CCER_CC2NP;
	
	TIM3->SMCR |= TIM_SMCR_TS_0;
	TIM3->SMCR &= ~TIM_SMCR_TS_1;
	TIM3->SMCR |= TIM_SMCR_TS_2;
	
	TIM3->SMCR &= ~TIM_SMCR_SMS_0;
	TIM3->SMCR &= ~TIM_SMCR_SMS_1;
	TIM3->SMCR |= TIM_SMCR_SMS_2;
	
	TIM3->CCER |= TIM_CCER_CC1E;
	TIM3->CCER |= TIM_CCER_CC2E;
	
	TIM3->CR1 |= TIM_CR1_CEN;
}
