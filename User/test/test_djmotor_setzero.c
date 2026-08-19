/**
 * @file test_djmotor_setzero.c
 * @brief DJmotor_SetZero 函数的单元测试
 *
 * 测试目标: void DJmotor_SetZero(DJMotorPointer motor)
 *
 * 函数功能:
 *   - 将 motor->valNow.PulseTotal 设置为 0
 *   - 将 motor->valNow.angle_deg 设置为 0.0f
 *   - 将 motor->valPre 复制为 motor->valNow
 */

#include "unity_config.h"
#include "Init_test.h"
#include <math.h>
#include <string.h>

/* 全局测试电机实例 */
static DJMotor testMotor;

/* 辅助函数: 初始化测试电机为已知状态 */
static void initMotorToKnownState(DJMotor *motor)
{
    memset(motor, 0, sizeof(DJMotor));

    motor->ID = 1;
    motor->Begin = true;
    motor->MODE_Set = DJ_RPM;
    motor->MODE_Cur = DJ_RPM;

    /* 初始化 valSet */
    motor->valSet.current_raw = 100;
    motor->valSet.angle_deg = 45.5f;
    motor->valSet.speed_rpm = 500;
    motor->valSet.current_A = 2.5f;
    motor->valSet.PulseRead = 200;
    motor->valSet.PulseGap = 50;
    motor->valSet.PulseTotal = 12345;
    motor->valSet.temperature_C = 30;

    /* 初始化 valNow */
    motor->valNow.current_raw = 150;
    motor->valNow.angle_deg = 90.0f;
    motor->valNow.speed_rpm = 800;
    motor->valNow.current_A = 3.5f;
    motor->valNow.PulseRead = 400;
    motor->valNow.PulseGap = 100;
    motor->valNow.PulseTotal = 67890;
    motor->valNow.temperature_C = 40;

    /* 初始化 valPre */
    motor->valPre.current_raw = 200;
    motor->valPre.angle_deg = 180.0f;
    motor->valPre.speed_rpm = 1000;
    motor->valPre.current_A = 4.5f;
    motor->valPre.PulseRead = 600;
    motor->valPre.PulseGap = 150;
    motor->valPre.PulseTotal = 11111;
    motor->valPre.temperature_C = 50;

    /* 初始化状态标志 */
    motor->statusFlag.IsSetZero = false;
    motor->statusFlag.ZeroFlag = false;
    motor->statusFlag.StuckFlag = false;
    motor->statusFlag.Overtimeflag = false;

    /* 初始化限幅参数 */
    motor->limit.MaxAngle_deg = 270.0f;
    motor->limit.MinAngle_deg = -270.0f;
    motor->limit.SpeedRPMLimit = 10000;
    motor->limit.ZeroRPMLimit = 500;
}

/* ============================================================
 * 测试用例 1: 正常场景 - 基本功能测试
 * 测试将非零值设置为零
 * ============================================================ */
void test_DJmotor_SetZero_BasicFunction(void)
{
    /* 准备: 初始化电机为已知状态 */
    initMotorToKnownState(&testMotor);

    /* 记录调用前的 valPre 值，用于后续验证 */
    int32_t originalPulseTotal = testMotor.valPre.PulseTotal;
    float originalAngleDeg = testMotor.valPre.angle_deg;

    /* 动作: 调用被测函数 */
    DJmotor_SetZero(&testMotor);

    /* 验证: valNow.PulseTotal 应为 0 */
    TEST_ASSERT_EQUAL_INT32(0, testMotor.valNow.PulseTotal);

    /* 验证: valNow.angle_deg 应为 0.0f */
    TEST_ASSERT_EQUAL_FLOAT(0.0f, testMotor.valNow.angle_deg);

    /* 验证: valPre.valNow.PulseTotal (复制后的值) 应为 0 */
    TEST_ASSERT_EQUAL_INT32(0, testMotor.valPre.PulseTotal);

    /* 验证: valPre.valNow.angle_deg (复制后的值) 应为 0.0f */
    TEST_ASSERT_EQUAL_FLOAT(0.0f, testMotor.valPre.angle_deg);

    /* 验证: valPre 其他字段应与调用后的 valNow 同步 */
    TEST_ASSERT_EQUAL_INT16(testMotor.valNow.current_raw, testMotor.valPre.current_raw);
    TEST_ASSERT_EQUAL_INT16(testMotor.valNow.speed_rpm, testMotor.valPre.speed_rpm);
    TEST_ASSERT_EQUAL_FLOAT(testMotor.valNow.current_A, testMotor.valPre.current_A);
    TEST_ASSERT_EQUAL_INT16(testMotor.valNow.PulseRead, testMotor.valPre.PulseRead);
    TEST_ASSERT_EQUAL_INT16(testMotor.valNow.PulseGap, testMotor.valPre.PulseGap);
    TEST_ASSERT_EQUAL_INT8(testMotor.valNow.temperature_C, testMotor.valPre.temperature_C);
}

