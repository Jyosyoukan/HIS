#pragma once
#include "Typedef.h"

// 登录函数：返回登录用户的Loginf指针，NULL为登录失败
Loginf* Login(void);
// 注册函数：1-注册成功 0-注册失败 -1-输入错误
int Register(void);
// 登录验证：根据身份证+密码查找用户，返回Loginf指针
Loginf* CheckLogin(const char* idcard, const char* password);
// 验证身份证是否已存在：1-存在 0-不存在
int IsIdExist(const char* idcard);