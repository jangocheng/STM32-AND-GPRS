/**********************************************************************************
 * �ļ���  ��usart.c
 * ����    ��usart1 Ӧ�ú�����          
 * ʵ��ƽ̨��NiRen_TwoHeartϵͳ��
 * Ӳ�����ӣ�TXD(PB9)  -> �ⲿ����RXD     
 *           RXD(PB10) -> �ⲿ����TXD      
 *           GND	   -> �ⲿ����GND 
 * ��汾  ��ST_v3.5
**********************************************************************************/

#include "usart.h"
#include "SysTick.h"
	
//vu8 Usart1_R_Buff[USART1_REC_MAXLEN];	//����1���ݽ��ջ����� 
vu8 Usart1_R_State=0;					//����1����״̬
vu16 Usart1_R_Count=0;					//��ǰ�������ݵ��ֽ��� 	  
//u8 UART_First_Int = 0;

u16 USART_RX_STA=0;       //����״̬���
//u8 USART_RX_BUF[USART_REC_LEN];     //���ջ���,���USART_REC_LEN���ֽ�.
extern void CLR_Buf(void);
/*******************************************************************************
* ������  : USART1_Init_Config
* ����    : USART1��ʼ������
* ����    : bound��������(���ã�2400��4800��9600��19200��38400��115200��)
* ���    : ��
* ����    : �� 
* ˵��    : ��
*******************************************************************************/
void USART1_Init_Config(u32 bound)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef  NVIC_InitStructure;

	/*ʹ��USART1��GPIOA����ʱ��*/  
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA, ENABLE);	

	/*��λ����1*/
	USART_DeInit(USART1);  

	/*USART1_GPIO��ʼ������*/ 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;			//USART1_TXD(PA.9)     
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;		//�����������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//������������������Ϊ50MHz
	GPIO_Init(GPIOA, &GPIO_InitStructure);				//���ÿ⺯���е�GPIO��ʼ����������ʼ��USART1_TXD(PA.9)  


	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;				//USART1_RXD(PA.10)
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;	//��������
	GPIO_Init(GPIOA, &GPIO_InitStructure);					//���ÿ⺯���е�GPIO��ʼ����������ʼ��USART1_RXD(PA.10)


	/*USART1 ��ʼ������*/
	USART_InitStructure.USART_BaudRate = bound;										//���ò�����
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;						//8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;							//1��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;								//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;					//����ģʽ����Ϊ�շ�ģʽ
	USART_Init(USART1, &USART_InitStructure);										//��ʼ������1

	/*Usart1 NVIC����*/
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2;	//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);							//����ָ���Ĳ�����ʼ��VIC�Ĵ��� 

	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);			//ʹ�ܴ���1�����ж�

	USART_Cmd(USART1, ENABLE);                    			//ʹ�ܴ��� 
	USART_ClearFlag(USART1, USART_FLAG_TC);					//���������ɱ�־
}


/*******************************************************************************
* ������  : UART1_SendString
* ����    : USART1�����ַ���
* ����    : *s�ַ���ָ��
* ���    : ��
* ����    : �� 
* ˵��    : ��
*******************************************************************************/
void UART1_SendString(char* s)
{
	while(*s)//����ַ���������
	{
		while(USART_GetFlagStatus(USART1, USART_FLAG_TC)==RESET); 
		USART_SendData(USART1 ,*s++);//���͵�ǰ�ַ�
	}
}


