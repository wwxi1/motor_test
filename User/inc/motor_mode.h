#ifndef MOTOR_MODE
#define MOTOR_MODE



#include "Init.h"



// 速度模式（单环增量PID）
void DJmotor_SpeedMode(DJMotorPointer motor);

// 位置模式（双环混合PID）
void DJmotor_PositionMode(DJMotorPointer motor);

// 寻零模式（与机械限位配合使用）
void DJmotor_ZeroMode(DJMotorPointer motor);



#endif


