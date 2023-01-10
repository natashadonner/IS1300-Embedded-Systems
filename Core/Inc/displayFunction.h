#include <stdbool.h>
#include <stdint.h>
#include "main.h"
#include "stm32l4xx_hal.h"

#ifndef DISPLAY_FUNCTIONS_H
#define DISPLAY_FUNCTIONS_H

#define RESET_CS HAL_GPIO_WritePin(CS_SPI2_GPIO_Port, CS_SPI2_Pin, 0) // Reset pin
#define SET_CS HAL_GPIO_WritePin(CS_SPI2_GPIO_Port, CS_SPI2_Pin, 1)  // Set pin, enables SPI to communicate to screen

#define SET_CSR HAL_GPIO_ReadPin(CS_SPI2_GPIO_Port, CS_SPI2_Pin, 1)
#define RESET_CSR HAL_GPIO_ReadPin(CS_SPI2_GPIO_Port, CS_SPI2_Pin, 0)
void display(void);
void display_init(void);
void display_send_inst(uint8_t instruction);
void set_time(void);
void write_time(void);
void wait_cycles(uint32_t n );
void display_char(uint8_t chars);
void display_string(uint8_t* string, uint8_t row);
void display_reset();
void set_dimmer(void);
uint32_t get_adc(void);

#define Display_off 0x03
#define Display_on 0x04
#define Cursor_off 0x05
#define Cursor_on 0x02
#define Blink_off 0x06
#define Blink_on 0x01

#endif
