#include "LoginRegister.h"
#include "Init.h"

int IsIdExist(const char* idcard) {
    if (strcmp(idcard, ADMIN_ID) == 0) return 1;
    Loginf* p = userroot;
    while (p != NULL) {
        if (strcmp(p->idcard, idcard) == 0) return 1;
        p = p->next;
    }
    return 0;
}

Loginf* CheckLogin(const char* idcard, const char* password) {
    if (strcmp(idcard, ADMIN_ID) == 0 && strcmp(password, ADMIN_PWD) == 0) {
        Loginf* admin = (Loginf*)malloc(sizeof(Loginf));
        strcpy_s(admin->idcard, ID_LENGTH, ADMIN_ID);
        strcpy_s(admin->password, PASSWORD_LENGTH, ADMIN_PWD);
        admin->role = ADMIN_ROLE;
        admin->next = NULL;
        return admin;
    }
    Loginf* p = userroot;
    while (p != NULL) {
        if (strcmp(p->idcard, idcard) == 0 && strcmp(p->password, password) == 0) {
            return p;
        }
        p = p->next;
    }
    return NULL;
}

int Register(void) {
    system("cls");
    PrintTitleBox("新用户注册");
    PrintInfo("请选择注册身份：1-患者  2-医生  3-管理员  (0 取消)");
    int roleChoice = -1;
    while (1) {
        printf("输入选项：");
        if (scanf_s("%d", &roleChoice) != 1) {
            int ch;
            while ((ch = getchar()) != '\n' && ch != EOF) {}
            PrintError("输入无效，请重试。");
            continue;
        }
        if (roleChoice == 0) {
            system("cls");
            return 0;
        }
        if (roleChoice >= 1 && roleChoice <= 3) break;
        PrintError("请选择 1/2/3 或 0 取消。");
    }
    if (roleChoice != 1) {
        PrintInfo("请输入企业统一验证码：");
        char checkcodeinput[20];
        scanf_s("%s", checkcodeinput, 20);
        if (strcmp(checkcodeinput, CHACKCODE) != 0) {
            PrintError("验证码错误！注册失败！");
            Sleep(1500);
            system("cls");
            return 0;
        }
    }
    Loginf newUser;
    memset(&newUser, 0, sizeof(newUser));
    newUser.next = NULL;

    while (1) {
        printf("请输入18位身份证号：");
        scanf_s("%s", newUser.idcard, (unsigned)_countof(newUser.idcard));
        if (strlen(newUser.idcard) != 18) {
            PrintError("身份证号必须为18位！");
            continue;
        }
        if (IsIdExist(newUser.idcard)) {
            PrintError("该身份证号已注册！请直接登录或使用其它身份证号。");
            Sleep(1500);
            return 0;
        }
        break;
    }

    while (1) {
        printf("请输入密码（6-16位）：");
        scanf_s("%s", newUser.password, (unsigned)_countof(newUser.password));
        if (strlen(newUser.password) < 6 || strlen(newUser.password) > 16) {
            PrintError("密码长度必须6-16位！");
            continue;
        }
        break;
    }

    if (roleChoice == 1) {
        if (patCnt >= MAX_PATIENTS) { PrintError("患者数量已满，无法注册。"); return 0; }
        Patient newPat;
        memset(&newPat, 0, sizeof(newPat));
        printf("请输入患者姓名：");
        scanf_s("%s", newPat.name, (unsigned)_countof(newPat.name));
        while (1) {
            printf("请输入性别（M-男 F-女）：");
            scanf_s(" %c", &newPat.gender, 1);
            if (newPat.gender == 'M' || newPat.gender == 'F' || newPat.gender == 'm' || newPat.gender == 'f') {
                newPat.gender = toupper(newPat.gender);
                break;
            }
            PrintError("性别输入错误！请输入M/F！");
        }
        strcpy_s(newPat.idcard, ID_LENGTH, newUser.idcard);
        newPat.isHospitalized = 0;
        newPat.bedId = 0;
        newPat.inDate.year = newPat.inDate.month = newPat.inDate.day = 0;
        newPat.outDate.year = newPat.outDate.month = newPat.outDate.day = 0;
        newPat.medicalRecord[0] = '\0';
        newUser.role = 1;
        patients[patCnt++] = newPat;
    }
    else if (roleChoice == 2) {
        if (docCnt >= MAX_DOCTORS) { PrintError("医生数量已满，无法注册。"); return 0; }
        Doctor newDoc;
        memset(&newDoc, 0, sizeof(newDoc));
        strcpy_s(newDoc.idcard, ID_LENGTH, newUser.idcard);
        printf("请输入医生姓名：");
        scanf_s("%s", newDoc.name, (unsigned)_countof(newDoc.name));
        PrintInfo("请选择科室：1-内科 2-外科 3-妇产科 4-儿科 5-骨科");
        int dept = 0;
        while (1) {
            printf("输入科室编号(1-5)：");
            if (scanf_s("%d", &dept) != 1) { int ch; while ((ch = getchar()) != '\n' && ch != EOF) {} PrintError("输入无效。"); continue; }
            if (dept >= 1 && dept <= 5) break;
            PrintError("请输入1~5。");
        }
        newDoc.deptId = (DeptType)dept;
        printf("请输入职称（例如 主任医师/主治医师 等）：");
        scanf_s("%s", newDoc.title, (unsigned)_countof(newDoc.title));
        printf("请输入擅长（简短描述）：");
        scanf_s("%s", newDoc.specialty, (unsigned)_countof(newDoc.specialty));
        sprintf_s(newDoc.info, MAX_INFO, "%s医生，擅长%s。", newDoc.name, newDoc.specialty);
        newUser.role = 2;
        doctors[docCnt++] = newDoc;
    }
    else {
        newUser.role = 3;
    }

    if (userroot == NULL) {
        userroot = (Loginf*)malloc(sizeof(Loginf));
        if (!userroot) { PrintError("内存分配失败！"); return 0; }
        *userroot = newUser;
        userroot->next = NULL;
    }
    else {
        Loginf* p = userroot;
        while (p->next != NULL) p = p->next;
        p->next = (Loginf*)malloc(sizeof(Loginf));
        if (!p->next) { PrintError("内存分配失败！"); return 0; }
        *(p->next) = newUser;
        p->next->next = NULL;
    }

    if (roleChoice == 1) SavePatientToFile();
    if (roleChoice == 2) SaveDoctorToFile();
    SaveLoginfToFile();

    PrintSuccess("注册成功！");
    SetColor(11);
    printf("身份：%s | 身份证：%s\n",
        roleChoice == 1 ? "患者" : roleChoice == 2 ? "医生" : "管理员",
        newUser.idcard);
    ResetColor();
    PrintInfo("请牢记密码！");
    Sleep(2000);
    system("cls");
    return 1;
}

