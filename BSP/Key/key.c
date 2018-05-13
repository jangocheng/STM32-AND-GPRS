/**********************************************************************************
 * �ļ���  ��key.c
 * ����    ������ɨ��(ϵͳ�δ�ʱ��SysTickʵ��ɨ����ʱ)
 * ʵ��ƽ̨��NiRen_TwoHeartϵͳ��
 * Ӳ�����ӣ�  PB1 -> KEY1     
 *             PB2 -> KEY2       
 * ��汾  ��ST_v3.5
**********************************************************************************/

#include "key.h" 
#include "SysTick.h"

/*******************************************************************************
* ������  : Key_GPIO_Config
* ����    : KEY IO����
* ����    : ��
* ���    : ��
* ����    : �� 
* ˵��    : KEY���õ�����Ϊ:PA4-7
*******************************************************************************/
void Key_GPIO_Config(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;				//����һ��GPIO_InitTypeDef���͵�GPIO��ʼ���ṹ��
	
	RCC_APB2PeriphClockCmd(KEY_RCC, ENABLE);			//ʹ��GPIOA������ʱ��	
	
	GPIO_InitStructure.GPIO_Pin = KEY0 | KEY1 | KEY2 | KEY3;			//ѡ��Ҫ��ʼ����GPIOA����(PB1,PB2)
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;		//�������Ź���ģʽΪ�������� 		
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//������������������Ϊ50MHz
	GPIO_Init(KEY_PORT, &GPIO_InitStructure);			//���ÿ⺯���е�GPIO��ʼ����������ʼ��GPIOA�е�PB1,PB2����
}

/*******************************************************************************
* ������  : Key_Detection
* ����    : �������¼��
* ����    : GPIOx��������Ӧ��GPIO��GPIO_Pin����Ӧ�����˿�
* ���    : ��
* ����    : KEY_DOWN(0):��Ӧ�������£�KEY_UP(1):��Ӧ����û����
* ˵��    : KEY���õ�����Ϊ:PA4-7
*******************************************************************************/
u8 Key_Down_Detection(GPIO_TypeDef* GPIOx, u16 GPIO_Pin)
{			
	if(GPIO_ReadInputDataBit(GPIOx,GPIO_Pin) == KEY_DOWN)	//����Ƿ��а������� 
	{	   
		delay_ms(10);	//��ʱ����		
		if(GPIO_ReadInputDataBit(GPIOx,GPIO_Pin) == KEY_DOWN)	//����Ƿ��а�������   
		{	 
			while(GPIO_ReadInputDataBit(GPIOx,GPIO_Pin) == KEY_DOWN);	//�ȴ������ͷ�  
			return KEY_DOWN;	 
		}
		else
		{
			return KEY_UP;
		}
	}
	else
	{
		return KEY_UP;
	}
}

/*******************************************************************************
* ������  : Key_Down_Scan
* ����    : ��������ɨ��
* ����    : ��
* ���    : 0��û�а������£�1����������
* ����    : �� 
* ˵��    : KEY���õ�����Ϊ:PA4-7
*******************************************************************************/
u16 Key_Down_Scan(void)
{
	u16 downflag = 0;
	u16 value    = 0;

  	downflag = GPIO_ReadInputData(KEY_PORT);
	value    = (~downflag) & (0x0001 << 4);
	if((~downflag) & (0x0001 << 4))	//����Ƿ��а������� 
	{	   
		delay_ms(1);	//��ʱ����	
		downflag = GPIO_ReadInputData(KEY_PORT);	
		if((~downflag) & (0x0001 << 4))	//����Ƿ��а�������   
		{	 
			value = (~downflag) & (0x0001 << 4);
			while((~GPIO_ReadInputData(KEY_PORT))& (0x0001 << 4));//�ȴ�����̧��
		}	
	}
	else if((~downflag) & (0x0001 << 5))	//����Ƿ��а������� 
	{	   
		delay_ms(1);	//��ʱ����	
		downflag = GPIO_ReadInputData(KEY_PORT);	
		if((~downflag) & (0x0001 << 5))	//����Ƿ��а�������   
		{	 
			value = (~downflag) & (0x0001 << 5);
			while((~GPIO_ReadInputData(KEY_PORT))& (0x0001 << 5));//�ȴ�����̧��
		}		
	}
	else if((~downflag) & (0x0001 << 6))	//����Ƿ��а������� 
	{	   
		delay_ms(1);	//��ʱ����	
		downflag = GPIO_ReadInputData(KEY_PORT);	
		if((~downflag) & (0x0001 << 6))	//����Ƿ��а�������   
		{	 
			value = (~downflag) & (0x0001 << 6);
			while((~GPIO_ReadInputData(KEY_PORT))& (0x0001 << 6));//�ȴ�����̧��
		}		
	}	
	else if((~downflag) & (0x0001 << 7))	//����Ƿ��а������� 
	{	   
		delay_ms(1);	//��ʱ����	
		downflag = GPIO_ReadInputData(KEY_PORT);	
		if((~downflag) & (0x0001 << 7))	//����Ƿ��а�������   
		{	 
			value = (~downflag) & (0x0001 << 7);
			while((~GPIO_ReadInputData(KEY_PORT))& (0x0001 << 7));//�ȴ�����̧��
		}		
	}	
	
	return value;
}
