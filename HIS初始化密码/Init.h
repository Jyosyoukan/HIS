#pragma once
#include "Typedef.h"


// 系统初始化：编码+文件检测+科室/床位初始化+系统时间
int InitSystem(void);
// 加载所有本地数据：9个TXT文件→内存数组/链表
int LoadAllData(void);
// 生成30名医生测试数据：随机生成+持久化到DoctorData.txt/LoginRes.txt
int GenDoctorData(void);
// 生成医生值班表：5科室×7天，每日每科室1名医生，持久化到DoctorDuty.txt
int GenDoctorDuty(void);
// 生成30类药品：5科室×6类，随机生成名称/别名/价格，持久化到MedData.txt
int GenMedicineData(void);
// 检测并创建空TXT文件：防止文件不存在导致fopen_s失败
int CheckAndCreateFile(const char* filename);
// 获取当前电脑系统时间：返回Date结构体（核心，全系统时间基准）
Date GetCurrentDate(void);
// 计算日期对应的周几：输入Date，返回1-7（周一至周日）
int GetWeekDay(Date date);
// 校验预约日期是否合法：未来7天内，非过去日期
int CheckAppointDate(Date now, Date appoint);
// 数据持久化：内存→TXT（实时调用，保证数据不丢失）
int SaveDoctorToFile(void);
int SavePatientToFile(void);
int SaveLoginfToFile(void);
int SaveDoctorDutyToFile(void);
int SaveMedicineToFile(void);
int SaveRegToFile(void);
int SaveCaseToFile(void);
int SaveBedToFile(void);