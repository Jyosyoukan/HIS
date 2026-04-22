#include "Typedef.h"
#include "Init.h"
#include "LoginRegister.h"
#include "Menu.h"

int main() {
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD mode;
    GetConsoleMode(hOut, &mode);
    SetConsoleMode(hOut, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
    system("chcp 639");
    system("cls");

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
        printf("|  [1] 系统登录  [2] 注册新账号  [3]关于本院   [0] 退出系统     |\n");
        ResetColor();
        PrintMenuBorder();
        SetColor(11);
        printf("|  请输入操作选项 [0-3]：                                   |\n");
        ResetColor();
        printf("|  ");
        SetColor(10);
        printf("> ");
        ResetColor();

        if (scanf_s("%d", &choice) != 1) {
            
            PrintError("输入错误！请输入数字！\n");
            PauseAndClear();
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
            //Login 返回的 admin 是动态分配的，需要释放
            if (strcmp(user->idcard, ADMIN_ID) == 0) free(user);
            break;
        }
        case 2: Register(); break;
		case 3: About(); break;
        case 0:
            system("cls");
            printf("感谢使用医院HIS管理系统！系统已安全退出！\n");
            FreeAllLists();  //释放所有链表内存
            Sleep(1000);
            return 0;
        default:
            
            printf("无效选项！请输入0/1/2/3！\n");
            PauseAndClear();
            break;
        }
    }
    return 0;
}