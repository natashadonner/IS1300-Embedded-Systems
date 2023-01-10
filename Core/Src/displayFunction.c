/**
 ******************************************************************************
@brief 		display, rtc and potentiometer functions
@file  		displayFunction.c
@author 	Natasha Donner
@version 	1.0
@date 		5-januari-2022
******************************************************************************
**/

#include "displayFunction.h"
#include "main.h"
#include "usart.h"
#include "gpio.h"
#include "spi.h"
#include "stm32l4xx_hal.h"
#include "rtc.h"
#include "tim.h"
#include "adc.h"


static RTC_TimeTypeDef sTime;
static RTC_DateTypeDef sDate;
static uint32_t prevsec = 0;
static uint32_t hh = 0;
static uint32_t mm = 0;
static uint32_t ss = 0;



/* @brief initialize the screen 
 * @param made from INITIALISATION EXAMPLE in data sheet for EA DOGS104-A
 * 
 */

void display(void)

{
	display_init();
	set_time();				// passed, can write in the "screen" mode on mac, have to update how it is presented on the screen
	while(1)
	{

		write_time();      // passed
		set_dimmer();      // passed

	}

}


void display_init(void)
{
	SET_CS;
	HAL_Delay(10);
	RESET_CS;

	display_reset();
	HAL_Delay(10);

	display_send_inst(0x0F);   	// Display on
	display_send_inst(0x3A);   	// 8 bit datalength extension Bit RE = 0; REV = 0
	display_send_inst(0x09);   	// 4 line display
	display_send_inst(0x06);	// bottom view
	display_send_inst(0x1E);    // BS1 = 1
	display_send_inst(0x39);	// 8 bit datalength extension Bit RE = 0, IS = =
	display_send_inst(0x1B);	// BSO = 1 -> Bias = 1/6
    display_send_inst(0x6E);    // Devider in and set value
    display_send_inst(0x56); 	// Booster on and set contrast (DB1 = C5, DB = C4)
    display_send_inst(0x7A);    // Set contrast (DB3 - DB0, C3 - C0)
    display_send_inst(0x38);    // 8 bit data length extension Bit RE = 0, IS = 0
	display_send_inst(0x01);    // clear display


	SET_CS;

}

/* @brief send instruction to the screen
 * @param instruction is always 1 byte.
 * @more information found in datasheet
 */
void display_send_inst(uint8_t instruction)
{
	SET_CS;
	HAL_Delay(10);
	RESET_CS;

	uint8_t buff[3];
	buff[0] = 0x1F;				  // Informs its an instruction to be sent
	buff[1] = instruction & 0x0F; // mask the LSB, split into 4 bit values as
	buff[2] = (instruction >> 4); // mask the MSB

	HAL_SPI_Transmit(&hspi2, buff, 3, 10); // sends 24 bits the LSB first
}

/* @brief set_time, sets the time from the screen function in mac or putty on windows.
 * @param takes in hours, minutes, and second.
 * @more sets time in RTC (real time clock)
 */
void set_time(void){

	uint8_t *buff = "\n write hours\n\r";
	HAL_UART_Transmit(&huart5, (uint8_t *)buff, 17, 5000);      			// sends to the screen
	while(HAL_UART_Receive(&huart5, (uint8_t*)&hh, 2, 1000) != HAL_OK); 	// receive from screen input of two bits and stores in variable hh

	buff = "write minutes\n\r";
    HAL_UART_Transmit(&huart5, (uint8_t *)buff, 18, 5000);
    while(HAL_UART_Receive(&huart5, (uint8_t*)&mm, 2, 1000) != HAL_OK);   	// receive from screen input of two bits and stores in variable mm

    buff = "write seconds\n\r";
    HAL_UART_Transmit(&huart5, (uint8_t *)buff, 18, 5000);
    while(HAL_UART_Receive(&huart5, (uint8_t*)&ss, 2, 1000) != HAL_OK);		// receive from screen input of two bits and stores in variable ss


    buff = "transfer is done!\n\r";
    HAL_UART_Transmit(&huart5, (uint8_t *)buff, 20, 5000);


    sscanf((uint8_t *)&hh, "%hhu", &sTime.Hours);							//stores new input value hh in adress of &sTime.Hours
    sscanf((uint8_t *)&mm, "%hhu", &sTime.Minutes);							//stores new input value mm in adress of &sTime.Minutes
    sscanf((uint8_t *)&ss, "%hhu", &sTime.Seconds);							//stores new input value ss in adress of &sTime.Seconds

    HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BIN); 						// Sets date
    HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BIN);							// Sets time

}


