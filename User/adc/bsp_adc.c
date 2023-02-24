#include "bsp_adc.h"

/* Rheostat�������� */

static void Rheostat_ADC_GPIO_Config(void)
{
		GPIO_InitTypeDef GPIO_InitStructure;
	
	// ʹ�� GPIO ʱ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
		
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;               //PB0
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;	    	//ģ��
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ; 		//������������
	GPIO_Init(GPIOB, &GPIO_InitStructure);		
}



static void Rheostat_ADC_Mode_Config(void)
{
	ADC_CommonInitTypeDef ADC_CommonInitStructure;
	ADC_InitTypeDef ADC_InitStructure;	
	
	// ����ADCʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1 , ENABLE);


  // -------------------ADC Common �ṹ�� ���� ��ʼ��------------------------	
	
	
  // ����ADCģʽ
  ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;
  // ʱ��Ϊfpclk x��Ƶ	
  ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div4;
  // ��ֹDMAֱ�ӷ���ģʽ	
  ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;
  // ����ʱ����	
  ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_20Cycles;  
  ADC_CommonInit(&ADC_CommonInitStructure);
	
	
	
  // -------------------ADC Init �ṹ�� ���� ��ʼ��--------------------------	
	ADC_StructInit(&ADC_InitStructure);

	ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;  // ADC �ֱ���
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;   // ��ֹɨ��ģʽ����ͨ���ɼ�����Ҫ	
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;   // ����ת��	
	ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;  //��ֹ�ⲿ���ش���
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T1_CC1;  //�ⲿ����ͨ����������ʹ�������������ֵ��㸳ֵ����
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;  //�����Ҷ���	
	ADC_InitStructure.ADC_NbrOfConversion = 1;          //ת��ͨ�� 1��                            
	ADC_Init(RHEOSTAT_ADC, &ADC_InitStructure);
  
  
    // ���� ADC1��ͨ��8 ת��˳��Ϊ1����һ��ת��������ʱ��Ϊ3��ʱ������
  ADC_RegularChannelConfig(ADC1, ADC_Channel_8, 1, ADC_SampleTime_3Cycles);
  
	// ADC ת�����������жϣ����жϷ�������ж�ȡת��ֵ
	ADC_ITConfig(ADC1, ADC_IT_EOC, ENABLE);
  
	// ADC ת�����������жϣ����жϷ�������ж�ȡת��ֵ
	ADC_ITConfig(ADC1, ADC_IT_EOC, ENABLE);
	
  // ʹ��ADC
  ADC_Cmd(ADC1, ENABLE);  
  
  //��ʼADCת�����������
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
// ADC ת������жϷ������
void ADC_IRQHandler(void)
{
	if(ADC_GetITStatus(RHEOSTAT_ADC,ADC_IT_EOC)==SET)
	{
  // ��ȡADC��ת��ֵ
		ADC_ConvertedValue = ADC_GetConversionValue(RHEOSTAT_ADC);

	}
	//���ת����ɱ�־λ
	ADC_ClearITPendingBit(RHEOSTAT_ADC,ADC_IT_EOC);

}	

