#include "Tx.h"
#include "can.h"
#include "can.h"   

CAN_HandleTypeDef* DJmotor_GetCanHandle(void)
{
    return &hcan1;   
}


void DJmotor_CurrentTransmit(DJMotorPointer motor)
{
    static uint8_t tx_data[8] = {0};
    CAN_TxHeaderTypeDef tx_header = {0};
    uint32_t tx_mailbox = 0;  
    uint8_t tag = 0;

    /* 电流限幅由各模式函数负责,此处只打包发送 */
    tx_header.IDE = CAN_ID_STD;
    tx_header.RTR = CAN_RTR_DATA;
    tx_header.DLC = 8;
 

    if (motor->ID <= 4U)
    {
        tx_header.StdId = 0x200U;
        tag = (uint8_t)((motor->ID - 1U) * 2U);
    }
    else
    {
        tx_header.StdId = 0x1FFU;
        tag = (uint8_t)((motor->ID - 5U) * 2U);
    }

    EncodeS16Data(&motor->valSet.current_raw, &tx_data[tag]);
    ChangeDataByte(&tx_data[tag], &tx_data[tag + 1U]);
    HAL_CAN_AddTxMessage (DJmotor_GetCanHandle(),  &tx_header, tx_data, &tx_mailbox);
    
}
