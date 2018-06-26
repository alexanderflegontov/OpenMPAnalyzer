#include <stdlib.h> // system
#include <stdio.h> // printf
#include <string> // string

/*
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <cstring>
#include <string.h>

#include <stddef.h>
#include <time.h>
#include <sys/sysinfo.h>

#include <string.h> // memset
#include <iomanip>
*/

int main(int argc, char* argv[]){

	// print all args
	printf("print all args [%d]\n", argc);
	for(int i = 0; i < argc; i++){
		printf("~~%s \n", argv[i]);	
	}

	printf("HI~~!!!\n");

	//system("dir");
	std::string str = "";
	if(argc >= 4){
		//argv[1] the path to pin
		//argv[2] the path to pintool
		//argv[3] the path to app
		//argv[4..] the args in app 
		str = str + argv[1] + " -t " + argv[2] + " -- " + argv[3];
		// Addition args of the called app
		if(argc >= 5){
			for(int i = 4; i < argc; i++){
				str = str + " " +  argv[i];	
			}	
		}
	}else{
		printf("~~ \nThere are not enough arguments to run \n");		
	}

	// Sammarize
	printf("~~ \nSammarize:\n %s \n\n", str.c_str());

	// Call the app
	system(str.c_str());

	printf("BYE~~!!!\n");

	return 0;
}
