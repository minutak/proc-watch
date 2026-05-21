#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

int getPID(const char *processName);
int readStat(int pid, int upTime);
int upTimeM();

/*
int main(int argc, char *argv[])
{
	int pid = getPID(argv[1]);
	printf("PID: %d\n", pid);
	while(true)
	{
		int upTime = upTimeM();
		int cpuUsage = readStat(pid, upTime);
		printf("CPU usage: %d\n", cpuUsage);
		sleep(1);
	}
}
*/

int main(int argc, char *argv[])
{
	if (argc < 2) {
		printf("Použití: %s <název_procesu>\n", argv[0]);
		return 1;
	}

	int pid = getPID(argv[1]);
	printf("Sleduji PID: %d... (Ukonči přes Ctrl+C)\n", pid);

	// Budeme potřebovat dva snímky po sobě
	int upTime1, upTime2;
	int cpuUsage;

	while (1)
	{
		// 1. Načteme uptime na začátku sekundy
		upTime1 = upTimeM();

		// 2. Spustíme readStat, který nám interně spočítá tiky (Snímek A)
		// Abychom ale viděli změnu, musíme v readStat porovnávat rozdíly.
		// Pro jednoduché otestování teď necháme tvůj readStat běžet,
		// ale skočíme do smyčky:

		int usage1 = readStat(pid, upTime1);

		// Počkáme přesně 1 sekundu, během které můžeš pustit video
		sleep(1);

		upTime2 = upTimeM();
		int usage2 = readStat(pid, upTime2);

		// Schválně ti to vypíše rozdíl mezi dvěma měřeními
		printf("----------------------------------------\n");
		printf("Aktuální zátěž v této sekundě: %d%%\n", usage2);
		printf("----------------------------------------\n");
	}

	return 0;
}

int getPID(const char *processName)
{
	// Creating variables for command and line(string of chars that will command output)
	char line[256];
	char command[300];
	int pid = 0;
	// Puts in command "pidof %s" ...
	sprintf(command, "pidof %s | tail -c 5", processName);
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

int readStat(int pid, int upTime)
{
	char command[300];
	sprintf(command, "cat /proc/%d/stat", pid);

	FILE *status = popen(command, "r");
	char line[2560];
	int ticks = sysconf(_SC_CLK_TCK);
	int startTime;
	int uTime;
	int kTime;
	int totalTime;
	int cpuUsage;
	if(status == NULL) return 1;

	if (fgets(line, sizeof(line), status) != NULL)
    	{
        	int spaceCount = 0;
        	char *ptr = line;

        	// Pokud název procesu obsahuje závorky, bezpečně najdeme tu POSLEDNÍ uzavírací.
        	// Tím přeskočíme zmatek s PID a názvem procesu.
        	char *last_bracket = strrchr(line, ')');
        	if (last_bracket != NULL) 
		{
			// sscanf začne číst text až ZA závorkou.
			// První věc za závorkou je stav (%*c, např. 'S'), 
			// pak následuje ppid (%*d), pgrp (%*d) atd.
			// Přeskočíme přesně tolik polí, abychom se dostali na utime (14.), stime (15.) a starttime (22.)
			sscanf(last_bracket + 1, 
			       " %*c %*d %*d %*d %*d %*d %*u %*u %*u %*u %*u %ld %ld %*d %*d %*d %*d %*d %*d %ld", 
			       &uTime, &kTime, &startTime);
		}

        	// Procházíme string znak po znaku a počítáme reálné mezery
        	while (*ptr != '\0') 
		{
            		if (*ptr == ' ') 
			{
                		spaceCount++;

                		// Hledáme utime (14. políčko, takže následuje za 13. mezerou)
                		if (spaceCount == 13) 
				{
                    			uTime = atoi(ptr + 1);
                		}
                		// Hledáme stime (15. políčko, za 14. mezerou)
                		if (spaceCount == 14) 
				{
                    			kTime = atoi(ptr + 1);
                		}
                		// Hledáme starttime (22. políčko, za 21. mezerou)
                		if (spaceCount == 21) 
				{
                   			 startTime = atoi(ptr + 1);
                   			 break; // Máme všechno, můžeme cyklus ukončit
                		}
            		}
            		ptr++;
        	}
   	}
   	pclose(status);
	printf("utime: %d, Ktime: %d, Starttime: %d\n", uTime, kTime, startTime);
	printf("Ticks: %d\n", ticks);

	totalTime = uTime + kTime;

	// Spočítáme sekundy (věk procesu)
	double seconds = (double)upTime - ((double)startTime / ticks);

	// Ochrana před dělením nulou, kdyby proces vznikl před milisekundou
	if (seconds <= 0) return 0;

	// Výpočet s (double) – Cčko nejdřív spočítá přesné procento s desetinami a pak ho (int) ořízne na celé číslo
	cpuUsage = (int)(100.0 * (((double)totalTime / ticks) / seconds));

	return cpuUsage;
}

int upTimeM()
{
	char command[256];
	sprintf(command, "cat /proc/uptime");
	FILE *uptime = popen(command, "r");
	char line[256];

	while(fgets(line, sizeof(line), uptime))
	{
		return atoi(line);
	}
}
