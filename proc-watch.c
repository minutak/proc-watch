#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int getPID(const char *processName);

int main(int argc, char *argv[])
{
	int pid = getPID(argv[1]);
	printf("PID: %d\n", pid);
}
int getPID(const char *processName)
{
	// Creating variables for command and line(string of chars that will command output)
	char line[256];
	char command[300];
	int pid = 0;
	// Puts in command "pidof %s" ...
	sprintf(command, "pidof %s", processName);
	FILE *pipe = popen(command, "r");
	if(pipe == NULL) return 1;
	
	// Reads the whole file and outputs it into line
	while(fgets(line,sizeof(line), pipe) != NULL)
	{
		// Magic happens here and atoi reads until it sees space, in this case first 4 numbers
		pid = atoi(line);
	}	
	pclose(pipe);
	return pid;	

}
