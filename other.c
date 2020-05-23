#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/stat.h>

struct msgbug{
	long mstype;
	char mtext[256];
	char myname[16];
	int seq;
};

int main(){
	key_t key_id;
	int i;
	struct msgbuf mybuf, rcvbuf;

	Key_id msgget((key_t)1234,IPC_CREAT|0666);
	if(Key_id==-1){perror("msgget error:");exit(0);}

	printf("Key is %d\n", Key_id);

	memset(mybuf.mtext,0x00,256);
	memset(mybuf.myname,0x00,16);
	memcpy(mybuf.myname,"yundream",8);
	mybuf.seq=0;
	i=0;

	while(1){
		if(i%2==0) mybuf.msgtype=4;
		else mybuf.msgtype=3;
		mybuf.seq=i;
	}
	printf("%d\n",rcvbuf.msgtype);
	printf("%s\n",rcvbuf.mtext);
	printf("%s\n",rcvbuf.myname);
	exit(0);
}

