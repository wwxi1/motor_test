#ifndef  TX
#define TX 


#include "Init.h"

void DJmotor_CurrentTransmit(DJMotorPointer motor);
CAN_HandleTypeDef* DJmotor_GetCanHandle(void);

#endif

