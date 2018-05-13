/**********************************************************************************
   STM32F103C8T6  	 
 * Ӳ������˵��
	 ʹ�õ�Ƭ����2��GPRSģ��ͨ��  ע��ʹ�ô���2���Ա������غ�ͨ�Ų����ͻ
	 STM32      GPRSģ��
	 ����ӽ��߷���:               mini���ӽ��߷���:
     PA3 (RXD2)->U_RX               PA3 (RXD2)->U_TX
	 PA2 (TXD2)->U_TX               PA2 (TXD2)->U_RX
	 GND	   ->GND                GND	   ->GND
	 
	 PA9(TXD1)--->������Ϣ�˿�
	 PA10(RXD1)-->������Ϣ�˿�
��ƣ�
(1)LED0-7���õ�����Ϊ:PB4-7 PB12-15
(2)KEY0-3���õ�����Ϊ:PA4-7
  
**********************************************************************************/
#include "stm32f10x.h"
#include "usart.h"
#include "Led.h"
#include "SysTick.h"
#include "timer.h"
#include "string.h"
#include "key.h"
#include "GA6_module_errors.h"
#include <stdio.h>
#include "DHT11.h"
//#include "time_delay.h"

#define Buf2_Max 	  70 //����2���泤��
#define Buf1_Max 	  70 //����2���泤��
#define STABLE_TIMES  10  //�ȴ�ϵͳ�ϵ����ȶ�

/*************	���س�������	**************/

/*************  ���ر�������	**************/
char Uart2_Buf[Buf2_Max];//����2���ջ���
char Uart1_Buf[Buf1_Max];//����1���ջ���
char data_from_uart1[20];
u8 First_Int = 0;
u8 UART1_First_Int = 0;
char error_result[20];
char GetTemp[4];
char Gethumi[4];

static char  *phone_num = "AT+CMGS=\"13067802058\""; //�޸���������޸ĵ绰����

/*************	���غ�������	**************/
void CLR_Buf(void);     //�������2���ջ���
void CLR_UART1_Buffer(void);  //�������1���ջ��� 
u8 Wait_CREG(u8 query_times);    //�ȴ�ģ��ע��ɹ�
u8 Find(char *a);
void Show_Dth11(void);
u8 UART1_Send_QUERY_Command(u8 query_times);
//void CLR_UART1_Buf(void);

/*************  �ⲿ�����ͱ�������*****************/


int check_status(void)
{
	int ret;
	
	ret = UART2_Send_AT_Command("AT","OK",3,50);//����ͨ���Ƿ�ɹ�
	if(ret == 0)
	{
		return COMMUNITE_ERROR;
	}
	
	ret = UART2_Send_AT_Command("AT+CPIN?","READY",2,50);//��ѯ���Ƿ����
	if(ret == 0)
	{
		return NO_SIM_CARD_ERROR;
	}
	
	ret = Wait_CREG(3);//��ѯ���Ƿ�ע�ᵽ����
	if(ret == 0)
	{
		return SIM_CARD_NO_REG_ERROR;
	}
	return 1;
}

/***************************************************************
ע����Ȼ����Է�������ֵ����ȷ����������һ������ָ�����ʧ���ˡ�
****************************************************************/
int send_text_message(char *content)
{
	u8 ret;
	char end_char[2];
	
	end_char[0] = 0x1A;//�����ַ�
	end_char[1] = '\0';
	
	//���ô洢λ��
	ret = UART2_Send_AT_Command("AT+CPMS=\"SM\",\"ME\",\"SM\"","OK",3,100);
	if(ret == 0)
	{
		return AT_CPMS_ERROR;
	}
	
	ret = UART2_Send_AT_Command("AT+CMGF=1","OK",3,50);//����ΪTEXTģʽ
	if(ret == 0)
	{
		return AT_CMGF_ERROR;
	}
	
	ret = UART2_Send_AT_Command("AT+CSCS=\"GSM\"","OK",3,50);//�����ַ���ʽ
	if(ret == 0)
	{
		return AT_CSCS_ERROR;
	}
	
	ret = UART2_Send_AT_Command(phone_num,">",3,50);//���������˵ĵ绰����
	if(ret == 0)
	{
		return AT_CMGS_ERROR;
	}
	
	UART2_SendString(content);
	ret = UART2_Send_AT_Command_End(end_char,"OK",1,250);//���ͽ��������ȴ�����ok,�ȴ�5S��һ�Σ���Ϊ���ųɹ����͵�״̬ʱ��Ƚϳ�
	if(ret == 0)
	{
		return END_CHAR_ERROR;
	}
	
	return 1;
}

