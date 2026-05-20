#ifndef ONEWIRE_STM32_H
#define ONEWIRE_STM32_H

#include "main.h"

void OW_Init(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin);
uint8_t OW_Reset(void);
void OW_WriteByte(uint8_t data);
uint8_t OW_ReadByte(void);

#endif