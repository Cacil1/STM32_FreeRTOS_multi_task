#include "mpu6050.h"
#include "usart.h"

uint8_t MPU_Init(void)
{ 
    uint8_t res;
    SOFT_IIC_Init();                                        // GPIO初始化(开SCL/SDA时钟+配置)
    
    MPU_Write_Byte(MPU_PWR_MGMT1_REG, 0X80);           // 软复位(复位所有寄存器到默认值)
    HAL_Delay(100);                                    // 等芯片内部复位完成(手册要求)
    MPU_Write_Byte(MPU_PWR_MGMT1_REG, 0X00);           // 唤醒(解除睡眠模式)
    
    MPU_Set_Gyro_Fsr(3);                               // 陀螺仪量程 ±2000°/s
    MPU_Set_Accel_Fsr(0);                              // 加速度计量程 ±2g
    MPU_Set_Rate(50);                                  // 采样率50Hz
    
    MPU_Write_Byte(MPU_INT_EN_REG, 0X00);              // 关闭所有中断
    MPU_Write_Byte(MPU_USER_CTRL_REG, 0X00);           // 关I2C主模式(AUX接口不用)
    MPU_Write_Byte(MPU_FIFO_EN_REG, 0X00);             // 关FIFO
    MPU_Write_Byte(MPU_INTBP_CFG_REG, 0X80);           // INT引脚低电平有效
    
    res = MPU_Read_Byte(MPU_DEVICE_ID_REG);            // 读WHO AM I验证芯片在线
    if(res == MPU_ADDR)                                // ID正确 → 芯片确实在总线上
    {
        MPU_Write_Byte(MPU_PWR_MGMT1_REG, 0X01);       // 时钟源=PLL(X轴陀螺)
        MPU_Write_Byte(MPU_PWR_MGMT2_REG, 0X00);       // 6轴全部使能
        MPU_Set_Rate(50);                              // 重设采样率50Hz
    }
    else return 1;                                     //    ID不对 → 芯片不在/接线错/地址错
    return 0;
}

//����MPU6050�����Ǵ����������̷�Χ
//fsr:0,��250dps;1,��500dps;2,��1000dps;3,��2000dps
//����ֵ:0,���óɹ�
//    ����,����ʧ�� 
uint8_t MPU_Set_Gyro_Fsr(uint8_t fsr)
{
	return MPU_Write_Byte(MPU_GYRO_CFG_REG,fsr<<3);//���������������̷�Χ  
}
//����MPU6050���ٶȴ����������̷�Χ
//fsr:0,��2g;1,��4g;2,��8g;3,��16g
//����ֵ:0,���óɹ�
//    ����,����ʧ�� 
uint8_t MPU_Set_Accel_Fsr(uint8_t fsr)
{
	return MPU_Write_Byte(MPU_ACCEL_CFG_REG,fsr<<3);//���ü��ٶȴ����������̷�Χ  
}

// 低通滤波
uint8_t MPU_Set_LPF(uint16_t lpf)
{
	uint8_t data=0;
	if(lpf>=188)data=1;
	else if(lpf>=98)data=2;
	else if(lpf>=42)data=3;
	else if(lpf>=20)data=4;
	else if(lpf>=10)data=5;
	else data=6; 
	return MPU_Write_Byte(MPU_CFG_REG,data);//�������ֵ�ͨ�˲���  
}
//����MPU6050�Ĳ�����(�ٶ�Fs=1KHz)
//rate:4~1000(Hz)
//����ֵ:0,���óɹ�
//    ����,����ʧ�� 
uint8_t MPU_Set_Rate(uint16_t rate)
{
	uint8_t data;
	if(rate>1000)rate=1000;
	if(rate<4)rate=4;
	data=1000/rate-1;
	data=MPU_Write_Byte(MPU_SAMPLE_RATE_REG,data);	//�������ֵ�ͨ�˲���
 	return MPU_Set_LPF(rate/2);	//�Զ�����LPFΪ�����ʵ�һ��
}

