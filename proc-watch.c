#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int getPID(char *app);

int main(int argc, char *argv[])
{
	if (argc != 2)
	{
		printf("You cant choose only one app!\n");
		return 1;
	}
	int PID = getPID(argv[1]);
	printf("PID: %d\n", PID);
}

//do proc stat reader

int getPID(char *app)
{
	char command[100];
	sprintf(command, "pidof %s", app);
	FILE *f = popen(command, "r");
	if(f == NULL) return 1;
	
	char buffer[256];

	//Reads f command into buffer, then finding the last ' ' that is used and after that it just puts the last pid to PID variable
	fgets(buffer, 256, f);
	char *p = strrchr(buffer, ' ') + 1;
	pid_t PID = strtoul(p,NULL,10);
	
	
	pclose(f);
	return PID;
}
