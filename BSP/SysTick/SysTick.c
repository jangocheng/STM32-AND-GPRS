/**********************************************************************************
 * �ļ���  ��SysTick.c
 * ����    ��ͨ��ϵͳ�δ�ʱ��SysTick�ж�ʵ��nMs(n����)��nS(n��)����ʱ        
 * ʵ��ƽ̨��NiRen_TwoHeartϵͳ��
 * Ӳ�����ӣ�������ӵ�·       
 * ��汾  ��ST_v3.5
**********************************************************************************/

#include "SysTick.h"

static u32 SysTickDelayTime;
static __IO u32 TimingDelay;
__IO uint32_t gTimingDelay;

/*******************************************************************************
* ������  : SysTick_Init_Config
* ����    : ��ʼ��ϵͳ�δ�ʱ��SysTick
* ����    : ��
* ���    : ��
* ����    : �� 
* ˵��    : 1)��SystemFrequency / 1000		1ms�ж�һ��
*			2)��SystemFrequency / 100000	10us�ж�һ��
*			3)��SystemFrequency / 1000000	1us�ж�һ��
*			���㷽��:(SystemFrequency / Value)��ϵͳʱ�ӽ����ж�һ��
*******************************************************************************/
void SysTick_Init_Config(void)
{

	while(SysTick_Config(SystemCoreClock / 1000));	//��ʼ����ʹ��ϵͳ�δ�ʱ��,����1��ʾ��������̫��			 
	SysTick->CTRL &= ~ SysTick_CTRL_ENABLE_Msk;		//ʧ�ܵδ�ʱ�� 
}

/*******************************************************************************
* ������  : delay_ms
* ����    : ��ʱ����(n*20����)
* ����    : nm����ʱʱ��(n����)
* ���    : ��
* ����    : �� 
* ˵��    : ��
*******************************************************************************/
void delay_ms(u32 nms)
{ 
	nms = nms * 20;
	SysTickDelayTime = nms;		 
	SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;	//ʹ�ܵδ�ʱ�� 
	while(SysTickDelayTime != 0);				//�ȴ���ʱʱ�䵽
}

void _delay_Nms(u32 nTime)    
{
	  TimingDelay = nTime*1000;	

	// ʹ�ܵδ�ʱ��  
	SysTick->CTRL |=  SysTick_CTRL_ENABLE_Msk;

	while(TimingDelay != 0);
}

/*******************************************************************************
* ������  : Delay_S
* ����    : ��ʱ����(n��)
* ����    : nS����ʱʱ��(n��)
* ���    : ��
* ����    : �� 
* ˵��    : ��
*******************************************************************************/
void Delay_nS(u32 ns)
{ 
	SysTickDelayTime = ns*1000;		 
	SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;	//ʹ�ܵδ�ʱ�� 
	while(SysTickDelayTime != 0);					//�ȴ���ʱʱ�䵽
}


//void delay_us(__IO uint32_t n)
//{
//    SysTickDelayTime = n;
//    while(SysTickDelayTime != 0);
//}

/*******************************************************************************
* ������  : SysTickDelayTime_Counter
* ����    : ��ȡ���ĳ���
* ����    : ��
* ���    : ��
* ����    : �� 
* ˵��    : ��SysTick�жϳ���SysTick_Handler()����(stm32f10x_it.c)
*******************************************************************************/ 
void SysTickDelayTime_Counter(void)
{
	if (SysTickDelayTime > 0)
	{ 
		SysTickDelayTime--;
	}
}
