#include "alarm.h"

static void DJmotor_Monitor(DJMotorPointer motor)
{

    if (motor->valNow.PulseGap < 5 &&motor->valNow.current_raw > 3000)
    {
        if (motor->error.stuckCount++ > 500U)
        {
            motor->error.stuckCount = 0;
            motor->statusFlag.StuckFlag = true;
            if (motor->limit.IsLooseStuck)
            {
                motor->MODE_Set = DJ_Disable;
            }
        }
    }

    else
    {
        motor->error.stuckCount = 0;
    }
    if (motor->error.lastRxTime++ > 50U)
    {

        if(motor->error.timeoutCount++ > 20U)
        {
            motor->error.timeoutCount = 0;
            motor->MODE_Set = DJ_Disable;
            motor->statusFlag.Overtimeflag = true;
        }
    }
}