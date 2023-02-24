#include "bsp_adc.h"

/* Rheostat：变阻器 */

static void Rheostat_ADC_GPIO_Config(void)
{
		GPIO_InitTypeDef GPIO_InitStructure;
	
	// 使能 GPIO 时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
		
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;               //PB0
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;	    	//模拟
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ; 		//不上拉不下拉
	GPIO_Init(GPIOB, &GPIO_InitStructure);		
}



static void Rheostat_ADC_Mode_Config(void)
{
	ADC_CommonInitTypeDef ADC_CommonInitStructure;
	ADC_InitTypeDef ADC_InitStructure;	
	
	// 开启ADC时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1 , ENABLE);


  // -------------------ADC Common 结构体 参数 初始化------------------------	
	
	
  // 独立ADC模式
  ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;
  // 时钟为fpclk x分频	
  ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div4;
  // 禁止DMA直接访问模式	
  ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;
  // 采样时间间隔	
  ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_20Cycles;  
  ADC_CommonInit(&ADC_CommonInitStructure);
	
	
	
  // -------------------ADC Init 结构体 参数 初始化--------------------------	
	ADC_StructInit(&ADC_InitStructure);

	ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;  // ADC 分辨率
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;   // 禁止扫描模式，多通道采集才需要	
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;   // 连续转换	
	ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;  //禁止外部边沿触发
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T1_CC1;  //外部触发通道，本例子使用软件触发，此值随便赋值即可
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;  //数据右对齐	
	ADC_InitStructure.ADC_NbrOfConversion = 1;          //转换通道 1个                            
	ADC_Init(RHEOSTAT_ADC, &ADC_InitStructure);
  
  
    // 配置 ADC1的通道8 转换顺序为1，第一个转换，采样时间为3个时钟周期
  ADC_RegularChannelConfig(ADC1, ADC_Channel_8, 1, ADC_SampleTime_3Cycles);
  
	// ADC 转换结束产生中断，在中断服务程序中读取转换值
	ADC_ITConfig(ADC1, ADC_IT_EOC, ENABLE);
  
	// ADC 转换结束产生中断，在中断服务程序中读取转换值
	ADC_ITConfig(ADC1, ADC_IT_EOC, ENABLE);
	
  // 使能ADC
  ADC_Cmd(ADC1, ENABLE);  
  
  //开始ADC转换，软件触发
  ADC_SoftwareStartConv(ADC1);


}



static void Rheostat_ADC_NVIC_Config()
{
  NVIC_InitTypeDef NVIC_InitStructure;
  NVIC_InitStructure.NVIC_IRQChannel = ADC_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	
  NVIC_Init(&NVIC_InitStructure);	

}



void Rheostat_Init(void)
{
	Rheostat_ADC_GPIO_Config();
	Rheostat_ADC_Mode_Config();
	Rheostat_ADC_NVIC_Config();
}


extern __IO uint16_t ADC_ConvertedValue;
// ADC 转换完成中断服务程序
void ADC_IRQHandler(void)
{
	if(ADC_GetITStatus(RHEOSTAT_ADC,ADC_IT_EOC)==SET)
	{
  // 读取ADC的转换值
		ADC_ConvertedValue = ADC_GetConversionValue(RHEOSTAT_ADC);

	}
	//清除转换完成标志位
	ADC_ClearITPendingBit(RHEOSTAT_ADC,ADC_IT_EOC);

}	

