#pragma once
#include "Typedef.h"

Loginf* Login(void);
int Register(void);
Loginf* CheckLogin(const char* idcard, const char* password);
int IsIdExist(const char* idcard);