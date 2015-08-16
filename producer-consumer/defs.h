#ifndef __PRODUCER_CONSUMER_DEFS
#define __PRODUCER_CONSUMER_DEFS

#include<string.h>
#include<errno.h>
#include<pthread.h>
#include<semaphore.h>

#define FAIL    0
#define SUCCESS 1

/*number of producer and consumer threads*/
#define NUM_PRODUCERS 3
#define NUM_CONSUMERS 2

#define BUFFER_SIZE 5
typedef int buf_item;

typedef struct buffer
{
	buf_item data[BUFFER_SIZE];
	buf_item in;				/*[in%BUFFER_SIZE] index to first empty slot*/
	buf_item out;				/*[out%BUFFER_SIZE] index to first full slot*/
	sem_t empty;				/*semaphore to track empty slots*/
	sem_t full;					/*semaphore to track full slots*/
	pthread_mutex_t mutex;		/*mutex for synchronization*/
}buffer;

buffer BUFFER;

int buf_init()
{
	if(sem_init(&(BUFFER.empty), 0, BUFFER_SIZE) != 0)
	{
		fprintf(stderr, "semaphore initialization for empty failed: %s\n", strerror(errno));
		return FAIL;
	}
	if(sem_init(&(BUFFER.full), 0, 0) != 0)
	{
		fprintf(stderr, "semaphore initialization for full failed: %s\n", strerror(errno));
		return FAIL;
	}
	if(pthread_mutex_init(&(BUFFER.mutex), NULL) != 0)
	{
		fprintf(stderr, "mutex initialization failed: %s\n", strerror(errno));
		return FAIL;
	}
	BUFFER.in=0;
	BUFFER.out=0;

	return SUCCESS;
}

int insert_item(buf_item item)
{
	/*wait if there are no empty slots*/
	if(sem_wait(&(BUFFER.empty)) != 0)
	{
		fprintf(stderr, "%s. In %s, line %d\n",strerror(errno), __func__, __LINE__);
		return FAIL;
	}

	/*acquire the lock*/
	if(pthread_mutex_lock(&(BUFFER.mutex)) != 0)
	{
		fprintf(stderr, "%s. In %s, line %d\n",strerror(errno), __func__, __LINE__);
		return FAIL;
	}

	BUFFER.data[BUFFER.in]=item;
	BUFFER.in=(BUFFER.in+1)%BUFFER_SIZE;

	/*release the lock*/
	pthread_mutex_unlock(&(BUFFER.mutex));

	/*update the semaphore*/
	if(sem_post(&(BUFFER.full)) != 0)
	{
		fprintf(stderr, "%s. In %s, line %d\n",strerror(errno), __func__, __LINE__);
		return FAIL;
	}

	return SUCCESS;
}

int fetch_item(buf_item * item)
{
	/*wait if there are no full slots*/
	if(sem_wait(&(BUFFER.full)) != 0)
	{
		fprintf(stderr, "%s. In %s, line %d\n",strerror(errno), __func__, __LINE__);
		return FAIL;
	}

	/*acquire lock*/
	if(pthread_mutex_lock(&(BUFFER.mutex)) != 0)
	{
		fprintf(stderr, "%s. In %s, line %d\n",strerror(errno), __func__, __LINE__);
		return FAIL;
	}

	*item=BUFFER.data[BUFFER.out];
	BUFFER.out=(BUFFER.out+1)%BUFFER_SIZE;

	/*release the lock*/
	pthread_mutex_unlock(&(BUFFER.mutex));

	/*update the semaphore*/
	if(sem_post(&(BUFFER.empty)) != 0)
	{
		fprintf(stderr, "%s. In %s, line %d\n",strerror(errno), __func__, __LINE__);
		return FAIL;
	}

	return SUCCESS;
}

#endif	/*__PRODUCER_CONSUMER_DEFS*/
