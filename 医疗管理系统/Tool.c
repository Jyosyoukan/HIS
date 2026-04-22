#include "Tool.h"

DoctorNode* FindDoctorById(const char* idcard) {
    DoctorNode* p = doctorHead;
    while (p != NULL) {
        if (strcmp(p->idcard, idcard) == 0) return p;
        p = p->next;
    }
    return NULL;
}

PatientNode* FindPatientById(const char* idcard) {
    PatientNode* p = patientHead;
    while (p != NULL) {
        if (strcmp(p->idcard, idcard) == 0) return p;
        p = p->next;
    }
    return NULL;
}

char* FindDutyDoctor(DeptType deptId, int weekDay) {
    DutyNode* p = dutyHead;
    while (p != NULL) {
        if (p->deptId == deptId && p->weekDay == weekDay) {
            return p->doctorId;
        }
        p = p->next;
    }
    return NULL;
}

int FindMedicineByKey(const char* key, MedicineNode* res[]) {
    int idx = 0;
    char keyLow[NAME_LENGTH];
    strcpy_s(keyLow, NAME_LENGTH, key);
    for (int i = 0; keyLow[i]; i++) keyLow[i] = (char)tolower(keyLow[i]);

    MedicineNode* p = medicineHead;
    while (p != NULL) {
        char medNameLow[NAME_LENGTH], medAliasLow[MAX_MED_ALIAS];
        strcpy_s(medNameLow, NAME_LENGTH, p->medName);
        strcpy_s(medAliasLow, MAX_MED_ALIAS, p->medAlias);
        for (int j = 0; medNameLow[j]; j++) medNameLow[j] = (char)tolower(medNameLow[j]);
        for (int j = 0; medAliasLow[j]; j++) medAliasLow[j] = (char)tolower(medAliasLow[j]);
        if (strstr(medNameLow, keyLow) || strstr(medAliasLow, keyLow)) {
            if (idx < MAX_MEDICINE) res[idx++] = p;
        }
        p = p->next;
    }
    res[idx] = NULL;
    return idx;
}

int FindFreeBed(DeptType deptId) {
    BedNode* p = bedHead;
    while (p != NULL) {
        if (p->deptId == deptId && p->isFree == 1) {
            return p->bedId;
        }
        p = p->next;
    }
    return -1;
}

int FindFreeIcuBed(void) {
    BedNode* p = bedHead;
    while (p != NULL) {
        if (p->roomType == 2 && p->isFree == 1) {
            return p->bedId;
        }
        p = p->next;
    }
    return -1;
}

int FreeBed(int bedId) {
    BedNode* p = bedHead;
    while (p != NULL) {
        if (p->bedId == bedId) {
            DeptType dept = p->deptId;
            p->isFree = 1;
            p->patientId[0] = '\0';
            memset(&p->inDate, 0, sizeof(Date));
            if (dept >= NEIKE && dept <= GUKE) {
                depts[dept - 1].freeBed++;
            }
            SaveBedToFile();
            return 1;
        }
        p = p->next;
    }
    return 0;
}

int ReduceMedStock(int medId, int num) {
    MedicineNode* p = medicineHead;
    while (p != NULL) {
        if (p->medId == medId) {
            if (p->stock < num) return 0;
            p->stock -= num;
            SaveMedicineToFile();
            return 1;
        }
        p = p->next;
    }
    return 0;
}

void PrintDate(Date date, char* str) {
    sprintf_s(str, 20, "%d年%d月%d日", date.year, date.month, date.day);
}

void PauseAndClear(void) {
    printf("\n按任意键继续...");
    _getch();
    system("cls");
}