/* ============================================================
 * 测试用例 2: 边界值 - PulseTotal 为最大值 (INT32_MAX)
 * ============================================================ */
void test_DJmotor_SetZero_PulseTotal_MaxValue(void)
{
    /* 准备: 设置 PulseTotal 为 INT32_MAX */
    memset(&testMotor, 0, sizeof(DJMotor));
    testMotor.valNow.PulseTotal = INT32_MAX;
    testMotor.valNow.angle_deg = 100.0f;
    testMotor.valPre.PulseTotal = 0;

    /* 动作 */
    DJmotor_SetZero(&testMotor);

    /* 验证 */
    TEST_ASSERT_EQUAL_INT32(0, testMotor.valNow.PulseTotal);
    TEST_ASSERT_EQUAL_INT32(0, testMotor.valPre.PulseTotal);
}

/* ============================================================
 * 测试用例 3: 边界值 - PulseTotal 为最小值 (INT32_MIN)
 * ============================================================ */
void test_DJmotor_SetZero_PulseTotal_MinValue(void)
{
    /* 准备: 设置 PulseTotal 为 INT32_MIN */
    memset(&testMotor, 0, sizeof(DJMotor));
    testMotor.valNow.PulseTotal = INT32_MIN;
    testMotor.valNow.angle_deg = -100.0f;
    testMotor.valPre.PulseTotal = 100;

    /* 动作 */
    DJmotor_SetZero(&testMotor);

    /* 验证 */
    TEST_ASSERT_EQUAL_INT32(0, testMotor.valNow.PulseTotal);
    TEST_ASSERT_EQUAL_INT32(0, testMotor.valPre.PulseTotal);
}

/* ============================================================
 * 测试用例 4: 边界值 - PulseTotal 为 -1 (负数边界)
 * ============================================================ */
void test_DJmotor_SetZero_PulseTotal_NegativeOne(void)
{
    /* 准备: 设置 PulseTotal 为 -1 */
    memset(&testMotor, 0, sizeof(DJMotor));
    testMotor.valNow.PulseTotal = -1;
    testMotor.valNow.angle_deg = 0.0f;

    /* 动作 */
    DJmotor_SetZero(&testMotor);

    /* 验证 */
    TEST_ASSERT_EQUAL_INT32(0, testMotor.valNow.PulseTotal);
    TEST_ASSERT_EQUAL_INT32(0, testMotor.valPre.PulseTotal);
}

/* ============================================================
 * 测试用例 5: 边界值 - angle_deg 为正浮点最大值
 * ============================================================ */
void test_DJmotor_SetZero_AngleDeg_PositiveMax(void)
{
    /* 准备: 设置 angle_deg 为大正数 */
    memset(&testMotor, 0, sizeof(DJMotor));
    testMotor.valNow.PulseTotal = 99999;
    testMotor.valNow.angle_deg = 1e10f;

    /* 动作 */
    DJmotor_SetZero(&testMotor);

    /* 验证 */
    TEST_ASSERT_EQUAL_FLOAT(0.0f, testMotor.valNow.angle_deg);
    TEST_ASSERT_EQUAL_FLOAT(0.0f, testMotor.valPre.angle_deg);
}

/* ============================================================
 * 测试用例 6: 边界值 - angle_deg 为负浮点最小值
 * ============================================================ */
void test_DJmotor_SetZero_AngleDeg_NegativeMin(void)
{
    /* 准备: 设置 angle_deg 为大负数 */
    memset(&testMotor, 0, sizeof(DJMotor));
    testMotor.valNow.PulseTotal = -99999;
    testMotor.valNow.angle_deg = -1e10f;

    /* 动作 */
    DJmotor_SetZero(&testMotor);

    /* 验证 */
    TEST_ASSERT_EQUAL_FLOAT(0.0f, testMotor.valNow.angle_deg);
    TEST_ASSERT_EQUAL_FLOAT(0.0f, testMotor.valPre.angle_deg);
}

