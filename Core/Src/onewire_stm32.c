#include "onewire_stm32.h"
#include <stdio.h>
#include <string.h>
extern UART_HandleTypeDef huart2;
extern void delay_us(uint16_t us);
static GPIO_TypeDef *ow_port;
static uint16_t ow_pin;
static uint8_t ow_pin_num;

void OW_Init(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin)
{
    ow_port = GPIOx;
    ow_pin = GPIO_Pin;

    for (uint8_t i = 0; i < 16; i++)
    {
        if (GPIO_Pin & (1U << i))
        {
            ow_pin_num = i;
            break;
        }
    }

    HAL_GPIO_WritePin(ow_port, ow_pin, GPIO_PIN_SET);
}

static void OW_Output(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = ow_pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(ow_port, &GPIO_InitStruct);
}
static void OW_Input(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    GPIO_InitStruct.Pin = ow_pin;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(ow_port, &GPIO_InitStruct);
}
uint8_t OW_Reset(void)
{
    uint8_t presence = 0;

    OW_Output();
    HAL_GPIO_WritePin(ow_port, ow_pin, GPIO_PIN_RESET);
    delay_us(600);

    OW_Input();
    delay_us(30);

    if (HAL_GPIO_ReadPin(ow_port, ow_pin) == GPIO_PIN_RESET)
        presence = 1;

    delay_us(500);

    return presence;
}

void OW_WriteByte(uint8_t data)
{
    for (int i = 0; i < 8; i++)
    {
        OW_Output();
        HAL_GPIO_WritePin(ow_port, ow_pin, GPIO_PIN_RESET);

        if (data & (1 << i))
        {
            delay_us(6);
            OW_Input();
            delay_us(64);
        }
        else
        {
            delay_us(65);
            OW_Input();
            delay_us(10);
        }

        OW_Input();
        delay_us(10); // recovery между битами
    }
}
uint8_t OW_ReadByte(void)
{
    uint8_t value = 0;

    for (int i = 0; i < 8; i++)
    {
        OW_Output();
        HAL_GPIO_WritePin(ow_port, ow_pin, GPIO_PIN_RESET);
        delay_us(3);

        OW_Input();

        delay_us(20);
        GPIO_PinState s20 = HAL_GPIO_ReadPin(ow_port, ow_pin);

        delay_us(10);
        GPIO_PinState s30 = HAL_GPIO_ReadPin(ow_port, ow_pin);

        delay_us(10);
        GPIO_PinState s40 = HAL_GPIO_ReadPin(ow_port, ow_pin);

        if (s30 == GPIO_PIN_SET)
            value |= (1 << i);

        delay_us(30);
    }

    return value;
}
