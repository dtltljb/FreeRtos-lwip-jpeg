/**
  ******************************************************************************
  * File Name          : ADC.c
  * Description        : This file provides code for the configuration
  *                      of the ADC instances.
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

/* Includes ------------------------------------------------------------------*/
#include "adc.h"
#include "gpio.h"

/**
  * @brief  Enables or disables the specified ADC DMA request.
  * @param  ADCx: where x can be 1, 2 or 3 to select the ADC peripheral.
  * @param  NewState: new state of the selected ADC DMA transfer.
  *          This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void ADC_DMACmd(ADC_TypeDef* ADCx, FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_ADC_ALL_PERIPH(ADCx));
  assert_param(IS_FUNCTIONAL_STATE(NewState));
  if (NewState != DISABLE)
  {
    /* Enable the selected ADC DMA request */
    ADCx->CR2 |= (uint32_t)ADC_CR2_DMA;
  }
  else
  {
    /* Disable the selected ADC DMA request */
    ADCx->CR2 &= (uint32_t)(~ADC_CR2_DMA);
  }
}


/**
  * @brief  Enables or disables the specified ADC peripheral.
  * @param  ADCx: where x can be 1, 2 or 3 to select the ADC peripheral.
  * @param  NewState: new state of the ADCx peripheral. 
  *          This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void ADC_Cmd(ADC_TypeDef* ADCx, FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_ADC_ALL_PERIPH(ADCx));
  assert_param(IS_FUNCTIONAL_STATE(NewState));
  if (NewState != DISABLE)
  {
    /* Set the ADON bit to wake up the ADC from power down mode */
    ADCx->CR2 |= (uint32_t)ADC_CR2_ADON;
  }
  else
  {
    /* Disable the selected ADC peripheral */
    ADCx->CR2 &= (uint32_t)(~ADC_CR2_ADON);
  }
}

/**
  * @brief  Enables or disables the ADC DMA request after last transfer in multi ADC mode       
  * @param  NewState: new state of the selected ADC DMA request after last transfer.
  *          This parameter can be: ENABLE or DISABLE.
  * @note   if Enabled, DMA requests are issued as long as data are converted and 
  *         DMA mode for multi ADC mode (selected using ADC_CommonInit() function 
  *         by ADC_CommonInitStruct.ADC_DMAAccessMode structure member) is 
  *          ADC_DMAAccessMode_1, ADC_DMAAccessMode_2 or ADC_DMAAccessMode_3.     
  * @retval None
  */
  
  
void ADC_MultiModeDMARequestAfterLastTransferCmd(FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_FUNCTIONAL_STATE(NewState));
  if (NewState != DISABLE)
  {
    /* Enable the selected ADC DMA request after last transfer */
    ADC->CCR |= (uint32_t)ADC_CCR_DDS;
  }
  else
  {
    /* Disable the selected ADC DMA request after last transfer */
    ADC->CCR &= (uint32_t)(~ADC_CCR_DDS);
  }
}


/**
  * @brief  Enables the selected ADC software start conversion of the regular channels.
  * @param  ADCx: where x can be 1, 2 or 3 to select the ADC peripheral.
  * @retval None
  */
void ADC_SoftwareStartConv(ADC_TypeDef* ADCx)
{
  /* Check the parameters */
  assert_param(IS_ADC_ALL_PERIPH(ADCx));
  
  /* Enable the selected ADC conversion for regular group */
  ADCx->CR2 |= (uint32_t)ADC_CR2_SWSTART;
}


/*
*		get sample dot kPa01Value,Calibration minimum | maximun matrix 
*		para in: uint16_t x,y, and adcValue;
*		para out:uint16_t kpa01Value;
*		return value: 0 is sucess,none 0 is failure
*/

static	Calibrate_Para_s	m_Cal_Para;
int8_t	Get_Sample_kPa_Value(uint16_t x, uint16_t y, uint16_t ADCval, uint16_t *kPa)
{
	
	if ( ( x > Max_X) || ( y  > Max_Y) )
		return	-1;
  *kPa	=	ADCval * 1;  /* test calulate speed */		
  return	0;
}

/*!! DMA transfer complete callback  
 *	void    (* XferCpltCallback)( struct __DMA_HandleTypeDef * hdma);         
 */

uint16_t Sample_Frame_Out	=	0;
uint8_t Index = 0, Global_x = 0, Global_y = 0;
uint16_t	iMatrix[2][Max_X][Max_Y];

//	iMatrix	mMatrix1
void ADC_Conv_DMA_XferCpltCallback(struct __DMA_HandleTypeDef * hdma)
{
	uint16_t	Val;
	/* CHANGE SELECT MATRIX DOT*/
	Global_x	+= 1;															//one by one scanf
	if ( Global_x >= Max_X ){
		
		Global_y	+=	1;													//one by one scanf
		if ( Global_y >= Max_Y){
					Global_x	=	0x0000;
					Global_y	=	0x0000;
					/* output frame synchronization single */
					Toggle_Frame();
					/*OUT SAMPLE FRAME CONTEXT AND FLAG */
					if ( (Sample_Frame_Out == SAMPLE_FRAME_OUT_FLAG) || (Sample_Frame_Out == SAMPLE_FRAME_OVE_FLAG) ){
							Sample_Frame_Out	=	SAMPLE_FRAME_OVE_FLAG;
					}
					else {
							Sample_Frame_Out	=	SAMPLE_FRAME_OUT_FLAG;
					}
					/* change matrix index */
					if (Index >= 1){
								Index	= 0;
							}
					else{
								Index	+= 1;
					}

		}
		else{
			Global_x	=	0x0000;
			/* output line synchronization single */
			Toggle_Line();
		}
		
	}
	else{
			/* nothing do */
	}
	Select_Matrix_Dot(Global_x,Global_y);
	Get_Sample_kPa_Value(Global_x,Global_y,	(ADC_ConvertedValue[0]+ADC_ConvertedValue[1]+ADC_ConvertedValue[2])/3, &Val);  /* sum / 3 */
	iMatrix[Index][Global_x][Global_y] = Val;	
	
	/* soft startup adc  conveter,so that one by one sample dot */
	
 // ADC_SoftwareStartConv(hadc1.Instance);
   HAL_ADC_Start(&hadc1);

}

/*
*		sample matrix property  index, current use matrix 
*/
uint8_t Get_Sample_Matrix_Index(void){
	return	Index;
}



/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
