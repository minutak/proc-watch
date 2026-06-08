#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>

#define hertz (sysconf(_SC_CLK_TCK))

int utime;
int stime;
int cutime;
int cstime;
int startTime;
int upTime;

int getPID(char *app);
int statReader(int PID);
float calculation();

//Need to make cpu usage in some time interval

int main(int argc, char *argv[])
{
	if (argc != 2)
	{
		printf("You can choose only one app!\n");
		return 1;
	}

	int PID = getPID(argv[1]);
	printf("PID: %d\n", PID);
	
	while(true)
	{
	statReader(PID);
	float usage1 = utime + stime;
	sleep(1);
	statReader(PID);
	float usage2 = utime + stime;

	float cpuUsage = ((usage2 - usage1) / hertz) / 1 * 100;
	printf("CPU usage: %f%\n", cpuUsage);
	}
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

	fgets(buffer, 2560, f);

	//find the utime and stime(14, 15)
	for(int i = 0; i < 2560; i++)
	{	
		if(spaces == 12)
		{
			utime = atoi(&buffer[i]);
		}
		if(spaces == 13)
		{
			stime = atoi(&buffer[i]);
		}
		if(spaces == 14)
		{
			cutime = atoi(&buffer[i]);
		}
		if (spaces == 16)
		{
			cstime = atoi(&buffer[i]);
		}
		if(spaces == 21)
		{
			startTime = atoi(&buffer[i]);
			break;
		}
		if(isspace(buffer[i]))
		{
			spaces++;
		}	
	}
	//printf("Utime: %d, Stime: %d, Cutime: %d, Cstime: %d, startTime: %d, Hertz: %ld\n", utime, stime, cutime, cstime, startTime, hertz);
	return 0;
}

float calculation()
{
	float totalTime;
	float elapsedTime;

	elapsedTime = upTime - (startTime / hertz);

	totalTime = utime + stime;
	totalTime += cutime + cstime;
	
	return ((totalTime / hertz) / elapsedTime);
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
