#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>
#include <sys/ipc.h>
#include <semaphore.h>
#include <fcntl.h>
#define FIFO "myfifo"
#define N 5
int lock_var;
time_t end_time;
char buf_r[100];
sem_t mutex,full,avail;
int fd;
void pthread1(void *arg);
void pthread2(void *arg);
void consumer(void *arg);
void productor(void *arg);

int main(int argc, char *argv[])
{
	pthread_t id1,id2;
	pthread_t mon_th_id;
	int ret;
	end_time = time(NULL)+30;
	if((mkfifo(FIFO,O_CREAT|O_EXCL)<0)&&(errno!=EEXIST))
		printf("cannot create fifoserver\n");
	printf("Preparing for reading bytes...\n");
	memset(buf_r,0,sizeof(buf_r));
	fd=open(FIFO,O_RDWR|O_NONBLOCK,0);
	if(fd==-1)
	{
		perror("open");
		exit(1);
	}
	ret=sem_init(&mutex,0,1);
	ret=sem_init(&avail,0,N);
	ret=sem_init(&full,0,0);
	if(ret!=0)
	{
		perror("sem_init");
	}
	ret=pthread_create(&id1,NULL,(void *)productor, NULL);
	if(ret!=0)
		perror("pthread cread1");
	ret=pthread_create(&id2,NULL,(void *)consumer, NULL);
	if(ret!=0)
		perror("pthread cread2");
	pthread_join(id1,NULL);
	pthread_join(id2,NULL);

	exit(0);
}

void productor(void *arg)
{
	int i,nwrite;
	while(time(NULL) < end_time){
		sem_wait(&avail);
		sem_wait(&mutex);
		if((nwrite=write(fd,"hello",5))==-1)
		{
			if(errno==EAGAIN)
				printf("The FIFO has not been read yet.Please try later\n");
		}
		else
			printf("write hello to the FIFO\n");
		sem_post(&full);
		sem_post(&mutex);
		sleep(1);
	}
}

void consumer(void *arg)
{
	int nolock=0;
	int ret,nread;
	while(time(NULL) < end_time){
		sem_wait(&full);
		sem_wait(&mutex);
		memset(buf_r,0,sizeof(buf_r));
		if((nread=read(fd,buf_r,100))==-1){
			if(errno==EAGAIN)
				printf("no data yet\n");
		}
		printf("read %s from FIFO\n",buf_r);
		sem_post(&avail);
		sem_post(&mutex);
		sleep(1);
	}
}

