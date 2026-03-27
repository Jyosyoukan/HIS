#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_PATIENTS 105           // 最大病人数
#define NAME_LENGTH 25             // 名字长度
#define ID_LENGTH 25               // ID长度
#define PASSWORD_LENGTH 100         // 最长密码长度
#define MAX_OTHER 114514

typedef struct Loginf
{
    char idcard[ID_LENGTH];
    char password[PASSWORD_LENGTH];
    Loginf *next;
} Loginf;

Loginf *loginput();//turn void and return Loginf* head