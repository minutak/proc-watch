#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
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

int getPID(char *app)
{
	char command[100];
	sprintf(command, "pidof %s", app);
	FILE *f = popen(command, "r");
	if(f == NULL) return 1;
	
	char buffer[256];

	fgets(buffer, 256, f);

	pid_t PID = strtoul(buffer,NULL,10);
	
	
	pclose(f);
	return PID;
}
