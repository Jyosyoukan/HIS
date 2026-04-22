#pragma once
#include "Typedef.h"
#include "Init.h"

DoctorNode* FindDoctorById(const char* idcard);
PatientNode* FindPatientById(const char* idcard);
char* FindDutyDoctor(DeptType deptId, int weekDay); // 返回医生ID字符串
int FindMedicineByKey(const char* key, MedicineNode* res[]); // 结果存入指针数组
int FindFreeBed(DeptType deptId);          // 返回床位号
int FreeBed(int bedId);
int ReduceMedStock(int medId, int num);
int FindFreeIcuBed(void);
void PrintDate(Date date, char* str);
void PauseAndClear(void);
void ChangePassword(Loginf* user);

// 界面美化函数
void SetColor(int color);
void ResetColor(void);
void PrintHospitalBanner(void);
void PrintTitleBox(const char* title);
void PrintMenuBorder(void);
void PrintMenuBottom(void);
void PrintLoadingAnimation(int seconds);
void PrintSuccess(const char* msg);
void PrintError(const char* msg);
void PrintInfo(const char* msg);
void printProgressBar(int now, int all);
void PrintSectionLine(void);
void SetMainColor(void);
void SetAccentColor(void);
void SetNormalColor(void);
void SetBoldColor(void);
