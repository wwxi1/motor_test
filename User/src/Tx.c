#include "Tx.h"

void DJmotor_CurrentTransmit(DJMotorPointer motor)
{
    static uint8_t tx_data[8] = {0};
    CAN_TxHeaderTypeDef tx_header = {0};
    uint8_t tag = 0;

    /* 电流限幅由各模式函数负责,此处只打包发送 */
    tx_header.IdType = CAN_STANDARD_ID;
    tx_header.TxFrameType = CAN_DATA_FRAME;
    tx_header.DataLength = CAN_DLC_BYTES_8;
    tx_header.ErrorStateIndicator = CAN_ESI_ACTIVE;
    tx_header.BitRateSwitch = CAN_BRS_OFF;
    tx_header.FDFormat = CAN_CLASSIC_CAN;
    tx_header.TxEventFifoControl = CAN_NO_TX_EVENTS;
    tx_header.MessageMarker = 0;

    if (motor->ID <= 4U)
    {
        tx_header.Identifier = 0x200U;
        tag = (uint8_t)((motor->ID - 1U) * 2U);
    }
    else
    {
        tx_header.Identifier = 0x1FFU;
        tag = (uint8_t)((motor->ID - 5U) * 2U);
    }
    EncodeS16Data(&motor->valSet.current_raw, &tx_data[tag]);
    ChangeDataByte(&tx_data[tag], &tx_data[tag + 1U]);

    if (motor->ID == 4U || motor->ID == 8U)
    {
        HAL_CAN_AddMessageToTxFifoQ(DJmotor_GetCanHandle(), &tx_header, tx_data);
    }
}
