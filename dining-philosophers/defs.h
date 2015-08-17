/*
 *			p0
 *
 *		c3		c0
 *
 *	p3				p1
 *
 *		c2		c1
 *
 *			p2
 */

#ifndef __DINING_PHILOSOPHER_DEFS_
#define __DINING_PHILOSOPHER_DEFS_

#include<semaphore.h>
#include<stdlib.h>
#include<unistd.h>
#include<errno.h>
#include<string.h>

#define FAIL	0
#define SUCCESS	1

#define NUM_PHILOSOPHERS	4
#define NUM_CHOPSTICKS		NUM_PHILOSOPHERS

typedef enum States
{
	THINKING,
	HUNGRY,
	EATING
}States;

States states[NUM_PHILOSOPHERS];

sem_t chopsticks[NUM_CHOPSTICKS];

/*generate random number between 1 and 10*/
int random_num()
{
	srand(time(NULL));
	return (rand()%10 + 1);
}

int init()
{
	int i;
	for(i=0;i<NUM_PHILOSOPHERS;i++)
	{
		states[i]=THINKING;
	}

	for(i=0;i<NUM_CHOPSTICKS;i++)
	{
		if(sem_init(&chopsticks[i], 0, 1) != 0)
		{
			fprintf(stderr, "semaphore initialization failed: %s\n",
					strerror(errno));
			return FAIL;
		}
	}

	return SUCCESS;
}

/*
 * A philosopher will first pick a chopstick on his/her left side, and then
 * attempt to pick the one on the right side. If chopstick on the right is 
 * not available, the philosopher will keep down the left chopstick and try
 * to acquire both the chopsticks at a later time.
 */

void * philosopher(void * param)
{
	int id=*((int *)param);
	free(param);

	while(1)
	{
		up:
		sleep(random_num());

		states[id]=HUNGRY;

		/*left chopstick*/
		if(sem_wait(&chopsticks[id%NUM_CHOPSTICKS]) != 0)
		{
			//if(errno == EAGAIN)
			//	continue;
			//else
			{
				fprintf(stderr, "%s. In %s, line %d\n", 
						strerror(errno), __func__, __LINE__);
				pthread_exit(NULL);
			}
		}
		/*right chopstick*/
		if(sem_trywait(&chopsticks[(NUM_CHOPSTICKS-1+id)%NUM_CHOPSTICKS]) != 0)
		{
			if(errno == EAGAIN)
			{
				/*relinquish the left one if we can't get right*/
				if(sem_post(&chopsticks[id%NUM_CHOPSTICKS]) != 0)
				{
					fprintf(stderr, "%s. In %s, line %d\n",
							strerror(errno), __func__, __LINE__);
					pthread_exit(NULL);
				}
				goto up;
			}
			else
			{
				fprintf(stderr, "%s. In %s, line %d\n",
						strerror(errno), __func__, __LINE__);
				pthread_exit(NULL);
			}
		}
		/*eat*/
		states[id]=EATING;
		fprintf(stdout, "philosopher %d eating\n", id);
		sleep(random_num());
		fprintf(stdout, "philosopher %d finished eating\n", id);
		states[id]=THINKING;

		if(sem_post(&chopsticks[id%NUM_CHOPSTICKS]) != 0)
		{
			fprintf(stderr, "%s. In %s, line %d\n",
					strerror(errno), __func__, __LINE__);
			pthread_exit(NULL);
		}
		if(sem_post(&chopsticks[(NUM_CHOPSTICKS-1+id)%NUM_CHOPSTICKS]) != 0)
		{
			fprintf(stderr, "%s. In %s, line %d\n",
					strerror(errno), __func__, __LINE__);
			pthread_exit(NULL);
		}
	}
}

#endif	/*__DINING_PHILOSOPHER_DEFS_*/
