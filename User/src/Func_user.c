#include "Func_user.h"

void DJmotor_Func(void)
{
    for (uint32_t i = 0; i < USE_DJNUM; i++)
    {
        if (DJmotor[i].Begin)
        {
            // DJmotor_Monitor(&DJmotor[i]);
            DJmotor_SwitchMode(&DJmotor[i]);

            switch (DJmotor[i].MODE_Cur)
            {
            case DJ_Disable:
                DJmotor[i].valSet.current_raw = 0;
                DJmotor_CurrentTransmit(&DJmotor[i]);
                continue;
                break;
            case DJ_RPM:
                DJmotor_SpeedMode(&DJmotor[i]);
                break;
            case DJ_Position:
                DJmotor_PositionMode(&DJmotor[i]);
                break;
            case DJ_Zero:
                DJmotor_ZeroMode(&DJmotor[i]);
                break;
            case DJ_Current:
                /* 直通电流:任务层每周期写 valSet.current_raw,这里补限幅 */
                ClampPeak(DJmotor[i].valSet.current_raw, DJmotor[i].param.CurrentLimit_raw);
                break;
            default:
                break;
            }
        }

        else
        {
            /* Begin=false(未初始化/寻零完成):强制0电流,防止残留累加电流持续输出 */
            DJmotor[i].valSet.current_raw = 0;
        }
        DJmotor_CurrentTransmit(&DJmotor[i]);
    }
}

static void DJmotor_SwitchMode(DJMotorPointer motor)
{
    if (motor->MODE_Set != motor->MODE_Cur)
    {
        motor->MODE_Cur = motor->MODE_Set;
        motor->valSet.current_raw = 0;
        motor->valSet.speed_rpm = 0;
        motor->valSet.angle_deg = motor->valNow.angle_deg;
        /*清误差历史与位置环累加的目标速度(velPID.SetVal),避免残留值冲击新模式 */
        PID_Reset(&motor->posPID);
        PID_Reset(&motor->velPID);
        motor->statusFlag.ZeroFlag = false;
        motor->statusFlag.Overtimeflag = false;
        motor->statusFlag.StuckFlag = false;
    }
}