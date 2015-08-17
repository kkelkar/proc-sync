/*Producer-Consumer problem*/

#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include"defs.h"

/*run the program for NUM_TIME seconds*/
#define NUM_TIME 100

/*generate random number between 1 and 10*/
int random_num()
{
	srand(time(NULL));
	return (rand()%10 + 1);
}

void * producer(void * param)
{
	buf_item item;
	int id=*((int *)param); 
	free(param);

	while(1)
	{
		sleep(random_num());

		item=random_num();
		if(insert_item(item)==FAIL)
		{
			fprintf(stderr, "failed to insert item. "
					"In %s, line %d\n", __func__, __LINE__);
			pthread_exit(NULL);
		}
		else
		{
			fprintf(stdout, "producer[%d] produced %d\n", id, item);
		}
	}	
}

void * consumer(void * param)
{
	buf_item item;
	int id=*((int *)param);
	free(param);

	while(1)
	{
		sleep(random_num());

		if(fetch_item(&item)==FAIL)
		{
			fprintf(stderr, "failed to fetch item. "
					"In %s, line %d\n", __func__, __LINE__);
			pthread_exit(NULL);
		}
		else
		{
			fprintf(stdout, "consumer[%d] consumed %d\n", id, item);
		}
	}	
}

int main()
{
	int i;
	pthread_t P_tid, C_tid;

	if(buf_init() == FAIL)
		return -1;

	for(i=0;i<NUM_PRODUCERS;i++)
	{
		int * param=malloc(sizeof(*param));
		*param=i;
		pthread_create(&P_tid, NULL, producer, param);
	}	

	for(i=0;i<NUM_CONSUMERS;i++)
	{
		int * param=malloc(sizeof(*param));
		*param=i;
		pthread_create(&C_tid, NULL, consumer, param);
	}

	sleep(NUM_TIME);

	return 0;	
}
