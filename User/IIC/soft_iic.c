#include "soft_iic.h"

/**
 * @brief       ��ʼ��IIC
 * @param       ��
 * @retval      ��
 */
void soft_iic_init(void)
{
    GPIO_InitTypeDef gpio_init_struct;

    SOFT_IIC_SCL_GPIO_CLK_ENABLE();  /* SCL����ʱ��ʹ�� */
    SOFT_IIC_SDA_GPIO_CLK_ENABLE();  /* SDA����ʱ��ʹ�� */

    gpio_init_struct.Pin = SOFT_IIC_SCL_GPIO_PIN;
    gpio_init_struct.Mode = GPIO_MODE_OUTPUT_PP;        /* ������� */
    gpio_init_struct.Pull = GPIO_PULLUP;                /* ���� */
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;      /* ���� */
    HAL_GPIO_Init(SOFT_IIC_SCL_GPIO_PORT, &gpio_init_struct);/* SCL */

    gpio_init_struct.Pin = SOFT_IIC_SDA_GPIO_PIN;
    gpio_init_struct.Mode = GPIO_MODE_OUTPUT_OD;        /* ��©��� */
    HAL_GPIO_Init(SOFT_IIC_SDA_GPIO_PORT, &gpio_init_struct);/* SDA */
    /* SDA����ģʽ����,��©���,����, �����Ͳ���������IO������, ��©�����ʱ��(=1), Ҳ���Զ�ȡ�ⲿ�źŵĸߵ͵�ƽ */

    soft_iic_stop();     /* ֹͣ�����������豸 */
}

/**
 * @brief       IIC��ʱ����,���ڿ���IIC��д�ٶ�
 * @param       ��
 * @retval      ��
 */
static void soft_iic_delay(void)
{
    // delay_us(2);    /* 2us����ʱ, ��д�ٶ���250Khz���� */
    // 	uint8_t t=1;
	// while(t--);
    //  for(volatile int i=0;i<20;i++);
    for(volatile uint8_t i=0;i<5;i++);
}

/**
 * @brief       ����IIC��ʼ�ź�
 * @param       ��
 * @retval      ��
 */
void soft_iic_start(void)
{
    SOFT_IIC_SDA(1);
    SOFT_IIC_SCL(1);
    soft_iic_delay();
    SOFT_IIC_SDA(0);     /* START�ź�: ��SCLΪ��ʱ, SDA�Ӹ߱�ɵ�, ��ʾ��ʼ�ź� */
    soft_iic_delay();
    SOFT_IIC_SCL(0);     /* ǯסI2C���ߣ�׼�����ͻ�������� */
    soft_iic_delay();
}

/**
 * @brief       ����IICֹͣ�ź�
 * @param       ��
 * @retval      ��
 */
void soft_iic_stop(void)
{
    SOFT_IIC_SDA(0);     /* STOP�ź�: ��SCLΪ��ʱ, SDA�ӵͱ�ɸ�, ��ʾֹͣ�ź� */
    soft_iic_delay();
    SOFT_IIC_SCL(1);
    soft_iic_delay();
    SOFT_IIC_SDA(1);     /* ����I2C���߽����ź� */
    soft_iic_delay();
}

/**
 * @brief       �ȴ�Ӧ���źŵ���
 * @param       ��
 * @retval      1������Ӧ��ʧ��
 *              0������Ӧ��ɹ�
 */
uint8_t soft_iic_wait_ack(void)
{
    uint8_t waittime = 0;
    uint8_t rack = 0;

    SOFT_IIC_SDA(1);     /* �����ͷ�SDA��(��ʱ�ⲿ������������SDA��) */
    soft_iic_delay();
    SOFT_IIC_SCL(1);     /* SCL=1, ��ʱ�ӻ����Է���ACK */
    soft_iic_delay();

    while (SOFT_IIC_READ_SDA)    /* �ȴ�Ӧ�� */
    {
        waittime++;

        if (waittime > 250)
        {
            soft_iic_stop();
            rack = 1;
            break;
        }
    }

    SOFT_IIC_SCL(0);     /* SCL=0, ����ACK��� */
    soft_iic_delay();
    return rack;
    // return 0;
}

/**
 * @brief       ����ACKӦ��
 * @param       ��
 * @retval      ��
 */
void soft_iic_ack(void)
{
    SOFT_IIC_SDA(0);     /* SCL 0 -> 1  ʱ SDA = 0,��ʾӦ�� */
    soft_iic_delay();
    SOFT_IIC_SCL(1);     /* ����һ��ʱ�� */
    soft_iic_delay();
    SOFT_IIC_SCL(0);
    soft_iic_delay();
    SOFT_IIC_SDA(1);     /* �����ͷ�SDA�� */
    soft_iic_delay();
}

/**
 * @brief       ������ACKӦ��
 * @param       ��
 * @retval      ��
 */
void soft_iic_nack(void)
{
    SOFT_IIC_SDA(1);     /* SCL 0 -> 1  ʱ SDA = 1,��ʾ��Ӧ�� */
    soft_iic_delay();
    SOFT_IIC_SCL(1);     /* ����һ��ʱ�� */
    soft_iic_delay();
    SOFT_IIC_SCL(0);
    soft_iic_delay();
}

/**
 * @brief       IIC����һ���ֽ�
 * @param       data: Ҫ���͵�����
 * @retval      ��
 */
void soft_iic_send_byte(uint8_t data)
{
    uint8_t t;
    
    for (t = 0; t < 8; t++)
    {
        SOFT_IIC_SDA((data & 0x80) >> 7);    /* ��λ�ȷ��� */
        soft_iic_delay();
        SOFT_IIC_SCL(1);
        soft_iic_delay();
        SOFT_IIC_SCL(0);
        data <<= 1;     /* ����1λ,������һ�η��� */
    }
    SOFT_IIC_SDA(1);         /* �������, �����ͷ�SDA�� */
}

/**
 * @brief       IIC��ȡһ���ֽ�
 * @param       ack:  ack=1ʱ������ack; ack=0ʱ������nack
 * @retval      ���յ�������
 */
uint8_t soft_iic_read_byte(uint8_t ack)
{
    uint8_t i, receive = 0;

    for (i = 0; i < 8; i++ )    /* ����1���ֽ����� */
    {
        receive <<= 1;  /* ��λ�����,�������յ�������λҪ���� */
        SOFT_IIC_SCL(1);
        soft_iic_delay();

        if (SOFT_IIC_READ_SDA)
        {
            receive++;
        }
        
        SOFT_IIC_SCL(0);
        soft_iic_delay();
    }

    if (!ack)
    {
        soft_iic_nack();     /* ����nACK */
    }
    else
    {
        soft_iic_ack();      /* ����ACK */
    }

    return receive;
}
