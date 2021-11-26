/*
 * GccApplication3.c
 *
 * Created: 12/29/2020 9:25:27 AM
 * Author : Geethaka
 */ 
#define F_CPU 8000000UL

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/delay.h>


#define INPUT 0 // For arduino like functionality
#define OUTPUT 1
#define HIGH 1
#define LOW 0

#define LSBFIRST 0 // For shiftOut function
#define MSBFIRST 1

#define shiftOutDelay 20 // delay value for shiftOut function


#define pinMode(pin,dir) ((dir==1)? (DDRB |= (1<<pin)): (DDRB &= ~(1 << pin))) // arduino function knock-offs
#define digitalWrite(pin, val) ((val == 1)? (PORTB |= (1 << pin)): (PORTB &= ~(1 << pin)))
#define delay(val) _delay_ms(val)
#define delayMicroseconds(val) _delay_us(val)

#define SET_PIN(port, pinNumber) (port |= (1 << pinNumber)) // low level functions for shiftOut function
#define CLEAR_PIN(port, pinNumber) (port &= ~(1 << pinNumber))

unsigned long time_from_start; // for millis function

unsigned long millis() //arduino millis function like one
{
	return time_from_start;
}

unsigned long t1 = 0; // for millis function
unsigned long t2 = 0;

uint8_t t = 100; //One frame time


void shiftOut(uint8_t dataPin, uint8_t clockPin, uint8_t bitOrder, int val) // arduino shiftOut function like one
{
	char i;

	for (i = 0; i < 8; i++)  {
		if (bitOrder == LSBFIRST){
			(val & (1 << i))?SET_PIN(PORTB,dataPin):CLEAR_PIN(PORTB,dataPin); // shift data in LSBFIRST mode
		}
		else{
			(val & (1 << (7 - i)))?SET_PIN(PORTB,dataPin):CLEAR_PIN(PORTB,dataPin); // shift data in MSBFIRST mode
		}
		
		//delayMicroseconds(20);
		SET_PIN(PORTB, clockPin); // shift pin High
		_delay_us(shiftOutDelay); // delay for rise-up
		CLEAR_PIN(PORTB, clockPin); //  shift pin Low
	}
}

void turnOnLED(int * reg_vals_ind, int column, int row) // get register values after given led is turned on
{
	if (column > 15)
	{
		*(reg_vals_ind + 1) |= (1 << (column - 16));
		*(reg_vals_ind + 2) = 0;
		*(reg_vals_ind + 3) = 0;
	}

	else if (column > 7)
	{
		*(reg_vals_ind + 1) |= 0b00000000;
		*(reg_vals_ind + 2) |= (1 << (column - 8));
		*(reg_vals_ind + 3) = 0;
	}

	else
	{
		*(reg_vals_ind + 1) |= 0b00000000;
		*(reg_vals_ind + 2) = 0;
		*(reg_vals_ind + 3) |= (1 << column);
	}
	
	if (row > 5)
	{
		*(reg_vals_ind) |= 1 << (row - 6);
	}
	else
	{
		*(reg_vals_ind + 1) |= 1 << (row + 2);
	}
}

void writeRegVals(uint8_t dataPin, uint8_t clockPin, uint8_t latchPin, int * reg_index_0)
{
	digitalWrite(latchPin, LOW); // shiftOut data
	for (int reg_index = 0; reg_index < 4; reg_index++)
	{
		shiftOut(dataPin, clockPin, MSBFIRST, * (reg_index_0 + reg_index));
	}
	_delay_us(shiftOutDelay);
	digitalWrite(latchPin, HIGH);
}



void timer_setup()  //handling timer for millis function
{
	TCCR0A |= (1 << WGM01);
	TCCR0B |= (1 << CS01);
	TCCR0B &= ~(1 << CS02);
	TCCR0B |= (1 << CS00);
	TCNT0 = 0;
	OCR0A = 125;
	TIMSK = (1 << OCIE0A);
	sei();
	
}