/* @brief Get time from RTC (real time clock)
 * @param takes in hours, minutes, and second.
 */
void write_time(void)
{
	char timearray[8]; // creates char array to store the value in right format
	HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
	HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);
	sprintf(timearray, "%02d:%02d:%02d", sTime.Hours, sTime.Minutes, sTime.Seconds);  // write to char array on format _,_:_,_

	display_string(timearray, 1); // display string (char[]);


}

/* @brief wait_cycles
 * @param the clk is 80Mhz
 * @calling wait_cykles(8000000) means waiting for 0,1s.
 */
void wait_cycles( uint32_t n )
{
	uint32_t l = n/3; //cycles per loop is 3
	asm volatile( "0:" "SUBS %[count], 1;" "BNE 0b;" :[count]"+r"(l));
}

/* @brief display_string takes in two parameters: the adress to string, and which row to write to
 * @instructions 0x80 = row 1, 0x80 + 0x20 = row 2, 0x80 + 0x40 = row 3, 0x80 + 0x60 = row 4
 */
void display_string(uint8_t* string, uint8_t row)
{
	if (row == 1)
	{
		display_send_inst(0x80); //sends instruction to display to write data on the first row.
		do
		{
			display_char(*string++); // display every char on every index, implements index by one every loop.
		}
		while (*string);

	}

	if (row == 2)
	{
		display_send_inst(0x80 + 0x20); //sends instruction to display to write data on the second row
		do
		{
			display_char(*string++);
		}
		while (*string);

		}

	if (row == 3)
	{
		display_send_inst(0x80 + 0x40); //sends instruction to display to write data on the first third row
		do
		{
			display_char(*string++);
		}
		while (*string);

	}

	if (row == 4)
	{
		display_send_inst(0x80 + 0x60); //sends instruction to display to write data on the first fourth row
		do
		{
			display_char(*string++);
		}
		while (*string);

	    }
}

/* @brief display_char
 * @param sends LSB first which tells what instruction should be performed.
 */
void display_char(uint8_t chars)
{
	SET_CS;
	HAL_Delay(10);
	RESET_CS;

	uint8_t transfer[3];
	transfer[0] = 0x5f;
	transfer[1] = chars & 0x0f;
	transfer[2] = chars >> 4;
	HAL_SPI_Transmit(&hspi2, transfer, 3, 10);   // transmit throught SPI

}

/* @brief set dimmer
 * @param using adc value to dim the color
 */
void set_dimmer()
{
	uint32_t adc = get_adc();
	htim3.Instance -> CCR2 = adc;
}

/* @brief get adc
 * @param get the value of the adc. Has to be in a constant while loop or called by rtos every ms to work.
 */
uint32_t get_adc()
{
	uint32_t adc;
	HAL_ADC_Start(&hadc1);
	if(HAL_ADC_PollForConversion(&hadc1, 100) == HAL_OK)
	{
		adc = HAL_ADC_GetValue(&hadc1);
	}
	return adc;
}
/* @brief display reset
 */
void display_reset()
{
	HAL_GPIO_WritePin(Display_reset_GPIO_Port, Display_reset_Pin, GPIO_PIN_SET);
}
