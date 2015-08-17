/*Dining philosophers problem*/

#include<stdio.h>
#include<pthread.h>
#include"defs.h"

#define NUM_TIME 100

int main()
{
	int i;
	pthread_t pid;

	if(init() == FAIL)
		return -1;

	for(i=0;i<NUM_PHILOSOPHERS;i++)
	{
		int * param=malloc(sizeof(*param));
		*param=i;
		pthread_create(&pid, NULL, philosopher, param);
	}

	sleep(NUM_TIME);

	return 0;
}
