#include "RGB_library.h"

static uint8_t LEDbuffer[LED_BUFFER_SIZE];
TIM_HandleTypeDef htim2;
TIM_OC_InitTypeDef sConfig;
GPIO_InitTypeDef GPIO_InitStruct;
DMA_HandleTypeDef hdma_tim2_ch1;
UART_HandleTypeDef huart1;

void HAL_TIM_MspPost_Init(TIM_HandleTypeDef* htim)
{
	/* Configure TIM1_Channel1 in output, push-pull & alternate function mode */
  GPIO_InitTypeDef GPIO_InitStruct;
  if(htim->Instance==TIM2)
  {  
    GPIO_InitStruct.Pin = GPIO_PIN_0;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
		HAL_GPIO_Init(TIMx_GPIO_CHANNEL1_PORT, &GPIO_InitStruct);
  }
}

void HAL_TIM_PWM_MspInit(TIM_HandleTypeDef *htim)
{
	/* TIMx clock enable */
	TIMx_CLK_ENABLE();

	/* Enable GPIO Channel Clock */
	TIMx_CHANNEL1_GPIO_CLK_ENABLE();

	/* Enable DMA clock */
	DMAx_CLK_ENABLE();

	/*initialize dma channel_1-------------------------*/
	hdma_tim2_ch1.Instance = TIMx_CC1_DMA_INST;
  hdma_tim2_ch1.Init.Direction = DMA_MEMORY_TO_PERIPH;
  hdma_tim2_ch1.Init.PeriphInc = DMA_PINC_DISABLE;
  hdma_tim2_ch1.Init.MemInc = DMA_MINC_ENABLE;
  hdma_tim2_ch1.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
  hdma_tim2_ch1.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
  hdma_tim2_ch1.Init.Mode = DMA_CIRCULAR;
  hdma_tim2_ch1.Init.Priority = DMA_PRIORITY_HIGH;
	HAL_DMA_Init(&hdma_tim2_ch1);

	/* Set hdma_tim instance */
	hdma_tim2_ch1.Instance = TIMx_CC1_DMA_INST;

	/* Link hdma_tim to hdma[TIM_DMA_ID_CC1] (channel1) */
	__HAL_LINKDMA(htim, hdma[TIM_DMA_ID_CC1], hdma_tim2_ch1);

	/* Initialize TIMx DMA handle */
	HAL_DMA_Init(htim->hdma[TIM_DMA_ID_CC1]);

	/* NVIC configuration for DMA transfer complete interrupt */
	HAL_NVIC_SetPriority(TIMx_DMA_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(TIMx_DMA_IRQn);
}

void ws2812_init(void)
{
	fillBufferBlack();
	
	/*initialize the timer--------------------*/
  TIM_OC_InitTypeDef sConfigOC;
	
  htim2.Instance = TIMx;
  htim2.Init.Prescaler = (uint32_t)((SystemCoreClock / TIMER_CLOCK_FREQ) - 1);
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = TIMER_PERIOD - 1;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	htim2.Init.RepetitionCounter = LED_BUFFER_SIZE + 1; 
	htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  HAL_TIM_PWM_Init(&htim2);
	
	/*inittialize pwm_channel_1---------------*/
	sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
	sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
	HAL_TIM_PWM_ConfigChannel(&htim2, &sConfig, TIM_CHANNEL_x);
	
	HAL_TIM_MspPost_Init(&htim2);
		
	/*initialize huart_1----------------------*/
	huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
	HAL_UART_Init(&huart1);
	
	HAL_TIM_PWM_MspInit(&htim2);
	
	HAL_TIM_PWM_Start_DMA(&htim2, TIM_CHANNEL_x, (uint32_t *) LEDbuffer,LED_BUFFER_SIZE);
}

void ws2812_update(void)
{
	HAL_TIM_PWM_ConfigChannel(&htim2, &sConfig, TIM_CHANNEL_x);
	HAL_TIM_PWM_Start_DMA(&htim2, TIM_CHANNEL_x, (uint32_t *) LEDbuffer,LED_BUFFER_SIZE);
}

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