void SetColor(int color) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, color);
}
void ResetColor(void) { SetColor(7); }
void printProgressBar(int now, int all) {
    if (all <= 0) return;
    if (now < 0) now = 0;
    if (now > all) now = all;
    printf("\n");
    int percent = (now * 100) / all;
    int barWidth = 30;
    int pos = (now * barWidth) / all;
    const char* color;
    if (percent < 30) color = "\033[1;33m";
    else if (percent < 70) color = "\033[1;34m";
    else color = "\033[1;32m";
    printf("[");
    for (int i = 0; i < barWidth; i++) {
        if (i < pos) printf("%s██\033[0m", color);
        else printf("██");
    }
    printf("] %3d%%", percent);
    printf("\n");
    fflush(stdout);
}
void PrintHospitalBanner(void) {
    SetColor(11);
    printf("\n  ██╗  ██╗ ██████╗ ███████╗██████╗ ██╗████████╗ █████╗ ██╗\n");
    printf("  ██║  ██║██╔═══██╗██╔════╝██╔══██╗██║╚══██╔══╝██╔══██╗██║\n");
    printf("  ███████║██║   ██║███████╗██████╔╝██║   ██║   ███████║██║\n");
    printf("  ██╔══██║██║   ██║╚════██║██╔═══╝ ██║   ██║   ██╔══██║██║\n");
    printf("  ██║  ██║╚██████╔╝███████║██║     ██║   ██║   ██║  ██║███████╗\n");
    printf("  ╚═╝  ╚═╝ ╚═════╝ ╚══════╝╚═╝     ╚═╝   ╚═╝   ╚═╝  ╚═╝╚══════╝\n\n");
    SetColor(14);
    printf("            \x1b[48;2;255;200;50m\x1b[38;2;255;255;255m                                           \x1b[0m\n");
    printf("            \x1b[48;2;255;200;50m\x1b[38;2;0;0;0m        HIS 医院综合管理系统  v1.0         \x1b[0m\n");
    printf("            \x1b[48;2;255;200;50m\x1b[38;2;255;255;255m                                           \x1b[0m\n");
    ResetColor();
    printf("\n");
}
void PrintTitleBox(const char* title) {
    int len = (int)strlen(title);
    SetColor(10);
    printf("+");
    for (int i = 0; i < len + 4; i++) printf("-");
    printf("+\n|  %s  |\n+", title);
    for (int i = 0; i < len + 4; i++) printf("-");
    printf("+\n");
    ResetColor();
}
void PrintMenuBorder(void) {
    SetColor(9);
    printf("+");
    for (int i = 0; i < 62; i++) printf("-");
    printf("+\n");
    ResetColor();
}
void PrintMenuBottom(void) {
    SetColor(9);
    printf("+");
    for (int i = 0; i < 62; i++) printf("-");
    printf("+\n");
    ResetColor();
}
void PrintLoadingAnimation(int seconds) {
    char spin[4] = { '|', '/', '-', '\\' };
    printf("\n  ");
    for (int i = 0; i < seconds * 5; i++) {
        printf("\b%c", spin[i % 4]);
        Sleep(200);
    }
    printf("\b \n");
}
void ChangePassword(Loginf* user) {
    system("cls");
    printf("===================== 修改密码 =====================\n");

    // 1. 验证旧密码
    char oldPassword[PASSWORD_LENGTH];
    printf("请输入旧密码（按0返回）：");
    scanf_s("%s", oldPassword, (unsigned)_countof(oldPassword));

    if (strcmp(oldPassword, "0") == 0) {
        system("cls");
        return;
    }

    if (strcmp(oldPassword, user->password) != 0) {
        PrintError("旧密码输入错误！\n");
        PauseAndClear();
        return;
    }

    // 2. 输入新密码
    char newPassword[PASSWORD_LENGTH];
    while (1) {
        printf("请输入新密码（6-15位）：");
        scanf_s("%s", newPassword, (unsigned)_countof(newPassword));

        if (strlen(newPassword) < 6 || strlen(newPassword) >= PASSWORD_LENGTH) {
            printf("\n密码长度必须6-15位！按任意键重新输入(按0返回)\n");
            if (_getch() == '0') {
                system("cls");
                return;
            }
            continue;
        }
        break;
    }

    // 3. 二次验证新密码
    while (1) {
        char confirmpassword[PASSWORD_LENGTH];
        printf("请再次输入新密码（6-15位）：");
        scanf_s("%s", confirmpassword, (unsigned)_countof(confirmpassword));

        if (strcmp(confirmpassword, newPassword) != 0) {
            printf("\n两次密码不一致！按任意键重新输入(按0返回)\n");
            if (_getch() == '0') {
                system("cls");
                return;
            }
            continue;
        }
        break;
    }

    // 4. 保存新密码
    strcpy_s(user->password, PASSWORD_LENGTH, newPassword);

    // 调用现有的文件保存函数，覆盖保存登录链表信息
    SaveLoginfToFile();

    printf("\n=======================================================\n");
    printf("密码修改成功！请牢记新密码。\n");
    PauseAndClear();
}
void PrintSuccess(const char* msg) { SetColor(10); printf("[√] %s\n", msg); ResetColor(); }
void PrintError(const char* msg) { SetColor(12); printf("[×] %s\n", msg); ResetColor(); }
void PrintInfo(const char* msg) { SetColor(14); printf("[i] %s\n", msg); ResetColor(); }
void PrintSectionLine() { SetAccentColor(); printf("\n═══════════════════════════════════════════════════════════════════════════════\n"); SetNormalColor(); }
void SetMainColor() { SetColor(11); }
void SetAccentColor() { SetColor(14); }
void SetNormalColor() { SetColor(8); }
void SetBoldColor() { SetColor(15); }