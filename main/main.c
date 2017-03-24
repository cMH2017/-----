#include <reg52.h>
#include "motor.h"
#include "trace.h"
#include "firesensor.h"
int  angle=0;
bit  pwmFlag=0;
sbit beep=P2^3;
sbit angleMotor=P1^2;
sbit fan=P2^5;				//连接继电器的引脚，控制风扇
uchar angleMotorCount=0;
extern uint fireSensorAngle[5];
extern char fireFlag;

void delayms(uint xms)
{
 uint i,j;
 for(i=xms;i>0;i--)
 	for(j=110;j>0;j--);
}

void main(void)		
{
	initWheelMotor();	 			//初始化轮子舵机
	delayms(2);
	initAngleMotor();	 			//初始化角度舵机，控制风扇
	fan=0;
	delayms(20);	
					
	while(1)
	{	
		ET0=1;
		angle=checkFire();			//传回度数
		if(fireFlag<0)
		{ 	
			fan=0;					//关闭继电器
			ET0=0;					//关闭角度舵机
			trace();				//如果没有火焰就循迹
		}
		else						//有火焰就灭火
		{
			delayms(200);			//防止误测
			if(fireFlag>0)
			{	
				if(fan==0)
				{
					ET0=1;
					delayms(800); 	//给时间转角度
					ET0=0;
				}	
				fan=1;	
				delayms(3000);				
			}
		}
	}
}

void Tim1forWheel(void) interrupt 3				//两个轮子
{
 	EA=0;
	wheelMotorInterrupt();
	EA=1;
}
void Tim0forAngleMotor(void) interrupt 1
{
	TR0=0;
	TH0=(65536-100)/256;
	TL0=(65536-100)%256;
	if(angle>angleMotorCount)				  //18,最左，3最右
	 	 angleMotor=1; 	
	else
	 	angleMotor=0;
	angleMotorCount++;
	if(angleMotorCount>=200)
	 	angleMotorCount=0;
	TR0=1;
}