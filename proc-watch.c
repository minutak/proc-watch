#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>

#define hertz (sysconf(_SC_CLK_TCK))

struct stats{
	int utime;
	int stime;
	int cutime;
	int cstime;
};

int getPID(char *app);
struct stats statReader(int PID);
float calculation(struct stats stat);

int main(int argc, char *argv[])
{
	if (argc != 2)
	{
		printf("You can choose only one app!\n");
		return 1;
	}

	struct stats stat;
	int PID = getPID(argv[1]);
	printf("PID: %d\n", PID);
	
	while(true)
	{
	stat = statReader(PID);
	float usage1 = calculation(stat);
	sleep(1);
	stat = statReader(PID);
	float usage2 = calculation(stat);

	float cpuUsage = ((usage2 - usage1) / hertz) / 1 * 100;
	printf("CPU usage: %.2f%%\n", cpuUsage);
	}
}

//do proc stat reader
struct stats statReader(int PID)
{
	struct stats stat;
	char command[100];
	sprintf(command, "cat /proc/%d/stat", PID);
	FILE *f = popen(command, "r");
	if(f == NULL) return stat;
	char buffer[2560];
	int spaces = 0;

	fgets(buffer, 2560, f);

	//find the utime and stime(14, 15)
	for(int i = 0; i < 2560; i++)
	{	
		if(spaces == 12)
		{
			stat.utime = atoi(&buffer[i]);
		}
		if(spaces == 13)
		{
			stat.stime = atoi(&buffer[i]);
		}
		if(spaces == 14)
		{
			stat.cutime = atoi(&buffer[i]);
		}
		if (spaces == 16)
		{
			stat.cstime = atoi(&buffer[i]);
			break;
		}
		if(isspace(buffer[i]))
		{
			spaces++;
		}	
	}
	//printf("Utime: %d, Stime: %d, Cutime: %d, Cstime: %d, startTime: %d, Hertz: %ld\n", utime, stime, cutime, cstime, startTime, hertz);
	return stat;
}

float calculation(struct stats stat){
	float totalTime;

	totalTime = stat.utime + stat.stime;
	totalTime += stat.cutime + stat.cstime;
	
	return totalTime;
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
