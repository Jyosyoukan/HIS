#include<stdio.h>
#include"Typedef.hpp"
#include"Init.h"
#include"Login_Register.h"
#include"Admin.h"
#include"Doctor.h"
#include"Patient.h"
int main() {
	Initsystem();
	Loaddata();
	void* me = NULL;
	int Logincode=Login(me);
	if (Logincode == 1) {
		//患者
		PatientPage((Patient*)me);
	}
	if(Logincode ==2) {
		//医生
		DoctorPage((Doctor*)me);
	}
	if(Logincode==3) {
		//药品管理员
		AdminPage((Admin*) me);
	}
	
	return 0;
}