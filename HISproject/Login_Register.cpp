#include<stdio.h>
#include<string.h>
#include<windows.h>
#include"Typedef.hpp"
#define ADMIN_USERNAME "admin"
#define ADMIN_PASSWORD "123"
int checkpassword(char * username_input,char* password_input,void* me) {
	//strcmp 
	if (  !strcmp(username_input , ADMIN_USERNAME) && !strcmp(password_input ,ADMIN_PASSWORD)) {
		//管理员登录，用于debug

		return 1;
	}
	return 0;
}

int Login(void* me) {
	me = NULL;
	char username_input[NAME_LENGTH];
	char password_input[PASSWORD_LENGTH];
	while (1) {
		printf("username:");
		scanf_s("%s", username_input,NAME_LENGTH);
		while (getchar() != '\n');
		printf("password:");
		scanf_s("%s", password_input, PASSWORD_LENGTH);
		while (getchar() != '\n');
		if (checkpassword(username_input, password_input,me) == 1) {
			printf("登录成功！欢迎！\n");
			Sleep(200);
			system("cls");
			return 2;
		}
		else {
			printf("用户名或密码错误!\n");
			for (int i = 0; i < 10; i++) {
				Sleep(500);
				printf(".");
			}
			system("cls");
		}
	}
	return 0;
};
int Register(void) {
	return 0;
};