int main(void)
{
	uint8_t dataPin = 0; //define pins for serial communication with shift regs
	uint8_t clockPin = 1;
	uint8_t latchPin = 2;
	pinMode(0, OUTPUT);
	pinMode(1, OUTPUT);
	pinMode(2, OUTPUT);

	timer_setup(); // initialize timer
	
    while (1) //main loop
    {
		
		//First pattern
		unsigned long pattern_start_time = millis();
		unsigned long pattern_time = millis();
		while (pattern_time - pattern_start_time < 60000)
		{
			for(int i = 0; i < 12; i++) // loop for iterating through all 12 patterns in column 0
			{
				t1 = millis();
				t2 = millis();
				while (t2 - t1 < t) // display one frame for t milliseconds
				{
							
					int val = i; //first column pattern index
					for (int column = 0; column < 18; column++) //iterate through all the columns and turn on relevant LEDs for one frame
					{
						int reg_vals[4] = {0,0,0,0}; //initial register values
								
						if (val == 1)
						{
							turnOnLED(& reg_vals, column, 0);
						}
						else if (val < 11)
						{
							turnOnLED(& reg_vals, column, val - 2);
							turnOnLED(& reg_vals, column, val - 1);
						}
						else if (val == 11)
						{
							turnOnLED(& reg_vals, column, 9);
						}
								
						writeRegVals(dataPin, clockPin, latchPin, & reg_vals);
								
						val++; //increment pattern index for the first column
						if (val > 11)
						{
							val = 0; // reset pattern index for first column
						}
					}
							
					t2 = millis();
							
				}
						
			}
					
			pattern_time = millis();
		}

		
		//Second pattern
		
		t= 150;
		
		pattern_start_time = millis();
		pattern_time = millis();
		while (pattern_time - pattern_start_time < 60000)
		{
			for(int i = 0; i < 11; i++) // loop for iterating through all 12 patterns in column 0
			{
				t1 = millis();
				t2 = millis();
				while (t2 - t1 < t) // display one frame for t milliseconds
				{
					for (int column = 0; column < 18; column++) //iterate through all the columns and turn on relevant LEDs for one frame
					{
						int reg_vals[4] = {0,0,0,0}; //initial register values
						
						if (i == 0)
						{
							turnOnLED(& reg_vals, column, 0);
						}
						else if (i < 10)
						{
							turnOnLED(& reg_vals, column, i - 1);
							turnOnLED(& reg_vals, column, i);
						}
						else
						{
							turnOnLED(& reg_vals, column, 9);
						}
						
						writeRegVals(dataPin, clockPin, latchPin, & reg_vals);
						
					}
					
					t2 = millis();
					
				}
				
			}
			
			pattern_time = millis();
			
		}
		
		//Third pattern
				
		t= 150;
				
		pattern_start_time = millis();
		pattern_time = millis();
		while (pattern_time - pattern_start_time < 60000)
		{
			for(int i = 0; i < 15; i++) // loop for iterating through all 12 patterns in column 0
			{
				t1 = millis();
				t2 = millis();
				while (t2 - t1 < t) // display one frame for t milliseconds
				{
					for (int column = 0; column < 18; column++) //iterate through all the columns and turn on relevant LEDs for one frame
					{
						int reg_vals[4] = {0,0,0,0}; //initial register values
								
						if ((column % 2) && (i > 2))
						{
							if (i == 3)
							{
								turnOnLED(& reg_vals, column, 0);
							}
							else if (i == 4)
							{
								turnOnLED(& reg_vals, column, 0);
								turnOnLED(& reg_vals, column, 1);
							}
							else if (i < 13)
							{
								turnOnLED(& reg_vals, column, i - 5);
								turnOnLED(& reg_vals, column, i - 4);
								turnOnLED(& reg_vals, column, i - 3);
							}
							else if (i == 13)
							{
								turnOnLED(& reg_vals, column, 8);
								turnOnLED(& reg_vals, column, 9);
							}
							else
							{
								turnOnLED(& reg_vals, column, 9);
							}
						}
						
						else if(!(column % 2) && (i < 12))
						{
							if (i == 0)
							{
								turnOnLED(& reg_vals, column, 0);
							}
							else if (i == 1)
							{
								turnOnLED(& reg_vals, column, 0);
								turnOnLED(& reg_vals, column, 1);
							}
							else if (i < 10)
							{
								turnOnLED(& reg_vals, column, i - 2);
								turnOnLED(& reg_vals, column, i - 1);
								turnOnLED(& reg_vals, column, i);
							}
							else if (i == 10)
							{
								turnOnLED(& reg_vals, column, 8);
								turnOnLED(& reg_vals, column, 9);
							}
							else
							{
								turnOnLED(& reg_vals, column, 9);
							}
						}
						
						writeRegVals(dataPin, clockPin, latchPin, & reg_vals);
								
					}
							
					t2 = millis();
							
				}
						
			}
					
			pattern_time = millis();
					
		}
		
		
		//4th pattern
		
		t = 150;
		
		pattern_start_time = millis();
		pattern_time = millis();
		while (pattern_time - pattern_start_time < 60000)
		{
			for(int i = 0; i < 12; i++) // loop for iterating through all 12 patterns in column 0
			{
				t1 = millis();
				t2 = millis();
				while (t2 - t1 < t) // display one frame for t milliseconds
				{
					
					int val = i; //first column pattern index
					for (int column = 0; column < 18; column++) //iterate through all the columns and turn on relevant LEDs for one frame
					{
						int reg_vals[4] = {0,0,0,0}; //initial register values
						
						if (val == 1)
						{
							turnOnLED(& reg_vals, column, 0);
						}
						else if (val < 11)
						{
							turnOnLED(& reg_vals, column, val - 2);
							turnOnLED(& reg_vals, column, val - 1);
						}
						else if (val == 11)
						{
							turnOnLED(& reg_vals, column, 9);
						}
						
						writeRegVals(dataPin, clockPin, latchPin, & reg_vals);
						
						int reg_vals_2[4] = {0,0,0,0}; //initial register values
						
						if (val == 1)
						{
							turnOnLED(& reg_vals_2, (17 - column), 0);
						}
						else if (val < 11)
						{
							turnOnLED(& reg_vals_2, (17 - column), val - 2);
							turnOnLED(& reg_vals_2, (17 - column), val - 1);
						}
						else if (val == 11)
						{
							turnOnLED(& reg_vals_2, (17 - column), 9);
						}
						
						writeRegVals(dataPin, clockPin, latchPin, & reg_vals_2);
						
						val++; // increment pattern index for the first column
						if (val > 11)
						{
							val = 0; // reset pattern index for first column
						}
					}
					
					t2 = millis();
					
				}
				
			}
			
			pattern_time = millis();
		}
		
		//5th pattern
		
		t= 150;
		
		pattern_start_time = millis();
		pattern_time = millis();
		while (pattern_time - pattern_start_time < 60000)
		{
			t1 = millis();
			t2 = millis();
			while (t2 - t1 < t) // display one frame for t milliseconds
			{
					
				for (int column = 0; column < 18; column++) //iterate through all the columns and turn on relevant LEDs for one frame
				{
					int reg_vals[4] = {0,0,0,0}; //initial register values
						
					turnOnLED(& reg_vals, column, 0);
					writeRegVals(dataPin, clockPin, latchPin, & reg_vals);
					
					int pattern_index = column % 9;
						
					if (0 < pattern_index && pattern_index < 8)
					{
						turnOnLED(& reg_vals, column, 1);
						writeRegVals(dataPin, clockPin, latchPin, & reg_vals);
					}
						
					if (1 < pattern_index && pattern_index < 7)
					{
						turnOnLED(& reg_vals, column, 2);
						writeRegVals(dataPin, clockPin, latchPin, & reg_vals);
					}
						
					if (2 < pattern_index && pattern_index < 6)
					{
						turnOnLED(& reg_vals, column, 3);
						writeRegVals(dataPin, clockPin, latchPin, & reg_vals);
					}
						
					if (pattern_index == 4)
					{
						turnOnLED(& reg_vals, column, 4);
						writeRegVals(dataPin, clockPin, latchPin, & reg_vals);
					}
						
				}
					
				t2 = millis();
				
			}
			
			pattern_time = millis();
			
		}
		
		
    }
}

ISR(TIMER0_COMPA_vect) //ISR for timer
{
	time_from_start++;
}

