

#include <stdio.h>
#include <stdlib.h>

int running_time = 0;
#define time_quantum 20					//time_quantum은 20으로 고정한다.

typedef struct priority_queue *queue_pointer;
struct priority_queue{ 
	int process_id;				//프로세스 고유id를 표현
	int priority;				//프로세스의 우선순위를 표현
	int computing_time;			//프로세스의 computing_time을 표현(유동적이다.)

	int before_time;			//본래 가지고 있던 computing_time을 구하기위함.(요구사항으로 나와있어서)
	int start_time;				//프로세스가 진입할때 가지고있던 시간.
	int turnarround_time;		//turnarround_time = 전체수행시간 - 프로세스진입시간
								//					running_time - start_time
	queue_pointer left_link, right_link;
};

typedef struct queue_head *head_pointer;
struct queue_head
{							 // 포인터구조체
	queue_pointer link;
};

head_pointer queue[4],RR;


void initialize_queue(void)
{
	queue[0] = (head_pointer)malloc(sizeof(struct queue_head));
	queue[0]->link = NULL;
	queue[1] = (head_pointer)malloc(sizeof(struct queue_head));
	queue[1]->link = NULL;
	queue[2] = (head_pointer)malloc(sizeof(struct queue_head));
	queue[2]->link = NULL;
	queue[3] = (head_pointer)malloc(sizeof(struct queue_head));
	queue[3]->link = NULL;
	//RR = (head_pointer)malloc(sizeof(struct queue_head));
	//RR->link = NULL;
	//queue[0]~[2]까지의 큐와 구분하기위해 RR이라는 head큐를 생성.
}

int insert_queue(int process_id,int priority, int computing_time,int before_time,int start_time,int mode)
{
	int que_id;
	queue_pointer pointer,temp;													//		temp=new_node

	if(priority<=10)		//~10 priority
		que_id=0;
	else if(priority>=11 && priority<=20)	//11~20 priority
		que_id=1;
	else if(priority>=21 && priority<=30)	//21~30 priority
		que_id=2;
	else 
		que_id=3;

	
	temp = (queue_pointer)malloc(sizeof(struct priority_queue));
	if(mode==0)												//파일상에서 insert_node를 할때 수행
	{
		temp->process_id = process_id;
		temp->priority = priority;
		temp->computing_time = computing_time;

		temp->before_time = computing_time;
		temp->start_time = running_time;
	}
	else if(mode==1)										//priority가 20을 초과해서 재진입할때 수행
	{
		temp->process_id = process_id;
		temp->priority = priority;
		temp->computing_time = computing_time;

		temp->before_time = before_time;
		temp->start_time = start_time;
	}

	temp->left_link = NULL;
	temp->right_link = NULL;

	//head와 첫번째노드를 연결해줄때(첫노드삽입)
	pointer=queue[que_id]->link;
	if(pointer==NULL)
	{
		queue[que_id]->link=temp;
		return 0;
	}
	//새priority가 첫노드보다 작을때 
	if(pointer->priority > priority)
	{
		temp->right_link=queue[que_id]->link;
		queue[que_id]->link->left_link=temp;
		queue[que_id]->link=temp;
		return 0;
	}
	//새priority가 중간에 들어갈때
	while((pointer->right_link != NULL) && (pointer->right_link->priority <= priority)) 

		{												
			pointer = pointer->right_link; //다음노드로이동
		}
	
		temp->right_link = pointer->right_link;
		temp->left_link = pointer;
		pointer->right_link = temp;

		if(pointer->right_link == NULL)	//다음큐가NULL이아닐때큐연결
		{
			pointer->right_link->left_link = temp; 
		}
	return 0;
}

int delete_queue(int que_id)
{
	queue_pointer pointer;
	pointer=queue[que_id]->link;

	//빈큐일 인셉션이라고할까 
	if(queue[que_id]->link==NULL)
	{
			printf("\n[queue[que_id]->link==NULL]\n");
			return 0;
	}

	if(pointer->right_link==NULL)	//NULL일때까지 찾기.
	{
			queue[que_id]->link = NULL;
				return 0;
	}
	else
{
		queue[que_id]->link = pointer->right_link;
		pointer->right_link->left_link=NULL;
		pointer->left_link=NULL;
		return 0;
	}
}

