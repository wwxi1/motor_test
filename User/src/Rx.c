#include "Rx.h"
#include "can.h"
void DJmotor_Receive(CAN_RxHeaderTypeDef Rxheader, uint8_t *Rx_data)
{
    if ((Rxheader.IDE != CAN_ID_STD) ||
        (Rxheader.RTR !=  CAN_RTR_DATA) ||
        (Rxheader. StdId < 0x201U) || (Rxheader.StdId  > 0x208U))
    {
        return;
    }

    uint8_t card_id = (uint8_t)(Rxheader.StdId - 0x200U); /* 1 .. 8 */

    if (card_id > USE_DJNUM)
    {
        return;
    }

    DJMotorPointer motor = &DJmotor[card_id - 1U];

    motor->valNow.PulseRead = (int16_t)(((uint16_t)Rx_data[0] << 8) | Rx_data[1]);
    motor->valNow.speed_rpm = (int16_t)(((uint16_t)Rx_data[2] << 8) | Rx_data[3]);
    motor->valNow.current_raw = (int16_t)(((uint16_t)Rx_data[4] << 8) | Rx_data[5]);

    if (motor->param.Reduction_ratio == M3508_RATIO)
    {
        motor->valNow.temperature_C = (int8_t)Rx_data[6];
        motor->valNow.current_A = (float)motor->valNow.current_raw * 0.0012207f;
    }
    else
    {
        motor->valNow.current_A = (float)motor->valNow.current_raw / 10000.0f * 10.0f;
    }

    motor->valNow.speed_rpm /= (motor->param.Gear_ratio * motor->param.Reduction_ratio);

    motor->error.lastRxTime = 0;
    DJmotor_AngleCalculate(motor);
}

void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
    uint8_t rx_data[8];
    CAN_RxHeaderTypeDef rx_header;

    if (hcan->Instance == CAN1)
    {
        if (HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &rx_header, rx_data) == HAL_OK)
        {
            DJmotor_Receive(rx_header, rx_data);
        }
    }
}


