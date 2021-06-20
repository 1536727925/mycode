#include "remote.h"
#include "delay.h"
#include "usart.h"

//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ���������ɣ��������������κ���;
//ALIENTEK NANO STM32������
//����ң�ؽ��� ��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2018/3/27
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) �������������ӿƼ����޹�˾ 2018-2028
//All rights reserved									  
//////////////////////////////////////////////////////////////////////////////////   
 
//����ң�س�ʼ��
//����IO�Լ���ʱ��3�����벶��
void Remote_Init(void)    			  
{  
	RCC->APB1ENR|=1<<1;   	//TIM3 ʱ��ʹ�� 
	RCC->APB2ENR|=1<<3;    	//ʹ��PORTBʱ�� 
	GPIOB->CRL&=0XFFFFFFF0;	//PB0 ����  
	GPIOB->CRL|=0X00000008;	//��������     
	GPIOB->ODR|=1<<0;		//PB0 ����
	
	RCC->APB2ENR|=1<<2; //ʹ��PORTAʱ��	 
	GPIOA->CRL&=0XF0FFFFFF;	//PC6���
	GPIOA->CRL|=0X0B000000;	//���ù������ 	  
	RCC->APB2ENR|=1<<0;     //ʹ��AFIOʱ��	
    AFIO->MAPR&=0XFFFFF3FF; //���MAPR��[11:10]
	AFIO->MAPR|=0<<10;      //������ӳ��,TIM3_CH1->PC6
	TIM3->CCMR1|=6<<4;  	//CH1 PWM2ģʽ		 
	TIM3->CCMR1|=1<<3; 	    //CH1Ԥװ��ʹ��	   
	TIM3->CCER|=1<<0;   	//OC1 ���ʹ��	   
	TIM3->CR1=0x0080;   	//ARPEʹ�� 	
	
	
	TIM3->ARR=10000;  		//�趨�������Զ���װֵ ���10ms���  
	TIM3->PSC=71;  			//Ԥ��Ƶ��,1M�ļ���Ƶ��,1us��1.	
	TIM3->CCMR2|=1<<0;		//CC3S=01 	ѡ������� IC3ӳ�䵽TI3��
 	TIM3->CCMR2|=3<<4;  	//IC3F=0011 ���������˲��� 8����ʱ��ʱ�������˲�
 	TIM3->CCMR2|=0<<2;  	//IC3PS=00 	���������Ƶ,����Ƶ 
	TIM3->CCER|=0<<9; 		//CC3P=0	�����ز���
	TIM3->CCER|=1<<8; 		//CC3E=1 	���������������ֵ������Ĵ�����
	TIM3->DIER|=1<<3;   	//����CC3IE�����ж�				
	TIM3->DIER|=1<<0;   	//���������ж�				
	TIM3->CR1|=0x01;    	//ʹ�ܶ�ʱ��3
  	MY_NVIC_Init(1,3,TIM3_IRQn,2);//��ռ1�������ȼ�3����2	
}

 
//ң��������״̬
//[7]:�յ����������־
//[6]:�õ���һ��������������Ϣ
//[5]:����	
//[4]:����������Ƿ��Ѿ�������								   
//[3:0]:�����ʱ��
u8 	RmtSta=0;	  	  
u16 Dval;		//�½���ʱ��������ֵ
u32 RmtRec=0;	//������յ�������	   		    
u8  RmtCnt=0;	//�������µĴ���	  
//��ʱ��3�жϷ������	 
void TIM3_IRQHandler(void)
{ 	
    u16 tsr;
	tsr=TIM3->SR;
	
    if(tsr&0X01)//���
	{
		if(RmtSta&0x80)//�ϴ������ݱ����յ���
		{	
			RmtSta&=~0X10;						//ȡ���������Ѿ���������
			if((RmtSta&0X0F)==0X00)RmtSta|=1<<6;//����Ѿ����һ�ΰ����ļ�ֵ��Ϣ�ɼ�
			if((RmtSta&0X0F)<14)RmtSta++;
			else
			{
				RmtSta&=~(1<<7);//���������ʶ
				RmtSta&=0XF0;	//��ռ�����	
			}						 	   	
		}							    
	}
 	if(tsr&(1<<3))//CC3IE�ж�
	{	  
		if(RDATA)//�����ز���
		{			
			TIM3->CCER|=1<<9; 				//CC3P=1	����Ϊ�½��ز���
			TIM3->CNT=0;					//��ն�ʱ��ֵ
			RmtSta|=0X10;					//����������Ѿ�������		
		}else //�½��ز���
		{			
			
			Dval=TIM3->CCR3;				//��ȡCCR3Ҳ������CC2IF��־λ
  			TIM3->CCER&=~(1<<9);			//CC3P=0	����Ϊ�����ز���
 			
			if(RmtSta&0X10)					//���һ�θߵ�ƽ���� 
			{
 				if(RmtSta&0X80)//���յ���������
				{
					
					if(Dval>300&&Dval<800)			//560Ϊ��׼ֵ,560us
					{
						RmtRec<<=1;	//����һλ.
						RmtRec|=0;	//���յ�0	   
					}else if(Dval>1400&&Dval<1800)	//1680Ϊ��׼ֵ,1680us
					{
						RmtRec<<=1;	//����һλ.
						RmtRec|=1;	//���յ�1
					}else if(Dval>2200&&Dval<2600)	//�õ�������ֵ���ӵ���Ϣ 2500Ϊ��׼ֵ2.5ms
					{
						RmtCnt++; 		//������������1��
						RmtSta&=0XF0;	//��ռ�ʱ��		
					}
 				}else if(Dval>4200&&Dval<4700)		//4500Ϊ��׼ֵ4.5ms
				{
					RmtSta|=1<<7;	//��ǳɹ����յ���������
					RmtCnt=0;		//�����������������
				}						 
			}
			RmtSta&=~(1<<4);
		}				 		     	    					   
	}	
   TIM3->SR=0;//����жϱ�־λ     
}

//�����������
//����ֵ:
//	 0,û���κΰ�������
//����,���µİ�����ֵ.
u8 Remote_Scan(void)
{        
	u8 sta=0;       
    u8 t1,t2;  
	if(RmtSta&(1<<6))//�õ�һ��������������Ϣ��
	{ 
	    t1=RmtRec>>24;			//�õ���ַ��
	    t2=(RmtRec>>16)&0xff;	//�õ���ַ���� 
 	    if((t1==(u8)~t2)&&t1==REMOTE_ID)//����ң��ʶ����(ID)����ַ 
	    { 
	        t1=RmtRec>>8;
	        t2=RmtRec; 	
	        if(t1==(u8)~t2)sta=t1;//��ֵ��ȷ	 
		}   
		if((sta==0)||((RmtSta&0X80)==0))//�������ݴ���/ң���Ѿ�û�а�����
		{
		 	RmtSta&=~(1<<6);//������յ���Ч������ʶ
			RmtCnt=0;		//�����������������
		}
	}  
    return sta;
}

 