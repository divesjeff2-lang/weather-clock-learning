#include "aht20.h"
#include "stm32f4xx.h"


#define I2C_CHECK_EVENT(event, timeout) do { \
	uint32_t _timeout = timeout; \
	while (!I2C_CheckEvent(I2C2, event)) { \
		if (--_timeout == 0) { \
			return false; \
		} \
	} \
} while (0)

void ath20_init(void)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C2, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);

	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_StructInit(&GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_High_Speed;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource10, GPIO_AF_I2C2);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource11, GPIO_AF_I2C2);
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	I2C_InitTypeDef I2C_InitStructure;
	I2C_StructInit(&I2C_InitStructure);	
	I2C_InitStructure.I2C_ClockSpeed = 100ul*1000ul;
	I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
	I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
	I2C_InitStructure.I2C_OwnAddress1 = 0x00;
	I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
	I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
	I2C_Init(I2C2, &I2C_InitStructure);
	I2C_Cmd(I2C2, ENABLE);

}

 static bool ath20_write(uint8_t data[], uint32_t length)
 	{
		I2C_AcknowledgeConfig(I2C2, ENABLE);
		I2C_GenerateSTART(I2C2, ENABLE);
		I2C_CHECK_EVENT(I2C_EVENT_MASTER_MODE_SELECT,1000);
		I2C_Send7bitAddress(I2C2, 0x70, I2C_Direction_Transmitter);
		I2C_CHECK_EVENT(I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED,1000);
		for(uint32_t i = 0; i < length; i++)
		{
		I2C_SendData(I2C2, data[i]);
		I2C_CHECK_EVENT(I2C_EVENT_MASTER_BYTE_TRANSMITTED,1000);
		}
		I2C_GenerateSTOP(I2C2, ENABLE);

		return true;
	}

 static bool ath20_read(uint8_t data[], uint32_t length)
	{
		I2C_AcknowledgeConfig(I2C2, ENABLE);
		I2C_GenerateSTART(I2C2, ENABLE);
		I2C_CHECK_EVENT(I2C_EVENT_MASTER_MODE_SELECT,1000);
		I2C_Send7bitAddress(I2C2, 0x70, I2C_Direction_Receiver);
		I2C_CHECK_EVENT(I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED,1000);
		for(uint32_t i = 0; i < length; i++)
		{
			if(i == length - 1)
			{
				I2C_AcknowledgeConfig(I2C2, DISABLE);
				I2C_GenerateSTOP(I2C2, ENABLE);
			}
			I2C_CHECK_EVENT(I2C_EVENT_MASTER_BYTE_RECEIVED,1000);
			data[i] = I2C_ReceiveData(I2C2);
		}
		return true;
	}