Loginf* Login(void) {
    system("cls");
    char idcard[ID_LENGTH], password[PASSWORD_LENGTH];
    while (1) {
        PrintHospitalBanner();
        PrintTitleBox("系统登录");
        PrintInfo("【药品管理员】账号：admin | 密码：123456");
        PrintInfo("【医生】账号：查看DoctorData.txt | 密码：123456");
        PrintInfo("【患者】账号：注册的身份证号 | 密码：注册密码");
        PrintMenuBorder();
        printf("请输入身份证号/管理员账号：");
        scanf_s("%s", idcard, (unsigned)_countof(idcard));
        printf("请输入密码：");
        scanf_s("%s", password, (unsigned)_countof(password));

        Loginf* user = CheckLogin(idcard, password);
        if (user != NULL) {
            MessageBeep(MB_OK);
            PrintSuccess("登录成功！");
            if (user->role == 1) PrintInfo("欢迎您，患者！");
            else if (user->role == 2) PrintInfo("欢迎您，医生！");
            else if (user->role == 3) PrintInfo("欢迎您，药品管理员！");
            Sleep(1200);
            system("cls");
            return user;
        }
        else {
            MessageBeep(MB_ICONERROR);
            PrintError("身份证号/密码错误！");
            PrintInfo("按任意键重新输入...(或按0返回主菜单)");
            if (_getch() == '0') {
                system("cls");
                return NULL;
            }
            system("cls");
        }
    }
}