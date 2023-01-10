/**
 ******************************************************************************
@brief 		test, testing functions for display
@file  		test.c
@author 	Natasha Donner
@version 	1.0
@date 		5-januari-2022
******************************************************************************
**/

#include "test.h"
#include <stdio.h>
#include "main.h"
#include "displayFunction.h"
#include "rtc.h"
#include "stm32l4xx_hal.h"
#include "usart.h"

static RTC_TimeTypeDef sTime;
static RTC_DateTypeDef sDate;


/**
@brief main test program
@param test rows, backlight, set-time, write time, set dimmer.
*/
void test_program(void)
{

	test_display_init();  	// passed
	test_write_char();		// passed
	test_backlight();		// only got it to work for green and white
	test_rows();			// passed
	test_rtc();				// passed
	test_potentiometer();
	//read_sensor();		// never got the right value on the screen so I took away this function.

	set_time();				// passed, can write in the "screen" mode on mac, have to update how it is presented on the screen
	while(1)
	{

		write_time();      // passed
		set_dimmer();      // passed

	}
}



void test_display_init()
{
	display_init();
	HAL_GPIO_WritePin(Display_white_GPIO_Port, Display_white_Pin, SET);
}

/* @brief display char at row one
 */
void test_write_char()
{
     display_send_inst(0x80);
     uint8_t* string = "A B C D E F G ";
     do
     {
    	display_char(*string++); // display every char on every index, implements index by one every loop.
     }
     while (*string);
     wait_cycles(80000000);
     display_send_inst(0x01);

}

/**
@brief test backlight of green and white.
@param function shows first white backlight for 1s and then green.
*/
void test_backlight()
{
	GPIO_TypeDef* ports[] = {Display_white_GPIO_Port, Display_green_GPIO_Port};  	// Defines array with white and green Port as values, at index 0 and 1.
	int16_t pins[] = {Display_white_Pin, Display_green_Pin}; 						// Defines array with white and green Pin as values, at index 0 and 1.

	for (int i = 0; i < 2; ++i)
	{
		HAL_GPIO_WritePin(ports[i], pins[i], GPIO_PIN_SET);     // Enables color
		wait_cycles(80000000);                              	// shows color for 1 sec.
	    HAL_GPIO_WritePin(ports[i], pins[i], GPIO_PIN_RESET);   // Reset color
	}
}


/**
@brief test display_string()
@param prints Hello! on every row on the display. I use 4 rows on the display.
*/
void test_rows(void)
{
	uint8_t* test = "test!";
	uint8_t* empty = "      ";

	for(uint8_t i = 1; i < 5; i ++)
	{

		display_string(test,i);	//calls for display_string() and rights "test" from row 1-4.
		wait_cycles(80000000);  	// 80 000 000 cycles is 1 second.
		display_send_inst(0x01);    // clears display

	}

}



void test_rtc()
{

		char timearray[8]; // creates char array to store the value in right format
		HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
		HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);
		sprintf(timearray, "%02d:%02d:%02d", sTime.Hours, sTime.Minutes, sTime.Seconds);  // write to char array on format _,_:_,_

		display_string(timearray, 1); // display string (char[]);
		wait_cycles(800000000);
		display_send_inst(0x01);

}


void test_potentiometer()

{	char result[50];
	uint32_t adc = get_adc(); // gets value of potentiometer, can be between 0 - 4096.
	sprintf(result, "%d", adc);
	display_string(result, 1);


}