//�õ��¶�ֵ
//����ֵ:�¶�ֵ(������100��)
short MPU_Get_Temperature(void)
{
    uint8_t buf[2]; 
    short raw;
	float temp;
	MPU_Read_Len(MPU_ADDR,MPU_TEMP_OUTH_REG,2,buf); 
    raw=((uint16_t)buf[0]<<8)|buf[1];  
    temp=36.53+((double)raw)/340;  
    return temp*100;;
}
//�õ�������ֵ(ԭʼֵ)
//gx,gy,gz:������x,y,z���ԭʼ����(������)
//����ֵ:0,�ɹ�
//    ����,�������
uint8_t MPU_Get_Gyroscope(short *gx,short *gy,short *gz)
{
    uint8_t buf[6],res;  
	res=MPU_Read_Len(MPU_ADDR,MPU_GYRO_XOUTH_REG,6,buf);
	if(res==0)
	{
		*gx=((uint16_t)buf[0]<<8)|buf[1];  
		*gy=((uint16_t)buf[2]<<8)|buf[3];  
		*gz=((uint16_t)buf[4]<<8)|buf[5];
	} 	
    return res;;
}
//�õ����ٶ�ֵ(ԭʼֵ)
//gx,gy,gz:������x,y,z���ԭʼ����(������)
//����ֵ:0,�ɹ�
//    ����,�������
uint8_t MPU_Get_Accelerometer(short *ax,short *ay,short *az)
{
    uint8_t buf[6],res;  
	res=MPU_Read_Len(MPU_ADDR,MPU_ACCEL_XOUTH_REG,6,buf);
	if(res==0)
	{
		*ax=((uint16_t)buf[0]<<8)|buf[1];  
		*ay=((uint16_t)buf[2]<<8)|buf[3];  
		*az=((uint16_t)buf[4]<<8)|buf[5];
	} 	
    return res;;
}
//IIC����д
//addr:������ַ 
//reg:�Ĵ�����ַ
//len:д�볤��
//buf:������
//����ֵ:0,����
//    ����,�������
uint8_t MPU_Write_Len(uint8_t addr,uint8_t reg,uint8_t len,uint8_t *buf)
{
	uint8_t i; 
    SOFT_IIC_Start(); 
	SOFT_IIC_Send_Byte((addr<<1)|0);//����������ַ+д����	
	if(SOFT_IIC_Wait_Ack())	//�ȴ�Ӧ��
	{
		SOFT_IIC_Stop();		 
		return 1;		
	}
    SOFT_IIC_Send_Byte(reg);	//д�Ĵ�����ַ
    SOFT_IIC_Wait_Ack();		//�ȴ�Ӧ��
	for(i=0;i<len;i++)
	{
		SOFT_IIC_Send_Byte(buf[i]);	//��������
		if(SOFT_IIC_Wait_Ack())		//�ȴ�ACK
		{
			SOFT_IIC_Stop();	 
			return 1;		 
		}		
	}    
    SOFT_IIC_Stop();	 
	return 0;	
} 
//IIC������
//addr:������ַ
//reg:Ҫ��ȡ�ļĴ�����ַ
//len:Ҫ��ȡ�ĳ���
//buf:��ȡ�������ݴ洢��
//����ֵ:0,����
//    ����,�������
uint8_t MPU_Read_Len(uint8_t addr,uint8_t reg,uint8_t len,uint8_t *buf)
{ 
 	SOFT_IIC_Start(); 
	SOFT_IIC_Send_Byte((addr<<1)|0);//����������ַ+д����	
	if(SOFT_IIC_Wait_Ack())	//�ȴ�Ӧ��
	{
		SOFT_IIC_Stop();		 
		return 1;		
	}
    SOFT_IIC_Send_Byte(reg);	//д�Ĵ�����ַ
    SOFT_IIC_Wait_Ack();		//�ȴ�Ӧ��
    SOFT_IIC_Start();
	SOFT_IIC_Send_Byte((addr<<1)|1);//����������ַ+������	
    SOFT_IIC_Wait_Ack();		//�ȴ�Ӧ�� 
	while(len)
	{
		if(len==1)*buf=SOFT_IIC_Read_Byte(0);//������,����nACK 
		else *buf=SOFT_IIC_Read_Byte(1);		//������,����ACK  
		len--;
		buf++; 
	}    
    SOFT_IIC_Stop();	//����һ��ֹͣ���� 
	return 0;	
}

// 软件IIC写字节
uint8_t MPU_Write_Byte(uint8_t reg,uint8_t data) 				 
{ 
    SOFT_IIC_Start(); 	//起始信号
	SOFT_IIC_Send_Byte((MPU_ADDR<<1)|0);	//器件地址+写位(0)  → 0xD0
	if(SOFT_IIC_Wait_Ack())	//等待从机应答
	{
		SOFT_IIC_Stop();		 
		return 1;		
	}
    SOFT_IIC_Send_Byte(reg);		//写寄存器地址
    SOFT_IIC_Wait_Ack();		//等待应答
	SOFT_IIC_Send_Byte(data);//发送数据
	if(SOFT_IIC_Wait_Ack())	//等待应答
	{
		SOFT_IIC_Stop();	 	//停止信号
		return 1;		 
	}		 
    SOFT_IIC_Stop();	 
	return 0;
}

// 软件IIC读字节
uint8_t MPU_Read_Byte(uint8_t reg)
{
	uint8_t res;
    SOFT_IIC_Start(); 
	SOFT_IIC_Send_Byte((MPU_ADDR<<1)|0);
	SOFT_IIC_Wait_Ack();		//�ȴ�Ӧ�� 
    SOFT_IIC_Send_Byte(reg);	//д�Ĵ�����ַ
    SOFT_IIC_Wait_Ack();		//�ȴ�Ӧ��
    SOFT_IIC_Start();
	SOFT_IIC_Send_Byte((MPU_ADDR<<1)|1);//����������ַ+������	
    SOFT_IIC_Wait_Ack();		//�ȴ�Ӧ�� 
	res=SOFT_IIC_Read_Byte(0);//��ȡ����,����nACK 
    SOFT_IIC_Stop();			//����һ��ֹͣ���� 
	return res;		
}


