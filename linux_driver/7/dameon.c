#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<fcntl.h>
#include<sys/types.h>
#include<unistd.h>
#include<sys/wait.h>
         
#define MAXFILE 65535
int main()
{
	pid_t pc;
	int i,fd,len;
	char *buf="This is a Dameon\n";
	len =strlen(buf);
	pc=fork(); 
	if(pc<0){
		printf("error fork\n");
		exit(1);
	}else if(pc>0)
	exit(0);
	setsid();
	chdir("/");
	umask(0);
	for(i=0;i<MAXFILE;i++)
		close(i);
	while(1){
		if((fd=open("/tmp/dameon.log",O_CREAT|O_WRONLY|O_APPEND,0600))<0){
			perror("open");
			exit(1);
			}
		write(fd, buf, len+1);
		close(fd);
		sleep(10);
	}
}

