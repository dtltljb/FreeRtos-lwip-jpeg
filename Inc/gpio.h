/**
  ******************************************************************************
  * File Name          : gpio.h
  * Description        : This file contains all the functions prototypes for 
  *                      the gpio  
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __gpio_H
#define __gpio_H
#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* USER CODE BEGIN Private defines */

#define	Max_X		52
#define	Max_Y		44

/* USER CODE END Private defines */

void MX_GPIO_Init(void);

/* USER CODE BEGIN Prototypes */

#define	TURN_ON_LED		1
#define	TURN_OFF_LED		0

#define	LED2(x)	\
				HAL_GPIO_WritePin(GPIOC,LED2_Pin,x)
#define	LED3(x)	\
				HAL_GPIO_WritePin(GPIOC,LED3_Pin,x)

#define	Toggle_LED2()	\
				HAL_GPIO_TogglePin(GPIOC,LED2_Pin)
#define	Toggle_LED3()	\
				HAL_GPIO_TogglePin(GPIOC,LED3_Pin)

#define	Toggle_Frame()	\
				HAL_GPIO_TogglePin(GPIOD,H_OUT_Pin)
#define	Toggle_Line()	\
				HAL_GPIO_TogglePin(GPIOD,H_OUTD11_Pin)


/** select matrix x,y dot,use H-EN-1~4,H-OE-1~3 pins  
        * Input  Parm: uint16_t x,y
        * Output Para: none zero is error 
*/

int8_t	Select_Matrix_Dot(uint16_t x,uint16_t y);

/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif
#endif /*__ pinoutConfig_H */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
