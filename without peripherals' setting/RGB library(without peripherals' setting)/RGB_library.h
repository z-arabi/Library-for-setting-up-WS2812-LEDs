#ifndef __WS2812
#define __WS2812

#include <stdio.h>
#include <string.h>
#include "stm32f1xx_hal.h"

#define TIMx                              TIM2
#define TIM_CHANNEL_x											TIM_CHANNEL_1

/* Definition for TIMx clock resources */
#define TIMx_CLK_ENABLE                  	__HAL_RCC_TIM2_CLK_ENABLE
#define DMAx_CLK_ENABLE                  	__HAL_RCC_DMA1_CLK_ENABLE

/* Definition for TIMx Pins */
#define TIMx_CHANNEL1_GPIO_CLK_ENABLE    	__HAL_RCC_GPIOA_CLK_ENABLE
#define TIMx_GPIO_CHANNEL1_PORT          	GPIOA
#define GPIO_PIN_CHANNEL1                	GPIO_PIN_0
#define GPIO_AF_TIMx                     	GPIO_AF1_TIM2

/* Definition for TIMx's DMA */
#define TIMx_CC1_DMA_INST                	DMA1_Channel5

/* Definition for DMAx's NVIC */
#define TIMx_DMA_IRQn                    	DMA1_Channel5_IRQn
#define TIMx_DMA_IRQHandler              	DMA1_Channel5_IRQHandler

//WS2812
#define WS2812_FREQ												(800000) 			// WS2812 require 800kHz frequency
#define TIMER_CLOCK_FREQ									(72000000)   	// it is depend on APB clk-bus
#define TIMER_PERIOD											(TIMER_CLOCK_FREQ / WS2812_FREQ)
#define LED_NUMBER												 3						// number of LEDs we must control
#define LED_DATA_SIZE											(LED_NUMBER * 24) 
#define RESET_SLOTS_BEGIN									50	//code reset time
#define RESET_SLOTS_END										50	//code reset time
#define WS2812_LAST_SLOT									1		//set one bit for end of sending pulse
#define LED_BUFFER_SIZE										(RESET_SLOTS_BEGIN + LED_DATA_SIZE + WS2812_LAST_SLOT + RESET_SLOTS_END)
#define WS2812_0													(TIMER_PERIOD / 3)			// WS2812's zero => high time is one third of the period
#define WS2812_1													(TIMER_PERIOD * 2 / 3)	// WS2812's one => high time is two thirds of the period
#define WS2812_RESET											0 

void ws2812_init(void);
void ws2812_update(void);
void setLEDcolor(uint32_t LEDnumber, uint8_t RED, uint8_t GREEN, uint8_t BLUE);
void setWHOLEcolor(uint8_t RED, uint8_t GREEN, uint8_t BLUE);
void diff_LEDcolors(uint8_t GREEN, uint8_t RED, uint8_t BLUE,int ch_LEDnumber);
void fillBufferBlack(void);
void fillBufferWhite(void);

#endif