/*******************************************************************************
* ������ : main 
* ����   : ������
* ����   : 
* ���   : 
* ����   : 
* ע��   : ����2������GA6ģ��ͨ�ţ�����1���ڴ��ڵ��ԣ�
*******************************************************************************/
int main(void)
{
//	u16 key_value;
	u8  i;
	u8  run_led_flag = 0;
	int  ret;
	u8 buffer[5];
	char str_hum[30];
	char str_temp[10];
	double hum;
  double temp;
	SysTick_Init_Config();   //ϵͳ�δ�ʱ�ӳ�ʼ��
	GPIO_Config();           //GPIO��ʼ��
	Key_GPIO_Config();
//	USART2_Init_Config(115200);  //����2��ʼ��
	USART2_Init_Config(9600);  //����2��ʼ��
	Timer2_Init_Config();        //��ʱ��2��ʼ��
//	delay_init();
	USART1_Init_Config(9600);//UART1�������ڵ�����Ϣ
	
	UART1_SendString("ϵͳ����.......................\r\n");
	
	for(i = 0;i < STABLE_TIMES;i++)
	{
		delay_ms(50);
	}
	
	memset(error_result,'\0',20);
	
	while(1)
	{ 		
//		UART1_SendString("ϵͳ����1.......................\r\n");
		if (dht11_read_data(buffer) == 0)
		{
				hum = buffer[0] + buffer[1] / 10.0;
				temp = buffer[2] + buffer[3] / 10.0;
		}
		
//		str_hum = (char)hum;
//		sprintf(str_hum, "%6f%6f", hum, temp);
//		sprintf(str_temp, "%f", temp);
		
//		UART1_SendString(str_hum);
//		UART1_SendString("\r\n");
		
		UART1_Send_QUERY_Command(2);
		
		sprintf(str_hum, "%6f%6f%s", hum, temp, data_from_uart1);
//		sprintf(str_hum, "%s", data_from_uart1);
		
		if(run_led_flag == 0)
		{
			LED1_ON();
//			UART1_SendString("LED0_ON\r\n");
			run_led_flag = 1;
		}
		else
		{
			LED1_OFF();
//			UART1_SendString("LED0_OFF\r\n");
			run_led_flag = 0;
		}
//		key_value = Key_Down_Scan();
//		switch (key_value)
//		{
//			case (0x0001 << 5):
//			{
				ret = check_status();
				if(ret == 1)
				{
//					ret = send_text_message(str);//����TEXT����
					ret = send_text_message(str_hum);
				}
				if(ret == 1)
				{
//					sprintf(error_result,"�ɹ�����TEXT����\r\n");
					UART1_SendString(error_result);
					LED2_ON();
				}
				else
				{
					sprintf(error_result,"������� : %d\r\n",ret);
					UART1_SendString(error_result);
					LED2_OFF();
					
				}
								
//				break;
//			}
//			
//			default: 
//				break;
			
//		}
//			CLR_UART1_Buffer();
			delay_ms(500);
	}
}

/*******************************************************************************
* ������  : USART2_IRQHandler
* ����    : ����1�жϷ������
* ����    : ��
* ����    : �� 
* ˵��    : 
*******************************************************************************/
void USART2_IRQHandler(void)                	
{
	u8 Res=0;
	Res = USART_ReceiveData(USART2);
	Uart2_Buf[First_Int] = Res;  	  //�����յ����ַ����浽������
	First_Int++;                	  //����ָ������ƶ�
	if(First_Int > Buf2_Max)       	  //���������,������ָ��ָ�򻺴���׵�ַ
	{
		First_Int = 0;
	}
} 	

/*******************************************************************************
* ������  : TIM2_IRQHandler
* ����    : ��ʱ��2�ж϶Ϸ�����
* ����    : ��
* ���    : ��
* ����    : �� 
* ˵��    : ��
*******************************************************************************/
void TIM2_IRQHandler(void)   //TIM2�ж�
{
	static u8 flag =1;

	if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)  //���TIM2�����жϷ������
	{
		
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);  //���TIM2�����жϱ�־ 
	
		if(flag)
		{
			//LED4_ON(); 
			flag=0;
		}
		else
		{
			//LED4_OFF(); 
			flag=1;
		}
	}	
}

/*******************************************************************************
* ������ : CLR_Buf
* ����   : �������2��������
* ����   : 
* ���   : 
* ����   : 
* ע��   : 
*******************************************************************************/
void CLR_Buf(void)
{
	u16 k;
	for(k=0;k<Buf2_Max;k++)      //��������������
	{
		Uart2_Buf[k] = 0x00;
	}
    First_Int = 0;              //�����ַ�������ʼ�洢λ��
}

