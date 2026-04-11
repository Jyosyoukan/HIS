#include "Typedef.h"
#include "Init.h"
#include "LoginRegister.h"
#include "Menu.h"

int main() {

    system("chcp 639");
    system("cls");

    // 打印医院横幅
    PrintHospitalBanner();
    PrintInfo("系统初始化中，请稍候...");
    PrintLoadingAnimation(2);
    if (InitSystem() != 0) {
        printf("系统初始化失败！程序退出！\n"); 
        return -1;
    }
    LoadAllData();
    

    int choice;
    while (1) {
        system("cls");
        PrintHospitalBanner();
        PrintMenuBorder();
        SetColor(14);
        printf("|  1. 系统登录       2. 注册新账号       0. 退出系统          |\n");
        ResetColor();
        PrintMenuBorder();
        SetColor(11);
        printf("|  请输入操作选项 [0-2]：                                   |\n");
        ResetColor();
        printf("|  ");
        SetColor(10);
        printf("> ");
        ResetColor();

        if (scanf_s("%d", &choice) != 1) {
            system("cls");
            printf("输入错误！请输入数字！\n");
            while (getchar() != '\n');
            continue;
        }

        switch (choice) {
        case 1: {
            Loginf* user = Login();
            if (!user) break;
            if (user->role == 1) PatientMenu(user);
            else if (user->role == 2) DoctorMenu(user);
            else if (user->role == 3) AdminMenu(user);
            break;
        }
        case 2: Register(); break;
        case 0:
            system("cls");
            printf("感谢使用医院HIS管理系统！系统已安全退出！\n");
            Sleep(1000);
            return 0;
        default:
            system("cls");
            printf("无效选项！请输入0/1/2！\n");
            Sleep(500);
            break;
        }
    }
    return 0;
}