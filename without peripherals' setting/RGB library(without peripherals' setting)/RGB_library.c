#include "RGB_library.h"

static uint8_t LEDbuffer[LED_BUFFER_SIZE];
void setLEDcolor(uint32_t LEDnumber, uint8_t GREEN, uint8_t RED, uint8_t BLUE) 
{

	uint8_t tempBuffer[24];
	uint32_t i;
	uint32_t LEDindex;
	LEDindex = LEDnumber % LED_NUMBER;

	for (i = 0; i < 8; i++) // GREEN data
		tempBuffer[i] = ((GREEN >> i) & 0x01) ? WS2812_1 : WS2812_0;
	for (i = 0; i < 8; i++) // RED data
		tempBuffer[8 + i] = ((RED >> i) & 0x01) ? WS2812_1 : WS2812_0;
	for (i = 0; i < 8; i++) // BLUE data
		tempBuffer[16 + i] = ((BLUE >> i) & 0x01) ? WS2812_1 : WS2812_0;

	for (i = 0; i < 24; i++)
		LEDbuffer[RESET_SLOTS_BEGIN+LEDindex * 24 + i] = tempBuffer[i];
	
	if(LEDindex==LED_NUMBER)
		LEDbuffer[RESET_SLOTS_BEGIN+LEDindex * 24 + WS2812_LAST_SLOT]= WS2812_RESET;	
	
}

void setWHOLEcolor(uint8_t GREEN, uint8_t RED, uint8_t BLUE) 
{
	uint32_t index;

	for (index = 0 ; index <= LED_NUMBER ; index++)
		setLEDcolor(index, GREEN, RED, BLUE);
}
void fillBufferWhite(void) 
{	
	//fill all LEDs white!
	uint32_t index, buffIndex;
	buffIndex = 0;

	for (index = 0; index < RESET_SLOTS_BEGIN; index++) 
	{
		LEDbuffer[buffIndex] = WS2812_RESET;
		buffIndex++;
	}
	for (index = 0; index < LED_DATA_SIZE; index++) 
	{
		LEDbuffer[buffIndex] = WS2812_1;
		buffIndex++;
	}
	LEDbuffer[buffIndex] = WS2812_0;
	buffIndex++;
	for (index = 0; index < RESET_SLOTS_END; index++) 
	{
		LEDbuffer[buffIndex] = 0;
		buffIndex++;
	}
}
void fillBufferBlack(void) 
{
	//set all LEDs off!
	uint32_t index, buffIndex;
	buffIndex = 0;

	for (index = 0; index < RESET_SLOTS_BEGIN; index++) 
	{
		LEDbuffer[buffIndex] = WS2812_RESET;
		buffIndex++;
	}
	for (index = 0; index < LED_DATA_SIZE; index++) 
	{
		LEDbuffer[buffIndex] = WS2812_0;
		buffIndex++;
	}
	LEDbuffer[buffIndex] = WS2812_0;
	buffIndex++;
	for (index = 0; index < RESET_SLOTS_END; index++) 
	{
		LEDbuffer[buffIndex] = 0;
		buffIndex++;
	}
}
void diff_LEDcolors(uint8_t GREEN, uint8_t RED, uint8_t BLUE,int ch_LEDnumber)
{
	//GREEN & RED & BLUE are the default colors
	uint8_t LEDs[ch_LEDnumber];
	uint8_t LEDcolors[3*ch_LEDnumber];
	HAL_UART_Receive(&huart1,(uint8_t *)LEDs,ch_LEDnumber,100);
	HAL_UART_Receive(&huart1,(uint8_t *)LEDcolors,3*ch_LEDnumber,100);
	uint32_t index=0;
	uint32_t j;

	for(j=0	; j<ch_LEDnumber ;j++)
	{ 
		while( LEDs[j] != index)
		{
			setLEDcolor(index, GREEN, RED, BLUE);
			index++;
		}
		setLEDcolor(index,LEDcolors[3*j],LEDcolors[(3*j)+1],LEDcolors[(3*j)+2]);
		index++;
	}
	
	while(index < LED_NUMBER)
	{
		setLEDcolor(index, GREEN, RED, BLUE);
		index++;
	}
}
