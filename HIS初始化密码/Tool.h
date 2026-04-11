#pragma once
#include "Typedef.h"
#include "Init.h"

// 根据身份证号查找医生：返回医生索引，-1为未找到
int FindDoctorById(const char* idcard);
// 根据科室+周几查找值班医生ID：返回医生ID字符串，NULL为未找到
char* FindDutyDoctor(DeptType deptId, int weekDay);
// 根据药品名/别名检索药品：返回药品索引数组，-1结束
int FindMedicineByKey(const char* key, int res[]);
// 根据身份证号查找患者：返回患者索引，-1为未找到
int FindPatientById(const char* idcard);
// 根据科室查找空闲床位：返回第一个空闲床位号，-1为无床位
int FindFreeBed(DeptType deptId);
// 释放床位：根据床位号释放，更新床位状态+科室空闲数
int FreeBed(int bedId);
// 扣减药品库存：药品ID+数量，库存不足返回0，成功返回1
int ReduceMedStock(int medId, int num);
// 格式化打印日期：Date→字符串
void PrintDate(Date date, char* str);
// 清屏+等待按键：美化界面
void PauseAndClear(void);
// ===================== 新增界面美化函数 =====================
void SetColor(int color);                     // 设置控制台文字颜色
void ResetColor(void);                        // 恢复默认颜色
void PrintHospitalBanner(void);               // 打印彩色医院大艺术字
void PrintTitleBox(const char* title);        // 打印带边框的标题框
void PrintMenuBorder(void);                   // 打印菜单上边框
void PrintMenuBottom(void);                   // 打印菜单下边框
void PrintLoadingAnimation(int seconds);      // 加载动画（秒数）
void PrintSuccess(const char* msg);           // 成功提示（绿色）
void PrintError(const char* msg);             // 错误提示（红色）
void PrintInfo(const char* msg);              // 信息提示（黄色）