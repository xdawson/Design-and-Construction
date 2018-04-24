#include "STM32F4xx.h"
#include <stdlib.h>

#include "UI.h"
#include "lcdDisplay.h"
#include "mathsFunctions.h"
#include "ADC.h"
#include "ranging.h"
#include "buttons.h"
#include "FreqCalc.h"
#include "serial_comms.h"

typedef struct UIVals {
	char *readType[5];
	char *voltageRange[6];
	char *currentRange[4];
	char *resistanceRage[1];
	char *capacitanceRange[3];
	
	int typeIndex;
	int rangeIndex;
	
	//for debouncing
	int prevbtn;
	int debCount;
	//for display
	int dispcount;
	//0 for off, 1 for on
	int autoRangeState;
	int commsState;
} UIVals;


UIVals *interfaceVals;
const int MAXINDEX[5] = {5, 3, 0, 0, 2};


void initUI(void) {
	initButtons();
	initDisplay();
	initFreqCalc();
	// Init the display timer
	RCC->APB1ENR |= RCC_APB1ENR_TIM5EN;
	NVIC_EnableIRQ(TIM5_IRQn);    // Enable IRQ for TIM5 in NVIC

  TIM5->ARR     = 10*84000;    // Auto Reload Register value =>
  TIM5->DIER   |= 0x0001;       // DMA/IRQ Enable Register - enable IRQ on update
  TIM5->CR1    |= 0x0001;       // Enable Counting
	
	RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;
	
	NVIC_EnableIRQ(TIM3_IRQn);    // Enable IRQ for TIM3 in NVIC

	TIM3->ARR     = 20*84000;        // Auto Reload Register value => 1ms
	TIM3->DIER   |= 0x0001;       // DMA/IRQ Enable Register - enable IRQ on update
	TIM3->CR1    |= 0x0001;       // Enable Counting
	
	interfaceVals = malloc(sizeof(UIVals));
	
	//Define the arrays for the user options
	interfaceVals->readType[0] = "V  ";
	interfaceVals->readType[1] = "A  ";
	interfaceVals->readType[2] = "Ohm";
	interfaceVals->readType[3] = "Hz ";
	interfaceVals->readType[4] = "C  ";
	
	interfaceVals->voltageRange[0] =  "0.001";
	interfaceVals->voltageRange[1] =  "0.01";
	interfaceVals->voltageRange[2] =  "0.1";
	interfaceVals->voltageRange[3] =  "1.0";
	interfaceVals->voltageRange[4] = "10.0";
	interfaceVals->voltageRange[5] = "Test";
	
	interfaceVals->currentRange[0] = "0.001";
	interfaceVals->currentRange[1] = "0.01";
	interfaceVals->currentRange[2] = "0.1";
	interfaceVals->currentRange[3] = "1.0";
	
	interfaceVals->capacitanceRange[0] = "1.0";
	interfaceVals->capacitanceRange[1] = "2.0";
	interfaceVals->capacitanceRange[2] = "3.0";

	// Unsure about what ranges we have for the resistance
	interfaceVals->resistanceRage[0] = "1000000";
}