int schedule()
{
	int que_id=0;
	int turnarround_time = 0;
	queue_pointer temp;
	

	for(que_id=0;que_id<4;que_id++)
	{
		if(queue[que_id]->link!=NULL)
		{
			temp = queue[que_id]->link;
			break;
		}
		else if(que_id==3)
		{
			printf("---");
			exit(1);
		}
	}
	if(temp->priority < 0)//real time일때
	{
		running_time = running_time + temp->computing_time;
		//전체수행시간 = 여태까지 수행시간+computing_time시간만큼 증가.

		printf("%d \t%15s %9d %15d %15d \n",
			temp->process_id, "read_time", temp->priority, temp->computing_time, temp->computing_time);
		//turnarround_time은 computing_time과 같다.실시간프로세스이므로.
		delete_queue(que_id);
		return 0;	
	}

	if(temp->computing_time < 21) //1~21까지일때
	{
		running_time = running_time + temp->computing_time;
		temp->turnarround_time = running_time - temp->start_time;

		if(que_id+1==4)										//RR문자을 표현하기 위해 만들었음.
		{
			printf("%d %19s %11d %15d %15d \n",
				temp->process_id, "R-R(Q4)", temp->priority, temp->before_time, temp->turnarround_time);
		}
		else												//que_id가0~2일때
		{
			printf("%d %15d %15d %15d %15d \n",
				temp->process_id, que_id+1, temp->priority, temp->before_time, temp->turnarround_time);
		}
		delete_queue(que_id);
		return 0;
	}
	else if(temp->computing_time > time_quantum)						//computing_time이 time_quantum보다 클때.
	{
		running_time = running_time + time_quantum;						//time_quantum은 20이다. 전체수행시간 = 전체수행시간+20
		temp->before_time = temp->before_time;							//고유computing_time시간은 그대로 지님.
		temp->computing_time = temp->computing_time-time_quantum;		//computing_time은 -20만큼 소모했으므로 20만큼 차감한다
		temp->priority = temp->priority + 10;							//priority는 +10만큼 증가한다.
		
		if(temp->priority > 31)											//priority가 31보다 클때는 31로 고정한다.
			temp->priority = 31;

			//int process_id,int priority, int computing_time
		
		insert_queue(temp->process_id,temp->priority,temp->computing_time		//computing_time >time_quantum 상황에선
			,temp->before_time,temp->start_time,1);								//재진입을 의미하므로 맨뒤에 mode값을 1로 준다.
		delete_queue(que_id);													//cf)main상에선 type==0일땐, mode는 0.
		return 0;
	}
	return 0;
}

int main()
{
	int type,intput_p,intput_c = 0;
	int mode=0;
	int p_id=0;
	//메모 : intial이 왜 앞에오명 안되냐 			
	FILE* input=fopen("input.txt","r");	
	if (input==NULL) { 
		printf("input.txt 파일이없습니다.\n");
		return 0;
	}

	initialize_queue();
	printf("\n");
	printf("Process_id    Queue_id    priority    computing_time    turnaround_time\n");
	printf("-----------------------------------------------------------------------\n");
	while (!feof(input))	
	{
		fscanf(input, "%d %d %d %d", &type, &p_id, &intput_p, &intput_c);
		switch(type)
		{
			case 0:
				insert_queue(p_id,intput_p, intput_c,intput_c,running_time,0);//맨마지막매개는 mode;
		//int insert_queue(int process_id,int priority, int computing_time,int before_time,int start_time,int mode)
		//위의 주석은 insert_queue을 나타낸것이고, type이0일때 일반적인 적재를 하므로 mode값은 0이다.
				if(intput_p<0)
					schedule();
				break;
			case 1:
				schedule();
				break;
			case -1:
				while(1)
				{
					schedule();			//type -1을만나면 더이상의 적재가 없으므로 큐에 남아있는 프로세스를 수행한다.(소모한다)
				}
		}
	}
	fclose(input);
	return 0;
		
}

