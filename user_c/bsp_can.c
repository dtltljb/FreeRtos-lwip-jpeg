/**
  ******************************************************************************
  * @file    bsp_debug_usart.c
  * @author  fire
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   can		driver
  ******************************************************************************
  * @attention
  *
  *
  ******************************************************************************
  */ 
#include <stdio.h>
#include "can.h"


/**
  * @brief  Enables or disables the specified CANx interrupts.
  * @param  CANx: where x can be 1,2 or 3 to select the CAN peripheral.
  * @param  CAN_IT: specifies the CAN interrupt sources to be enabled or disabled.
  *          This parameter can be: 
  *            @arg CAN_IT_TME: Transmit mailbox empty Interrupt 
  *            @arg CAN_IT_FMP0: FIFO 0 message pending Interrupt 
  *            @arg CAN_IT_FF0: FIFO 0 full Interrupt
  *            @arg CAN_IT_FOV0: FIFO 0 overrun Interrupt
  *            @arg CAN_IT_FMP1: FIFO 1 message pending Interrupt 
  *            @arg CAN_IT_FF1: FIFO 1 full Interrupt
  *            @arg CAN_IT_FOV1: FIFO 1 overrun Interrupt
  *            @arg CAN_IT_WKU: Wake-up Interrupt
  *            @arg CAN_IT_SLK: Sleep acknowledge Interrupt  
  *            @arg CAN_IT_EWG: Error warning Interrupt
  *            @arg CAN_IT_EPV: Error passive Interrupt
  *            @arg CAN_IT_BOF: Bus-off Interrupt  
  *            @arg CAN_IT_LEC: Last error code Interrupt
  *            @arg CAN_IT_ERR: Error Interrupt
  * @param  NewState: new state of the CAN interrupts.
  * @note   CAN3 peripheral is available only for STM32F413_423xx devices
  *          This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void CAN_ITConfig(CAN_TypeDef* CANx, uint32_t CAN_IT, FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_CAN_ALL_PERIPH(CANx));
  assert_param(IS_CAN_IT(CAN_IT));
  assert_param(IS_FUNCTIONAL_STATE(NewState));

  if (NewState != DISABLE)
  {
    /* Enable the selected CANx interrupt */
    CANx->IER |= CAN_IT;
  }
  else
  {
    /* Disable the selected CANx interrupt */
    CANx->IER &= ~CAN_IT;
  }
}

/*
 * 函数名：CAN_Filter_Config
 * 描述  ：CAN的过滤器 配置
 * 输入  ：无
 * 输出  : 无
 * 调用  ：内部调用
 */
void CAN_Filter_Config(void)
{
	//CAN_FilterInitTypeDef  CAN_FilterInitStructure;
	CAN_FilterTypeDef		CAN_FilterInitStructure;
	
	/*CAN筛选器初始化*/
	//CAN_FilterInitStructure.CAN_FilterNumber=14;						//筛选器组14
	CAN_FilterInitStructure.FilterBank	=	14;									//筛选器组14
	CAN_FilterInitStructure.FilterMode=CAN_FILTERMODE_IDMASK;	//工作在掩码模式
	CAN_FilterInitStructure.FilterScale	=	CAN_FILTERSCALE_32BIT;	//筛选器位宽为单个32位。
	/* 使能筛选器，按照标志的内容进行比对筛选，扩展ID不是如下的就抛弃掉，是的话，会存入FIFO0。 */
/*//receive only CAN_ID_EXT 0x1314
	CAN_FilterInitStructure.FilterIdHigh= ((((uint32_t)0x1314<<3)|CAN_ID_EXT|CAN_RTR_DATA)&0xFFFF0000)>>16;		//要筛选的ID高位 
	CAN_FilterInitStructure.FilterIdLow= (((uint32_t)0x1314<<3)|CAN_ID_EXT|CAN_RTR_DATA)&0xFFFF; //要筛选的ID低位 
	CAN_FilterInitStructure.FilterMaskIdHigh= 0xFFFF;			//筛选器高16位每位必须匹配
	CAN_FilterInitStructure.FilterMaskIdLow= 0xFFFF;			//筛选器低16位每位必须匹配
*/
	//enable receive any data
	CAN_FilterInitStructure.FilterIdHigh= 0;						//要筛选的ID高位 
	CAN_FilterInitStructure.FilterIdLow= 0; 					//要筛选的ID低位 
	CAN_FilterInitStructure.FilterMaskIdHigh= 0;			//筛选器高16位每位必须匹配
	CAN_FilterInitStructure.FilterMaskIdLow= 0;				//筛选器低16位每位必须匹配
	
	//CAN_FilterInitStructure.FilterFIFOAssignment=CAN_Filter_FIFO0 ;				//筛选器被关联到FIFO0
	CAN_FilterInitStructure.FilterFIFOAssignment=CAN_FILTER_FIFO0 ;				//筛选器被关联到FIFO0
	
	CAN_FilterInitStructure.FilterActivation=ENABLE;			//使能筛选器
	//CAN_FilterInit(&CAN_FilterInitStructure);
	HAL_CAN_ConfigFilter(&hcan2,&CAN_FilterInitStructure);
	/*CAN通信中断使能*/
	//CAN_ITConfig(CANx, CAN_IT_FMP0, ENABLE);
	/**	what not CAN_IT_RX_FIFO0_FULL(FIFO 0 full interrupt) 	*	*/
//	CAN_ITConfig(hcan2.Instance, CAN_IT_RX_FIFO0_MSG_PENDING, ENABLE);  /*!< FIFO 0 message pending interrupt */
//	CAN_ITConfig(CAN2 ,CAN_IT_TX_MAILBOX_EMPTY, ENABLE);								/*!< Transmit mailbox empty interrupt */
	
	HAL_CAN_ActivateNotification(&hcan2,CAN_IT_RX_FIFO0_MSG_PENDING);		/*!< FIFO 0 message pending interrupt */
	HAL_CAN_ActivateNotification(&hcan2,CAN_IT_TX_MAILBOX_EMPTY);				/*!< Transmit mailbox empty interrupt */
	
}