void processButtonPress(int buttonPressed, int* typeIndex, int* rangeIndex, int* autoRangeState, int* commsState) {
	switch(buttonPressed){
		case 1:
			//this button increments read type
			if(*typeIndex == 4) {
				*typeIndex = 0;
			} else {
				++*typeIndex;
			}
		break;				
		case 2:
			//this button decrements read type
			if(*typeIndex == 0) {
				*typeIndex = 4;
			} else {
				--*typeIndex;
			}
		break;				
		case 3:
			if(*autoRangeState == 0){
				//this button increments range type
				// TODO: make the range index change based on what is being measured
				if(*rangeIndex == MAXINDEX[*typeIndex]) {
					*rangeIndex = 0;
				} else {
					++*rangeIndex;
				}
			}
		break;				
		case 4:
			if(*autoRangeState == 0){
				//this button decrements range type
				if(*rangeIndex == 0) {
					*rangeIndex = MAXINDEX[*typeIndex];
				} else {
					--*rangeIndex;
				}
			}
		break;	
		case 5:
			//this button toggles autoranging
			if(*autoRangeState == 0){
				*autoRangeState = 1;
			} else {
				*autoRangeState = 0;
			}
		break;
		case 6:
			//this button toggles serial comms
			if(*commsState == 0){
				*commsState = 1;
			} else {
				*commsState = 0;
			}
	}
}
void display(char *readType[], 
						 char *voltageRange[], 
						 char *currentRange[], 
						 char *resistanceRange[], 
						 char *capacitanceRange[], 
						 int typeIndex, 
						 int rangeIndex, 
						 int autoRangeState,
						 int commsState) {	
	
							 
	//initilise display value
	double displayVal;
	
	//display type
	displayType(readType[typeIndex]);
							 
	//display if we are in auto and/or comms mode or not
	displayAuto(autoRangeState);
	displayComms(commsState);
	
	//display resolution
	switch(typeIndex){
		case 0:		//Voltage
			//Display the range (resolution)
			displayStringRange(voltageRange[rangeIndex]);
		
			//---- Code for displaying Voltage reading ----//
			switch(rangeIndex){
				case 0:
					displayVal = range1m();
					// Display to LCD
					displayReading(displayVal);
				
					// Attempt to send via uart
					if(commsState == 1)
					WriteToOutputString(displayVal);
					
				break;					
				case 1:
					displayVal = range10m();
					// Display to LCD
					displayReading(displayVal);
				
					// Attempt to send via uart
					if(commsState == 1)
					WriteToOutputString(displayVal);
					
				break;					
				case 2:
					displayVal = range10m();
					displayReading(displayVal);
				
					// Attempt to send via uart
					if(commsState == 1)
					WriteToOutputString(displayVal);
					
				break;
				case 3:
					displayVal = range1();
					displayReading(displayVal);
					
					// Attempt to send via uart
					if(commsState == 1)
					WriteToOutputString(displayVal);
					
				break;
				case 4:
					displayVal = range10();
					displayReading(range10());
				
					// Attempt to send via uart
					if(commsState == 1)
					WriteToOutputString(displayVal);
					
				break;
				case 5:
					displayVal = testRange();
					displayReading(displayVal);
				
					// Attempt to send via uart
					if(commsState == 1)
					WriteToOutputString(displayVal);
					
				break;
			}
			
			//-----------------------------------------//
		break;
		case 1:		//Current
			//Display the range (resolution)
			displayStringRange(currentRange[rangeIndex]);
		
			//---- Code for displaying Current reading ----//
			switch(rangeIndex){
				case 0:
					displayVal = range1m();
					// Display to LCD
					displayReading(displayVal);
				
					// Attempt to send via uart
					if(commsState == 1)
					WriteToOutputString(displayVal);
					
				break;					
				case 1:
					displayVal = range10m();
					// Display to LCD
					displayReading(displayVal);
				
					// Attempt to send via uart
					if(commsState == 1)
					WriteToOutputString(displayVal);
					
				break;					
				case 2:
					displayVal = range10m();
					displayReading(displayVal);
				
					// Attempt to send via uart
					if(commsState == 1)
					WriteToOutputString(displayVal);
					
				break;
				case 3:
					displayVal = range1();
					displayReading(displayVal);
					
					// Attempt to send via uart
					if(commsState == 1)
					WriteToOutputString(displayVal);
					
				break;
				}
			//-----------------------------------------//
		break;			
		case 2:		//Resistance
			//Display the range (resolution)
			displayStringRange(resistanceRange[0]);
		
			//---- Code for displaying Resistance reading ----//
			displayReading(readADC1());
			//-----------------------------------------//
		break;
		case 3:		//Frequency
			displayReading(getFrequency());
			//displayReading(getPeriod());
		break;
		case 4:		//Capacitance
			
		//---- Code for displaying Resistance reading ----//
			displayStringRange(capacitanceRange[rangeIndex]);
			if(rangeIndex == 0) {
				// Hacky way of addressing things for harry 
				setRange(0);
				double cap = (getPeriod() / (0.693 * 60000000));
				displayReading(cap);
				
			} else if(rangeIndex == 1) {
				// Hacky way of addressing things for harry 
				setRange(1);
				double cap = (getPeriod() / (0.693 * 60000));
				displayReading(cap);
				
			} else if (rangeIndex == 2) {
				// Hacky way of addressing things for harry 
				setRange(2);
				double cap = (getPeriod() / (0.693 * 6000));
				displayReading(cap);
			}
			
			displayReading(getPeriod());
			//-----------------------------------------//
		break;
	}
	
	
}

void TIM5_IRQHandler(void) {
	// JJ's button Handler!!! LOLS
	// Debounce the shitting button
  TIM5->SR &= ~0x00000001;      // clear IRQ flag in TIM5
	
	// Adjust the internal settings based on user input
	if(interfaceVals->autoRangeState == 1) {
		autoRange(interfaceVals->rangeIndex);
	} else {
		setRange(interfaceVals->rangeIndex);
	}
	
	// Display settings
	interfaceVals->dispcount++;
	
	if (interfaceVals->dispcount ==  20) {
	//display
	display(interfaceVals->readType, 
					interfaceVals->voltageRange, 
					interfaceVals->currentRange, 
					interfaceVals->resistanceRage, 
					interfaceVals->capacitanceRange, 
					interfaceVals->typeIndex, 
					interfaceVals->rangeIndex, 
					interfaceVals->autoRangeState,
				  interfaceVals->commsState);	
	//reset the counter
	interfaceVals->dispcount = 0;
	}
	
}

void TIM3_IRQHandler(void) {
	TIM3->SR &= ~0x00000001;      // clear IRQ flag in TIM3
	
	// Check button press
	int curbtn = getButtonPressed();
	
	if( interfaceVals->prevbtn == curbtn ){
		
		if(interfaceVals->debCount == 3 && curbtn != 0) {
			displayClear();
			processButtonPress(curbtn, &interfaceVals->typeIndex, &interfaceVals->rangeIndex, &interfaceVals->autoRangeState, &interfaceVals->commsState);
			interfaceVals->debCount++;
		} else if(interfaceVals->debCount < 4) {
			interfaceVals->debCount++;
		}	
			
	} else {
		//update the button pressed
		interfaceVals->debCount = 0;
		interfaceVals->prevbtn = curbtn;
	}
}
