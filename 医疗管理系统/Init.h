#pragma once
#include "Typedef.h"

// 系统初始化：编码+文件检测+科室/床位初始化+系统时间
int InitSystem(void);
// 加载所有本地数据：9个TXT文件→内存链表
int LoadAllData(void);
// 生成30名医生测试数据：随机生成+持久化
int GenDoctorData(void);
// 生成医生值班表：5科室×7天
int GenDoctorDuty(void);
// 生成30类药品：5科室×6类
int GenMedicineData(void);
// 检测并创建空TXT文件
int CheckAndCreateFile(const char* filename);
// 获取当前电脑系统时间
Date GetCurrentDate(void);
// 计算日期对应的周几
int GetWeekDay(Date date);
// 校验预约日期是否合法
int CheckAppointDate(Date now, Date appoint);
// 数据持久化：内存链表→TXT
int SaveDoctorToFile(void);
int SavePatientToFile(void);
int SaveLoginfToFile(void);
int SaveDoctorDutyToFile(void);
int SaveMedicineToFile(void);
int SaveRegToFile(void);
int SaveCaseToFile(void);
int SaveBedToFile(void);
int SaveExamToFile(void);
int LoadExamFromFile(void);
void FreeAllLists(void);