/*******************************************************************************
* ������  : USART1_IRQHandler
* ����    : ����1�жϷ������
* ����    : ��
* ����    : �� 
* ˵��    : 1)��ֻ������USART1�жϽ��գ�δ����USART1�жϷ��͡�
*           2)�����յ�0x0d 0x0a(�س���"\r\n")����֡���ݽ������
*******************************************************************************/
//void USART1_IRQHandler(void)                	
//{
//	u8 Res;
//	u8 t;
//	u8 len;	
//	u8 forward = 0x01;
//	u8 back = 0x02;
//	u8 turn_left = 0x03;
//	u8 turn_right = 0x04;
//	u8 fertilize = 0x05;
//	u8 fertilize_stop = 0x06;
//	u8 stop = 0x07;
//	
//#ifdef OS_TICKS_PER_SEC	 	//���ʱ�ӽ�����������,˵��Ҫʹ��ucosII��.
//	OSIntEnter();    
//#endif
//	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //�����ж�(���յ������ݱ�����0x0d 0x0a��β)
//		{
//		Res =USART_ReceiveData(USART1);//(USART1->DR);	//��ȡ���յ�������
//		
//			if((USART_RX_STA&0x8000)==0)//����δ���
//				{
//				if(USART_RX_STA&0x4000)//���յ���0x0d
//					{
//					if(Res!=0x0a)USART_RX_STA=0;//���մ���,���¿�ʼ
//					else USART_RX_STA|=0x8000;	//��������� 
//					}
//				else //��û�յ�0X0D
//					{	
//					if(Res==0x0d)USART_RX_STA|=0x4000;
//					else
//						{
//						USART_RX_BUF[USART_RX_STA&0X3FFF]=Res ;
//						USART_RX_STA++;
//						if(USART_RX_STA>(USART_REC_LEN-1))USART_RX_STA=0;//�������ݴ���,���¿�ʼ����	  
//						}		 
//					}
//				}
//			if(USART_RX_STA&0x8000)
//			{	
//	//			printf("\r\n send the message_1 \r\n\r\n");		
////				enableWheels();	
//				len=USART_RX_STA&0x3f;//�õ��˴ν��յ������ݳ���
////				printf("\r\n the message len you send is: \r\n\r\n");
////				printf("%s, %c\n", "len: ", len);
//				if(USART_RX_BUF[0]==forward){
//					UART1_SendString("forward");
////					printf("%s\n", "0x01");	
//				}else if(USART_RX_BUF[0]==back){
//					UART1_SendString("back");
////					printf("%s\n", "0x02");		
//				}else if(USART_RX_BUF[0]==turn_left){
//					UART1_SendString("left side");
////					printf("%s\n", "0x03");				
//				}else if(USART_RX_BUF[0]==turn_right){
////					rightSidesway();
////					printf("%s\n", "0x04");
//				}else if(USART_RX_BUF[0]==fertilize){
////						CLOSE_PUMP = 1;
////					printf("%s\n", "0x05");
//				}else if(USART_RX_BUF[0]==fertilize_stop){
////					printf("%s\n", "0x06");
////						CLOSE_PUMP = 0;
//				}else if(USART_RX_BUF[0]==stop){
////					printf("%s\n", "0x06");
////						disenableWheels();
//				}
////				for(t=0;t<len;t++)
////				{
////					USART_SendData(USART1, USART_RX_BUF[t]);//�򴮿�1��������
////					while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//�ȴ����ͽ���
////				}
////				printf("\r\n\r\n");//���뻻��
//				USART_RX_STA=0;
//			}else{
////				disenableWheels();
//			}	
//				
//     } 
//#ifdef OS_TICKS_PER_SEC	 	//���ʱ�ӽ�����������,˵��Ҫʹ��ucosII��.
//	OSIntExit();  											 
//#endif
//} 	

