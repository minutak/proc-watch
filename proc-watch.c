#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>


int getPID(char *app);
int statReader(int PID);

int main(int argc, char *argv[])
{
	if (argc != 2)
	{
		printf("You cant choose only one app!\n");
		return 1;
	}
	int PID = getPID(argv[1]);
	printf("PID: %d\n", PID);
	statReader(PID);
}

//do proc stat reader
int statReader(int PID)
{
	char command[100];
	sprintf(command, "cat /proc/%d/stat", PID);
	FILE *f = popen(command, "r");
	if(f == NULL) return 1;
	char buffer[2560];
	int spaces = 0;
	int utime;
	int stime;

	fgets(buffer, 2560, f);

	//find the utime and stime(14, 15)
	for(int i = 0; i < 2560; i++)
	{
		if(buffer[i] == ' ')
		{
			spaces++;
		}
		if(spaces == 13)
		{
			utime = atoi(buffer);
		}
		if(spaces == 14)
		{
			stime = atoi(buffer);
			break;
		}
	}
	printf("Stime: %d, Utime: %d\n", stime, utime);
}

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
