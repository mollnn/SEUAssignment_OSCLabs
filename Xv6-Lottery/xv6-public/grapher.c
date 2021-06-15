#include "types.h"
#include "fcntl.h"
#include "user.h"
#include "pstat.h"
/*
 * This programs monitors two processes and records their numer of ticks every so and so intervals
 */

// Sampling frequency
const int SAMP_PERIOD = 75;
// Total time sampled
const int SAMP_WINDOW = 2000;

// How many tickets the grapher has
const int GRAPHER_PRIORITY = 1000;
// Where we're writing to
const char*const OUTPUT_FILE = "graph.csv";

int main(int argc, char** argv)
{
	settickets(GRAPHER_PRIORITY);

	// How many processes we're observing
	const int PROCESS_QUANTITY = argc - 1;

	// list of pids
	int processes[PROCESS_QUANTITY];
	for(int i=0;i<PROCESS_QUANTITY;i++)
		processes[i] = atoi(argv[i+1]);

	// Let's get the pstat struct
	struct pstat pinfo = {0};
	if(0>getpinfo(&pinfo))
	{
		printf(2, "getpinfo() failed\n");
		exit();
	}

	// Open file
	//unlink(OUTPUT_FILE);
	int fp = 1;//open(OUTPUT_FILE, O_WRONLY);

	// Write header
	printf(fp, "time, ");
	for(int i=0;i<PROCESS_QUANTITY;i++)
	{
		printf(fp, "%d, ", processes[i]);
	}
	write(fp, "\n", 1);

	// List of ies in the pstat struct
	// that give us the processes we want
	int pindices[PROCESS_QUANTITY];
	for(int i=0;i<NPROC;i++)
	{
		for(int i=0;i<PROCESS_QUANTITY;i++)
		{
			if(pinfo.pid[i] == processes[i])
			{
				pindices[i] = i;
			}
		}
	}




	int time_passed = 0;
	while(1)
	{
		// Update pinfo
		getpinfo(&pinfo);

		printf(fp, "%d, ", uptime());

		// Fill ticks
		for(int i=0; i<PROCESS_QUANTITY;i++)
		{

			printf(fp, "%d, ", pinfo.ticks[pindices[i]]);
		}
		write(fp, "\n", 1);

		// End if needed
		if(time_passed>=SAMP_WINDOW)
			break;
		// Sleep
		sleep(SAMP_PERIOD);
		time_passed+=SAMP_PERIOD;
	}

	close(fp);


	exit();
}