/*******************************************************************************
* ������  : USART1_Init_Config
* ����    : USART1��ʼ������
* ����    : bound��������(���ã�2400��4800��9600��19200��38400��115200��)
* ���    : ��
* ����    : �� 
* ˵��    : ��
*******************************************************************************/
void USART2_Init_Config(u32 bound)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef  NVIC_InitStructure;

	/*ʹ��USART2����ʱ��*/  
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	
	/*��λ����2*/
	USART_DeInit(USART2);  

	/*USART2_GPIO��ʼ������*/ 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;			//USART2_TXD(PA.2)     
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;		//�����������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//������������������Ϊ50MHz
	GPIO_Init(GPIOA, &GPIO_InitStructure);				//���ÿ⺯���е�GPIO��ʼ����������ʼ��USART1_TXD(PA.9)  


	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;				//USART2_RXD(PA.3)
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;	//��������
	GPIO_Init(GPIOA, &GPIO_InitStructure);					//���ÿ⺯���е�GPIO��ʼ����������ʼ��USART1_RXD(PA.10)


	/*USART2 ��ʼ������*/
	USART_InitStructure.USART_BaudRate = bound;										//���ò�����
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;						//8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;							//1��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;								//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;					//����ģʽ����Ϊ�շ�ģʽ
	USART_Init(USART2, &USART_InitStructure);										//��ʼ������2

	/*Usart1 NVIC����*/
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1;	//��ռ���ȼ�1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		//�����ȼ�0
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);							//����ָ���Ĳ�����ʼ��VIC�Ĵ��� 

	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);			//ʹ�ܴ���2�����ж�

	USART_Cmd(USART2, ENABLE);                    			//ʹ�ܴ��� 
	USART_ClearFlag(USART2, USART_FLAG_TC);					//���������ɱ�־
}
/*******************************************************************************
* ������  : UART2_SendString
* ����    : USART2�����ַ���
* ����    : *s�ַ���ָ��
* ���    : ��
* ����    : �� 
* ˵��    : ��
*******************************************************************************/
void UART2_SendString(char* s)
{
	while(*s)//����ַ���������
	{
		while(USART_GetFlagStatus(USART2, USART_FLAG_TC)==RESET); 
		USART_SendData(USART2 ,*s++);//���͵�ǰ�ַ�
	}
}

void UART2_Send_Command(char* s)
{
	CLR_Buf(); //��ս������ݵ�buffer
	UART2_SendString(s); //�����ַ���
	UART2_SendString("\r\n"); //���Զ����� \r\n�����ַ�
}

/*******************************************************************************
* ������ : Second_AT_Command
* ����   : ����ATָ���
* ����   : �������ݵ�ָ�롢ϣ���յ���Ӧ�𡢷��͵ȴ�ʱ��(��λ��20ms)
* ���   : 
* ����   : 
* ע��   : 
*******************************************************************************/
extern u8 Find(char *a);

u8 UART2_Send_AT_Command(char *b,char *a,u8 wait_time,u32 interval_time)         
{
	u8 i;
	i = 0;
	while(i < wait_time)    //���û���ҵ� �ͼ����ٷ�һ��ָ�� �ٽ��в���Ŀ���ַ���                
	{
		UART2_Send_Command(b);//����2���� b �ַ��� �����Զ�����\r\n  �൱�ڷ�����һ��ָ��
		delay_ms(interval_time); //�ȴ�һ��ʱ�� ��50�Ļ����� 50*20ms = 1��
		if(Find(a))            //������ҪӦ����ַ��� a
		{
			return 1;
		}
		i++;
	}
	
	return 0;
}

void UART2_Send_Command_END(char* s)
{
	CLR_Buf(); //��ս������ݵ�buffer
	UART2_SendString(s); //�����ַ���
}

u8 UART2_Send_AT_Command_End(char *b,char *a,u8 wait_time,u32 interval_time)         
{
	u8 i;
	i = 0;
	while(i < wait_time)    //���û���ҵ� �ͼ����ٷ�һ��ָ�� �ٽ��в���Ŀ���ַ���                
	{
		UART2_Send_Command_END(b);//����2���� b �ַ��� ���ﲻ����\r\n 
		delay_ms(interval_time); //�ȴ�һ��ʱ�� ��50�Ļ����� 50*20ms = 1��
		if(Find(a))            //������ҪӦ����ַ��� a
		{
			return 1;
		}
		i++;
	}
	
	return 0;
}



