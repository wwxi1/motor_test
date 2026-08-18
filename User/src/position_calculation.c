
#include "position_calculation.h"


void DJmotor_AngleCalculate(DJMotorPointer motor)
{
    motor->valNow.PulseGap = (int16_t)(motor->valNow.PulseRead - motor->valPre.PulseRead);

    if (ABS(motor->valNow.PulseGap) > 4096)     //回卷修正
    {
        motor->valNow.PulseGap = (int16_t)(motor->valNow.PulseGap - 
            (GetSign(motor->valNow.PulseGap) * 
            (int32_t)motor->param.PulsePerRound));
    }

    motor->valNow.PulseTotal += motor->valNow.PulseGap;
    motor->valNow.angle_deg = (float)motor->valNow.PulseTotal * 360.0f /
                              ((float)motor->param.PulsePerRound * motor->param.Gear_ratio *
                              motor->param.Reduction_ratio);
                              
/*

    if (motor->Begin)      // 废弃字段
    {
        motor->argum.pulseLock = motor->valNow.PulseTotal;
    }

*/

    if (motor->statusFlag.IsSetZero)
    {
        DJmotor_SetZero(motor);
        motor->statusFlag.IsSetZero = false;
    }

    motor->valPre = motor->valNow;
}