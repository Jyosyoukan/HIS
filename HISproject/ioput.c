#include "ioput.h"

Loginf *loginput()
{
    FILE *plog;
    plog = fopen("LoginRes.txt", "r");
    if (!plog)
    {
        printf("文件打开失败！");
        exit(1);
    }
    Loginf *head = (Loginf *)malloc(sizeof(Loginf));
    Loginf *now = head;
    Loginf *pre = head;
    while (fscanf(plog, "%s", now->idcard, ID_LENGTH) != EOF)
    {
        fscanf(plog, "%s", now->password, PASSWORD_LENGTH);
        pre = now;
        now = (Loginf *)malloc(sizeof(Loginf));
        pre->next = now;
    }
    free(now);
    pre->next = NULL;
    fclose(plog);
    return head;
}

void reginput()
{
    FILE *preg;
    preg = fopen("LoginRes.txt", "a");
    if (!preg)
    {
        printf("文件打开失败！");
        exit(1);
    }
    char idbri[ID_LENGTH];
    char pasbri[PASSWORD_LENGTH];
    printf("请输入身份证号:");
    scanf("%s", idbri, ID_LENGTH);
    for (int i = 0; i < strlen(idbri); i++)
    {
        if (idbri[i] < '0' || idbri[i] > '9')
        {
            printf("输入格式错误！");
            exit(2);
        }
    }
    for(Loginf * p = loginput();p != NULL;p = p->next)
    {
        if(!strcmp(idbri,p->idcard))
        {
            printf("您已经注册过了！");
            exit(3);
        }
    }
    printf("请输入密码:");
    scanf("%s", pasbri, PASSWORD_LENGTH);
    fprintf(preg,"\n");
    fprintf(preg, idbri);
    fprintf(preg,"\n");
    fprintf(preg, pasbri);
    fclose(preg);
}

// int main()
// {
//     reginput();
//     Loginf *p = loginput();
//     while (p != NULL)
//     {
//         printf("%s\n", p->idcard);
//         printf("%s\n", p->password);
//         p = p->next;
//     }
//     return 1 + 1 - 4 + 5 + 1 - 4;
// }