/**
  * @brief  初始化 Rx Message数据结构体
  * @param  RxMessage: 指向要初始化的数据结构体
  * @retval None
  */

void Init_RxMes(CAN_RxHeaderTypeDef *RxMessage)
{
  uint8_t ubCounter = 0;

	/*把接收结构体清零*/
  RxMessage->StdId = 0x00;
  RxMessage->ExtId = 0x00;
  RxMessage->IDE = CAN_ID_STD;
  RxMessage->DLC = 0;
//  RxMessage->FMI = 0;

}


/*
 * 函数名：CAN_SetMsg
 * 描述  ：CAN通信报文内容设置,设置一个数据内容为0-7的数据包
 * 输入  ：发送报文结构体
 * 输出  : 无
 * 调用  ：外部调用
 */	 
void CAN_SetMsg(CAN_TxHeaderTypeDef *TxMessage)
{	  
	uint8_t ubCounter = 0;

//  TxMessage->StdId=0x68;		
//  TxMessage->IDE=CAN_ID_STD;					 //normal
  TxMessage->ExtId=0x1314;					 //使用的扩展ID
  TxMessage->IDE=CAN_ID_EXT;					 //扩展模式
  TxMessage->RTR=CAN_RTR_DATA;				 //发送的是数据
  TxMessage->DLC=8;							 			//数据长度为8字节
	
	
}

/** static configure CAN	CALL BACK funtion	**/

CAN_RxHeaderTypeDef			m_RxMessage;
uint8_t					CAN_Rx_Data[8];
uint8_t					CAN_RxFlag=0;

/**
  * @brief  Rx FIFO 0 message pending callback.
  * @param  hcan pointer to a CAN_HandleTypeDef structure that contains
  *         the configuration information for the specified CAN.
  * @retval None
  */
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
  /* Prevent unused argument(s) compilation warning */
	HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &m_RxMessage, CAN_Rx_Data);
	CAN_RxFlag	=	Request;
}

/**
  * @brief  Rx FIFO 0 full callback.
  * @param  hcan pointer to a CAN_RxHeaderTypeDef structure that contains
  *         the configuration information for the specified CAN.
  * @retval None
  */
void HAL_CAN_RxFifo0FullCallback(CAN_HandleTypeDef *hcan)
{
  /* Prevent unused argument(s) compilation warning */

}

/**
  * @brief  get	CAN_RxHeader function
  * @param  hcan pointer to a CAN_HandleTypeDef structure that contains
  *         copy to pointer.
  * @retval None
  */
void Get_CAN_RX_Header(CAN_RxHeaderTypeDef *msg)
{
  memcpy((char*)msg, (char*)&m_RxMessage,sizeof(CAN_RxHeaderTypeDef));
}


/**************************END OF FILE************************************/