/* ============================================================
 * 测试用例 7: 边界值 - angle_deg 为负小数
 * ============================================================ */
void test_DJmotor_SetZero_AngleDeg_NegativeFloat(void)
{
    /* 准备: 设置 angle_deg 为负小数 */
    memset(&testMotor, 0, sizeof(DJMotor));
    testMotor.valNow.PulseTotal = 100;
    testMotor.valNow.angle_deg = -180.5f;

    /* 动作 */
    DJmotor_SetZero(&testMotor);

    /* 验证 */
    TEST_ASSERT_EQUAL_FLOAT(0.0f, testMotor.valNow.angle_deg);
    TEST_ASSERT_EQUAL_FLOAT(0.0f, testMotor.valPre.angle_deg);
}

/* ============================================================
 * 测试用例 8: 边界值 - angle_deg 为 0.0f (已经是零)
 * ============================================================ */
void test_DJmotor_SetZero_AngleDeg_AlreadyZero(void)
{
    /* 准备: angle_deg 已经是 0 */
    memset(&testMotor, 0, sizeof(DJMotor));
    testMotor.valNow.PulseTotal = 100;
    testMotor.valNow.angle_deg = 0.0f;

    /* 动作 */
    DJmotor_SetZero(&testMotor);

    /* 验证: 再次调用后仍然是 0 */
    TEST_ASSERT_EQUAL_FLOAT(0.0f, testMotor.valNow.angle_deg);
    TEST_ASSERT_EQUAL_FLOAT(0.0f, testMotor.valPre.angle_deg);
}

/* ============================================================
 * 测试用例 9: PulseTotal 已经是 0 的情况
 * ============================================================ */
void test_DJmotor_SetZero_PulseTotal_AlreadyZero(void)
{
    /* 准备: PulseTotal 已经是 0 */
    memset(&testMotor, 0, sizeof(DJMotor));
    testMotor.valNow.PulseTotal = 0;
    testMotor.valNow.angle_deg = 90.0f;

    /* 动作 */
    DJmotor_SetZero(&testMotor);

    /* 验证 */
    TEST_ASSERT_EQUAL_INT32(0, testMotor.valNow.PulseTotal);
    TEST_ASSERT_EQUAL_INT32(0, testMotor.valPre.PulseTotal);
}

/* ============================================================
 * 测试用例 10: 验证 valNow 其他字段保持不变
 * ============================================================ */
void test_DJmotor_SetZero_OtherFieldsUnchanged(void)
{
    /* 准备: 设置 valNow 的所有字段 */
    memset(&testMotor, 0, sizeof(DJMotor));
    testMotor.valNow.current_raw = 200;
    testMotor.valNow.angle_deg = 90.0f;
    testMotor.valNow.speed_rpm = 1500;
    testMotor.valNow.current_A = 5.5f;
    testMotor.valNow.PulseRead = 4095;
    testMotor.valNow.PulseGap = 50;
    testMotor.valNow.PulseTotal = 50000;
    testMotor.valNow.temperature_C = 60;

    int16_t orig_current_raw = testMotor.valNow.current_raw;
    int16_t orig_speed_rpm = testMotor.valNow.speed_rpm;
    float orig_current_A = testMotor.valNow.current_A;
    int16_t orig_PulseRead = testMotor.valNow.PulseRead;
    int16_t orig_PulseGap = testMotor.valNow.PulseGap;
    int8_t orig_temperature = testMotor.valNow.temperature_C;

    /* 动作 */
    DJmotor_SetZero(&testMotor);

    /* 验证: 除了 PulseTotal 和 angle_deg，其他字段应保持不变 */
    TEST_ASSERT_EQUAL_INT16(orig_current_raw, testMotor.valNow.current_raw);
    TEST_ASSERT_EQUAL_INT16(orig_speed_rpm, testMotor.valNow.speed_rpm);
    TEST_ASSERT_EQUAL_FLOAT(orig_current_A, testMotor.valNow.current_A);
    TEST_ASSERT_EQUAL_INT16(orig_PulseRead, testMotor.valNow.PulseRead);
    TEST_ASSERT_EQUAL_INT16(orig_PulseGap, testMotor.valNow.PulseGap);
    TEST_ASSERT_EQUAL_INT8(orig_temperature, testMotor.valNow.temperature_C);

    /* 同时验证 valPre 也同步了这些值 */
    TEST_ASSERT_EQUAL_INT16(orig_current_raw, testMotor.valPre.current_raw);
    TEST_ASSERT_EQUAL_INT16(orig_speed_rpm, testMotor.valPre.speed_rpm);
    TEST_ASSERT_EQUAL_FLOAT(orig_current_A, testMotor.valPre.current_A);
    TEST_ASSERT_EQUAL_INT16(orig_PulseRead, testMotor.valPre.PulseRead);
    TEST_ASSERT_EQUAL_INT16(orig_PulseGap, testMotor.valPre.PulseGap);
    TEST_ASSERT_EQUAL_INT8(orig_temperature, testMotor.valPre.temperature_C);
}

