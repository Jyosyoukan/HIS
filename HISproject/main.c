#include<stdio.h>
#include"Init.h"
#include"Login_Register.h"
int main() {
	Initsystem();
	Loaddata();
	Login();
	
	return 0;
}