

#include "motor_mode.h"

// 速度模式（单环增量PID）
void DJmotor_SpeedMode(DJMotorPointer motor)
{
    motor->velPID.SetVal = (float)motor->valSet.speed_rpm * motor->param.Gear_ratio * motor->param.Reduction_ratio;
    motor->velPID.CurVal = (float)motor->valNow.speed_rpm * motor->param.Gear_ratio * motor->param.Reduction_ratio;

    if (motor->limit.RPMLimitFlag)
    {
        motor->velPID.SetVal = ClampPeak(motor->velPID.SetVal, motor->limit.SpeedRPMLimit);
    }
    motor->valSet.current_raw += PID_Caculate(&motor->velPID);
    motor->valSet.current_raw = (int16_t)ClampPeak(motor->valSet.current_raw, motor->param.CurrentLimit_raw);
}

// 位置模式（双环混合PID）

void DJmotor_PositionMode(DJMotorPointer motor)
{
    motor->valSet.PulseTotal = (int32_t)(motor->valSet.angle_deg * motor->param.Gear_ratio *
                                         motor->param.Reduction_ratio * (float)motor->param.PulsePerRound / 360.0f);
    motor->posPID.SetVal = (float)motor->valSet.PulseTotal;
    if (motor->limit.PosAngleLimitFlag)
    {
        const int32_t max_pulse = (int32_t)(motor->limit.MaxAngle_deg *
                                            (float)motor->param.PulsePerRound *
                                            motor->param.Gear_ratio * motor->param.Reduction_ratio / 360.0f);
        const int32_t min_pulse = (int32_t)(motor->limit.MinAngle_deg *
                                            (float)motor->param.PulsePerRound *
                                            motor->param.Gear_ratio * motor->param.Reduction_ratio / 360.0f);

        motor->posPID.SetVal = Clamp(motor->valSet.PulseTotal, min_pulse, max_pulse);
    }
    motor->posPID.CurVal = (float)motor->valNow.PulseTotal;
    motor->velPID.SetVal = PID_Caculate(&motor->posPID);
    motor->velPID.CurVal = (float)motor->valNow.speed_rpm * motor->param.Gear_ratio * motor->param.Reduction_ratio;

    if (motor->limit.PosRPMFlag)
    {
        motor->velPID.SetVal = ClampPeak(motor->velPID.SetVal, motor->limit.PosRPMLimit);
    }

    motor->valSet.current_raw += PID_Caculate(&motor->velPID);
    motor->valSet.current_raw = (int16_t)ClampPeak(motor->valSet.current_raw, motor->param.CurrentLimit_raw);
}

// 寻零模式（与机械限位配合使用）

void DJmotor_ZeroMode(DJMotorPointer motor)
{
    motor->velPID.SetVal = (float)motor->limit.ZeroRPMLimit;
    motor->velPID.CurVal = (float)motor->valNow.speed_rpm;
    motor->valSet.current_raw += PID_Caculate(&motor->velPID);
    motor->valSet.current_raw = (int16_t)ClampPeak(motor->valSet.current_raw, motor->limit.ZeroCurrentLimit_raw);

    if (ABS(motor->valNow.PulseGap) < Zero_Distance)
    {
        if (motor->argum.zeroCnt++ > 100U)
        {
            motor->argum.zeroCnt = 0;
            motor->statusFlag.ZeroFlag = true;
            motor->Begin = false;

            /* 寻零结束不走 SwitchMode,这里手动清 PID历史,重新使能时从零起步 */

            PID_Reset(&motor->posPID);
            PID_Reset(&motor->velPID);
            DJmotor_SetZero(motor);
        }
    }
}