#include<stdio.h>
#include<windows.h>
#include<time.h>
#include"Typedef.hpp"
void cls() {
	system("cls");
}
int Initsystem(void) {
	system("chcp 65001");//支持中文显示
	printf("int Initsystem(void) ok\n");
	time_t now = time(NULL);
	struct tm t;
	localtime_s(&t, &now);
	int year = t.tm_year += 1900; // tm_year 是从 1900 年开始的
	int month = t.tm_mon + 1;    // tm_mon 是从 0 开始的
	int day = t.tm_mday;
	int hour = t.tm_hour;
	int minute = t.tm_min;
	printf("当前日期和时间：%d-%02d-%02d %02d:%02d\n", year, month, day, hour, minute);
	Sleep(2000);
	//system("cls");
	return 0;
}
int Loaddata(void) {
	printf("int Loaddata(void) ok\n");


	system("cls");
	return 0;
}