/*******************************************************************************
* ������ : Wait_CREG
* ����   : �ȴ�ģ��ע��ɹ�
* ����   : 
* ���   : 
* ����   : 
* ע��   : 
*******************************************************************************/
u8 Wait_CREG(u8 query_times)
{
	u8 i;
	u8 k;
	u8 j;
	i = 0;
	CLR_Buf();
	while(i == 0)        			
	{

		UART2_Send_Command("AT+CREG?");
		delay_ms(100); 
		
		for(k=0;k<Buf2_Max;k++)      			
		{
			if((Uart2_Buf[k] == '+')&&(Uart2_Buf[k+1] == 'C')&&(Uart2_Buf[k+2] == 'R')&&(Uart2_Buf[k+3] == 'E')&&(Uart2_Buf[k+4] == 'G')&&(Uart2_Buf[k+5] == ':'))
			{
					 
				if((Uart2_Buf[k+7] == '1')&&((Uart2_Buf[k+9] == '1')||(Uart2_Buf[k+9] == '5')))
				{
					i = 1;
					return 1;
				}
				
			}
		}
		j++;
		if(j > query_times)
		{
			return 0;
		}
		
	}
	return 0;
}

/*******************************************************************************
* ������ : Find
* ����   : �жϻ������Ƿ���ָ�����ַ���
* ����   : 
* ���   : 
* ����   : unsigned char:1 �ҵ�ָ���ַ���0 δ�ҵ�ָ���ַ� 
* ע��   : 
*******************************************************************************/

u8 Find(char *a)
{ 
	if(strstr(Uart2_Buf, a)!=NULL)
	{
		return 1;
	}	
	else
	{
		return 0;
	}
		
}

void USART1_IRQHandler(void)                	
{
	u8 Res=0;
	Res = USART_ReceiveData(USART1);
	Uart1_Buf[UART1_First_Int] = Res;	
//	Uart2_Buf[First_Int] = Res;  	  //�����յ����ַ����浽������
	UART1_First_Int++;                	  //����ָ������ƶ�
	if(UART1_First_Int > Buf1_Max)       	  //���������,������ָ��ָ�򻺴���׵�ַ
	{
		UART1_First_Int = 0;
	}
} 

/*******************************************************************************
* ������ : CLR_UART1_Buf
* ����   : �������2��������
* ����   : 
* ���   : 
* ����   : 
* ע��   : 
*******************************************************************************/
void CLR_UART1_Buf(void)
{
	u16 k;
	for(k=0;k<Buf1_Max;k++)      //��������������
	{
		Uart1_Buf[k] = 0x00;
	}
    UART1_First_Int = 0;              //�����ַ�������ʼ�洢λ��
}

/*******************************************************************************
* ������ : Find
* ����   : �жϻ������Ƿ���ָ�����ַ���
* ����   : 
* ���   : 
* ����   : unsigned char:1 �ҵ�ָ���ַ���0 δ�ҵ�ָ���ַ� 
* ע��   : 
*******************************************************************************/

u8 UART1_Find(char *a)
{ 
	if(strstr(Uart1_Buf, a)!=NULL)
	{
		return 1;
	}	
	else
	{
		return 0;
	}
		
}


//�����Ƕȴ���������
u8 UART1_Send_QUERY_Command(u8 query_times)         
{
	u8 i;
	u8 k;
	u8 j;
	i = 0;
	CLR_UART1_Buffer();
//	while(i == 0)        			
//	{

		UART1_SendString("AT+V\r\n");
		delay_ms(200); 
//		strcpy(data_from_uart1, Uart1_Buf);
		for(k=0; k<20; k++){
			data_from_uart1[k] = Uart1_Buf[k];
//			data_from_uart1[k] = Uart2_Buf[k];
		}
		
//		UART1_SendString("׼���������ݣ�");
//		UART1_SendString(Uart1_Buf);
//		for(k=0;k<Buf1_Max;k++)      			
//		{
//			
//			
//			if((Uart1_Buf[k] == '+')&&(Uart2_Buf[k+1] == 'C')&&(Uart2_Buf[k+2] == 'R')&&(Uart2_Buf[k+3] == 'E')&&(Uart2_Buf[k+4] == 'G')&&(Uart2_Buf[k+5] == ':'))
//			{
//					 
//				if((Uart2_Buf[k+7] == '1')&&((Uart2_Buf[k+9] == '1')||(Uart2_Buf[k+9] == '5')))
//				{
//					i = 1;
//					return 1;
//				}
//				
//			}
//		}
//		j++;
//		if(j > query_times)
//		{
//			return 0;
//		}
		
//	}
	return 0;
}

/*******************************************************************************
* ������ : CLR_Buf
* ����   : �������2��������
* ����   : 
* ���   : 
* ����   : 
* ע��   : 
*******************************************************************************/
void CLR_UART1_Buffer(void)
{
	u16 k;
	for(k=0;k<Buf1_Max;k++)      //��������������
	{
		Uart1_Buf[k] = 0x00;
	}
    UART1_First_Int = 0;              //�����ַ�������ʼ�洢λ��
}