/* ============================================================
 * 测试用例 11: 验证 valPre 正确复制 valNow 的所有字段
 * ============================================================ */
void test_DJmotor_SetZero_ValPreCopyComplete(void)
{
    /* 准备: 设置 valNow 为特定值 */
    memset(&testMotor, 0, sizeof(DJMotor));
    testMotor.valNow.current_raw = 999;
    testMotor.valNow.angle_deg = 360.0f;
    testMotor.valNow.speed_rpm = 8192;
    testMotor.valNow.current_A = 10.123f;
    testMotor.valNow.PulseRead = 8191;
    testMotor.valNow.PulseGap = 1;
    testMotor.valNow.PulseTotal = 123456;
    testMotor.valNow.temperature_C = 85;

    /* 设置 valPre 为不同值 */
    testMotor.valPre.current_raw = 0;
    testMotor.valPre.angle_deg = 0.0f;
    testMotor.valPre.speed_rpm = 0;
    testMotor.valPre.current_A = 0.0f;
    testMotor.valPre.PulseRead = 0;
    testMotor.valPre.PulseGap = 0;
    testMotor.valPre.PulseTotal = 0;
    testMotor.valPre.temperature_C = 0;

    /* 动作 */
    DJmotor_SetZero(&testMotor);

    /* 验证: valPre 应该完全等于调用后的 valNow (全部为0) */
    TEST_ASSERT_EQUAL_INT16(testMotor.valNow.current_raw, testMotor.valPre.current_raw);
    TEST_ASSERT_EQUAL_FLOAT(testMotor.valNow.angle_deg, testMotor.valPre.angle_deg);
    TEST_ASSERT_EQUAL_INT16(testMotor.valNow.speed_rpm, testMotor.valPre.speed_rpm);
    TEST_ASSERT_EQUAL_FLOAT(testMotor.valNow.current_A, testMotor.valPre.current_A);
    TEST_ASSERT_EQUAL_INT16(testMotor.valNow.PulseRead, testMotor.valPre.PulseRead);
    TEST_ASSERT_EQUAL_INT16(testMotor.valNow.PulseGap, testMotor.valPre.PulseGap);
    TEST_ASSERT_EQUAL_INT32(testMotor.valNow.PulseTotal, testMotor.valPre.PulseTotal);
    TEST_ASSERT_EQUAL_INT8(testMotor.valNow.temperature_C, testMotor.valPre.temperature_C);
}

/* ============================================================
 * 测试用例 12: 连续调用两次
 * ============================================================ */
void test_DJmotor_SetZero_ConsecutiveCalls(void)
{
    /* 准备 */
    memset(&testMotor, 0, sizeof(DJMotor));
    testMotor.valNow.PulseTotal = 100;
    testMotor.valNow.angle_deg = 45.0f;

    /* 第一次动作 */
    DJmotor_SetZero(&testMotor);

    /* 第一次验证 */
    TEST_ASSERT_EQUAL_INT32(0, testMotor.valNow.PulseTotal);
    TEST_ASSERT_EQUAL_FLOAT(0.0f, testMotor.valNow.angle_deg);

    /* 第二次动作: 再次调用，valNow 已经是0 */
    DJmotor_SetZero(&testMotor);

    /* 第二次验证: 仍然是0 */
    TEST_ASSERT_EQUAL_INT32(0, testMotor.valNow.PulseTotal);
    TEST_ASSERT_EQUAL_FLOAT(0.0f, testMotor.valNow.angle_deg);
    TEST_ASSERT_EQUAL_INT32(0, testMotor.valPre.PulseTotal);
    TEST_ASSERT_EQUAL_FLOAT(0.0f, testMotor.valPre.angle_deg);
}

/* ============================================================
 * 测试用例 13: 特殊浮点值 - 无穷大和 NaN
 * ============================================================ */
