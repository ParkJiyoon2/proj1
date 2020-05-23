#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/msg.h>
#include <stdlib.h>
#include <string.h>

#include "info.h"

void printMsgInfo(int msqid){

        struct msqid_ds m_stat;
        printf("========== messege queue info =============\n");
        if(msgctl(msqid,IPC_STAT,&m_stat)==-1){
                printf("msgctl failed");
                exit(0);
        }
        printf(" message queue info \n");
        printf(" msg_lspid : %d\n",m_stat.msg_lspid);
        printf(" msg_qnum : %ld\n",m_stat.msg_qnum);
        printf(" msg_stime : %ld\n",m_stat.msg_stime);

        printf("========== messege queue info end =============\n");
}
int main(){
        key_t key=12345;
        int msqid;

        struct infomation info;
        info.msg_type=1;
        info.data.data1=80;
        strcpy(info.data.data2,"REAKWON");

        
        if((msqid=msgget(key,IPC_CREAT|0666))==-1){
                printf("msgget failed\n");
                exit(0);
        }

        
        printMsgInfo(msqid);

        
        if(msgsnd(msqid,&info,sizeof(struct real_data),0)==-1){
                printf("msgsnd failed\n");
                exit(0);
        }

        printf("message sent\n");
        
        printMsgInfo(msqid);
}
