/*
 ============================================================================
 Name        : Cube's.c
 Author      : GarnierAndrew
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */


#include<stdio.h>
#include"app.h"

int main(int argc, char*argv[]){

	printf("<====================ENTER MOBILES=================>\n");

	t_app*pThepApp = AppCreate();
	int ret = AppRun(pThepApp);
	AppDestroy(pThepApp);

	printf("<====================EXIT MOBILE=================>");

	return  ret;
}


