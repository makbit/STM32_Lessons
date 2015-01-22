#include <stm32f30x.h>
#include <stm32f30x_rcc.h>
#include <stm32f30x_gpio.h>
#include <stm32f30x_tim.h>


//============================================================================//
#ifdef SAMPLE5
#include <stm32f30x_exti.h>
#include <stm32f30x_syscfg.h>

void EXTI0_IRQHandler(void)
{
	if( (EXTI_GetITStatus(EXTI_Line0) == SET) )
	{
		EXTI_ClearITPendingBit( EXTI_Line0 );
		if( GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0) == SET )
			GPIO_SetBits( GPIOE, GPIO_Pin_8 );
		else
			GPIO_ResetBits( GPIOE, GPIO_Pin_8 );
	}
}

int main()
{
	GPIO_InitTypeDef gpio;
	EXTI_InitTypeDef exti;

	RCC_AHBPeriphClockCmd( RCC_AHBPeriph_GPIOA, ENABLE );  // push button port
	RCC_AHBPeriphClockCmd( RCC_AHBPeriph_GPIOE, ENABLE );  // led port
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE); // exti

	GPIO_StructInit( &gpio );

	gpio.GPIO_Mode = GPIO_Mode_IN;
	gpio.GPIO_Pin  = GPIO_Pin_0;
	GPIO_Init(GPIOA, &gpio);

	gpio.GPIO_Mode = GPIO_Mode_OUT;
	gpio.GPIO_Pin  = GPIO_Pin_8;
	GPIO_Init(GPIOE, &gpio);

	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource0);

	EXTI_StructInit( &exti );
	exti.EXTI_Line    = EXTI_Line0;
	exti.EXTI_Mode    = EXTI_Mode_Interrupt;
	exti.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
	exti.EXTI_LineCmd = ENABLE;
	EXTI_Init( &exti );

	NVIC_EnableIRQ( EXTI0_IRQn );
	NVIC_SetPriority( EXTI0_IRQn, 0x0F );

	while(1)
	{
	}
}
#endif

#ifdef SAMPLE1
///////////////////////////////////////////////////////////////////////////////
void Delay(unsigned long i)
{
	while( i-- > 0 ) ;
}

int main()
{
	GPIO_InitTypeDef gpio;

	//Reset and Clock Control (Enable PortE, LEDS 8-15)
	RCC_AHBPeriphClockCmd( RCC_AHBPeriph_GPIOE, ENABLE );

	// Init PortE (LEDS)
	GPIO_StructInit( &gpio );
	gpio.GPIO_Mode = GPIO_Mode_OUT;
	gpio.GPIO_Pin  = GPIO_Pin_8 | GPIO_Pin_9;
	GPIO_Init( GPIOE, &gpio );

	// Blinking LEDS
	while(1)
	{
		// On
		GPIO_SetBits( GPIOE, GPIO_Pin_8 );
		Delay(500000);
		GPIO_SetBits( GPIOE, GPIO_Pin_9 );
		Delay(500000);
		// All off
		GPIO_ResetBits( GPIOE, GPIO_Pin_8 |GPIO_Pin_9 );
		Delay(1000000);
	}
}
#endif

//============================================================================//

#ifdef SAMPLE2
///////////////////////////////////////////////////////////////////////////////
int main()
{
	GPIO_InitTypeDef gpio;

	uint32_t Leds = GPIO_Pin_8  | GPIO_Pin_9  |
	                GPIO_Pin_10 | GPIO_Pin_11 |
	                GPIO_Pin_12 | GPIO_Pin_13 |
	                GPIO_Pin_14 | GPIO_Pin_15;
	RCC_AHBPeriphClockCmd( RCC_AHBPeriph_GPIOA, ENABLE );
	RCC_AHBPeriphClockCmd( RCC_AHBPeriph_GPIOE, ENABLE );

	//---------------------------------------------------//
	// PortA - pushbutton
	GPIO_StructInit( &gpio );
	gpio.GPIO_Mode = GPIO_Mode_IN;
	gpio.GPIO_Pin  = GPIO_Pin_0;
	GPIO_Init( GPIOA, &gpio );

	//---------------------------------------------------//
	// Init PortE (LEDS)
	GPIO_StructInit( &gpio );
	gpio.GPIO_Mode = GPIO_Mode_OUT;
	gpio.GPIO_Pin  = Leds;
	GPIO_Init( GPIOE, &gpio );

	//---------------------------------------------------//
	while(1)
	{
		// Polling pushbutton state
		if( GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0) )
		{
			GPIO_SetBits( GPIOE, Leds );
		}
		else
		{
			GPIO_ResetBits( GPIOE, Leds );
		}
	}
}
#endif

//#ifdef SAMPLE3
//============================================================================//
static int i = 0;
void SysTick_Handler(void)
{
	// 1ms x 1000 == 1 sec
	if( ++i == 1000 )
	{
		uint8_t bit = GPIO_ReadOutputDataBit(GPIOE, GPIO_Pin_8);
		GPIO_WriteBit( GPIOE, GPIO_Pin_8, bit ? Bit_RESET : Bit_SET );
		i = 0;
	}
}

int main()
{
	GPIO_InitTypeDef gpio;
	RCC_AHBPeriphClockCmd( RCC_AHBPeriph_GPIOE, ENABLE );

	GPIO_StructInit( &gpio );
	gpio.GPIO_Mode = GPIO_Mode_OUT;
	gpio.GPIO_Pin  = GPIO_Pin_8;
	GPIO_Init( GPIOE, &gpio );

	// 1 ms ticks
	SysTick_Config(72000);
	while(1)
	{
	}
}
//#endif


#ifdef SAMPLE4
//============================================================================//
static int i = 0;
void TIM7_IRQHandler(void)
{
	if( TIM_GetITStatus(TIM7, TIM_IT_Update) != RESET )
	{
		TIM_ClearITPendingBit(TIM7, TIM_IT_Update);
		i = 1 - i;
		GPIO_WriteBit( GPIOE, GPIO_Pin_8, i ? Bit_SET : Bit_RESET );
	}
}

int main()
{
	GPIO_InitTypeDef gpio;
	RCC_AHBPeriphClockCmd( RCC_AHBPeriph_GPIOE, ENABLE );

	GPIO_StructInit( &gpio );
	gpio.GPIO_Mode = GPIO_Mode_OUT;
	gpio.GPIO_Pin  = GPIO_Pin_8 | GPIO_Pin_9;
	GPIO_Init( GPIOE, &gpio );
	GPIO_SetBits( GPIOE, GPIO_Pin_9 );
	//---------------------------------//
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM7, ENABLE);
	TIM_TimeBaseInitTypeDef timer;
	TIM_TimeBaseStructInit(&timer);
	timer.TIM_Prescaler = 65000 - 1;
	timer.TIM_Period = 500;
	TIM_TimeBaseInit(TIM7, &timer);

	TIM_ITConfig(TIM7, TIM_IT_Update, ENABLE);
	TIM_Cmd(TIM7, ENABLE);
	NVIC_EnableIRQ(TIM7_IRQn);
	//---------------------------------//
	while(1)
	{
	}
}
#endif