void test_DJmotor_SetZero_AngleDeg_Infinity(void)
{
    /* 准备: 设置 angle_deg 为无穷大 */
    memset(&testMotor, 0, sizeof(DJMotor));
    testMotor.valNow.PulseTotal = 100;
    testMotor.valNow.angle_deg = INFINITY;

    /* 动作 */
    DJmotor_SetZero(&testMotor);

    /* 验证: 应被设置为 0 */
    TEST_ASSERT_EQUAL_FLOAT(0.0f, testMotor.valNow.angle_deg);
    TEST_ASSERT_EQUAL_FLOAT(0.0f, testMotor.valPre.angle_deg);
}

/* ============================================================
 * 测试用例 14: 特殊浮点值 - 负无穷大
 * ============================================================ */
void test_DJmotor_SetZero_AngleDeg_NegativeInfinity(void)
{
    /* 准备: 设置 angle_deg 为负无穷大 */
    memset(&testMotor, 0, sizeof(DJMotor));
    testMotor.valNow.PulseTotal = 100;
    testMotor.valNow.angle_deg = -INFINITY;

    /* 动作 */
    DJmotor_SetZero(&testMotor);

    /* 验证: 应被设置为 0 */
    TEST_ASSERT_EQUAL_FLOAT(0.0f, testMotor.valNow.angle_deg);
    TEST_ASSERT_EQUAL_FLOAT(0.0f, testMotor.valPre.angle_deg);
}

/* ============================================================
 * 测试用例 15: 大整数边界 - 接近 int32 最大值
 * ============================================================ */
void test_DJmotor_SetZero_PulseTotal_CloseToMax(void)
{
    /* 准备: 设置 PulseTotal 接近 INT32_MAX */
    memset(&testMotor, 0, sizeof(DJMotor));
    testMotor.valNow.PulseTotal = INT32_MAX - 1;
    testMotor.valNow.angle_deg = 270.0f;

    /* 动作 */
    DJmotor_SetZero(&testMotor);

    /* 验证 */
    TEST_ASSERT_EQUAL_INT32(0, testMotor.valNow.PulseTotal);
    TEST_ASSERT_EQUAL_INT32(0, testMotor.valPre.PulseTotal);
}

/* ============================================================
 * 测试用例 16: 极小正值浮点数
 * ============================================================ */
void test_DJmotor_SetZero_AngleDeg_VerySmallPositive(void)
{
    /* 准备: 设置 angle_deg 为非常小的正数 */
    memset(&testMotor, 0, sizeof(DJMotor));
    testMotor.valNow.PulseTotal = 1;
    testMotor.valNow.angle_deg = 1e-30f;

    /* 动作 */
    DJmotor_SetZero(&testMotor);

    /* 验证 */
    TEST_ASSERT_EQUAL_FLOAT(0.0f, testMotor.valNow.angle_deg);
    TEST_ASSERT_EQUAL_FLOAT(0.0f, testMotor.valPre.angle_deg);
}

/* ============================================================
 * 测试用例 17: 空指针保护测试 (如果函数有保护的话)
 * 注: 当前 DJmotor_SetZero 没有空指针保护
 * 如果需要保护，应该检查 motor != NULL
 * ============================================================ */

/* ============================================================
 * 测试用例 18: 验证 motor 指针解引用后的结构体成员顺序
 * 确保结构体内存布局正确
 * ============================================================ */
void test_DJmotor_SetZero_StructMemoryLayout(void)
{
    /* 准备: 填充整个结构体 */
    memset(&testMotor, 0xAA, sizeof(DJMotor));
    testMotor.valNow.PulseTotal = 123456;
    testMotor.valNow.angle_deg = 90.0f;
    testMotor.valPre.PulseTotal = 0;
    testMotor.valPre.angle_deg = 0.0f;

    /* 动作 */
    DJmotor_SetZero(&testMotor);

    /* 验证: 确保指针正确解引用并赋值 */
    TEST_ASSERT_EQUAL_INT32(0, testMotor.valNow.PulseTotal);
    TEST_ASSERT_EQUAL_FLOAT(0.0f, testMotor.valNow.angle_deg);
}

/* ============================================================
 * Unity Framework 要求的测试设置和拆卸函数
 * ============================================================ */
void setUp(void)
{
    /* 每个测试用例前调用 */
}

void tearDown(void)
{
    /* 每个测试用例后调用 */
}
