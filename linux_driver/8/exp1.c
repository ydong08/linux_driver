#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>

int main(void)
{
	int fds[2];
	char buf[7];
	int i,rc,maxfd;
	fd_set inset1,inset2;
	struct timeval tv;
	if((mkfifo("fifo1",O_CREAT|O_EXCL)<0)&&(errno!=EEXIST))
		printf("cannot create fifoserver\n");
	if((mkfifo("fifo2",O_CREAT|O_EXCL)<0)&&(errno!=EEXIST))
		printf("cannot create fifoserver\n");
	if((fds[0] = open ("fifo1", O_RDWR|O_NONBLOCK,0))<0)
		perror("open fifo1");
	if((fds[1] = open ("fifo2", O_RDWR|O_NONBLOCK,0))<0)
		perror("open fifo2");
	if((rc = write(fds[0],"Hello!\n",7)))
		printf("rc=%d\n",rc);
	lseek(fds[0],0,SEEK_SET);
	maxfd = fds[0]>fds[1] ? fds[0] : fds[1];
	FD_ZERO(&inset1);
	FD_SET(fds[0],&inset1);
	FD_ZERO(&inset2);
	FD_SET(fds[1],&inset2);
	while(FD_ISSET(fds[0],&inset1)||FD_ISSET(fds[1],&inset2)){
		if(select(maxfd+1,&inset1,&inset2,NULL,NULL)<0)
			perror("select");
		else{
			if(FD_ISSET(fds[0],&inset1)){
				rc = read(fds[0],buf,7);
				if(rc>0){
					buf[rc]='\0';
					printf("read: %s\n",buf);
			}else
				perror("read");
			}
		if(FD_ISSET(fds[1],&inset2)){
			rc = write(fds[1],buf,7);
			if(rc>0){
				buf[rc]='\0';
				printf("rc=%d,write: %s\n",rc,buf);
			}else
				perror("write");
			}
		}
	}
	exit(0);
}

