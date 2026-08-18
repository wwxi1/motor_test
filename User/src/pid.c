#include "pid.h"

float PID_Caculate(PIDType *pid)
{
    pid->err[0] = pid->SetVal - pid->CurVal;

    switch (pid->mode)
    {
    case PIDINC:
        pid->output = pid->KP * (pid->err[0] - pid->err[1]) +
                      pid->KI * pid->err[0] +
                      pid->KD * (pid->err[0] - 2.0f * pid->err[1] + pid->err[2]);
        pid->err[2] = pid->err[1];
        pid->err[1] = pid->err[0];
        break;

    case PIDPOS:
        pid->err[2] = 0.5f * pid->err[0] + 0.5f * pid->err[2];
        if (ABS(pid->err[2]) > 100.0f)
        {
            pid->err[2] = GetSign(pid->err[2]) * 100.0f;
        }
        pid->output = pid->KP * pid->err[0] +
                      pid->KI * pid->err[2] +
                      pid->KD * (pid->err[0] - pid->err[1]);
        pid->err[1] = pid->err[0];
        break;

    default:
        break;
    }
    return pid->output;
}
