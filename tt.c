#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define TRUE 1
#define FALSE 0


typedef	struct Process {
	int a_time;		// 도착시간
	int b_time;		// 처리시간
	int rb_time;	// 현재까지 프로세스가 서비스된 시간  
	int start_time;	// 프로세스가 서비스되기 시작한 시각
	int w_time;		// waiting Time
	int r_time;		// return Time
	int q_time;		// 퀀텀타임 기록 변수
	int r_check;	// 문맥교환 변수
	int ps_End;		// 프로세스의 끝 채크 변수 
	int switching;	// 문맥교환 스위치 변수
	int ps_num;		// 프로세스 번호
	int s_check;    // 서비스 시작을 체크
	struct Process *link;
}Process;

Process *ps_l = NULL;
Process *ps_head = NULL;
Process *ps_temp = NULL;
Process *ps_temp2 = NULL;


int Qt;					// context switching time간격
int s_time;				// 시뮬레이션 시간
int numberPs;			// 프로세스 번호
int UsingPs;			// 현재 사용중인 프로세스 넘버 저장하는 변수 
int Cpu_check;			// cpu가 사용중인지 체크하는 함수
int Endtime;		    // 프로그램 종료 시각

void Init();				// 프로세스의 값들을 초기화 하는 함수 
void R_Robin();				// R_Robin 알고리즘 구현 함수 
void creat_ps(int top);		// 새로운 프로세스 생성
void display();				// 프로세스 처리 결과 출력

void main()
{
	int i;

	printf("Time Quantum, context switching time\n기본값: ENTER  값 입력: Y\n");

	//Time Quantum, context switching time과 시뮬레이션 시간입력 
	if(getchar()=='y'){
		printf("Time Quantum :");
		scanf("%d",&Qt);
		printf("Simulation time :");
		scanf("%d",&s_time);
		printf("Number of process :");
		scanf("%d",&numberPs);
		printf( "----------------------------------------------------\n");
	}
	else{ //Default값 제공
		Qt = 5;				
		s_time = 300;
		numberPs = 5;
	}
	printf( "Time Quantum : %d    \nSimulation time : %d",	Qt, s_time);


	for(i = 0; i < numberPs; i++) creat_ps(i); // 지정된 수 만큼 새 프로세스 생성

	Init();	
	printf("\n");

	//srand(time(NULL));

	//프로세스 도착시간, 처리시간 생성
	for(ps_temp = ps_head; ps_temp ; ps_temp = ps_temp->link){
		ps_temp->a_time = //rand()%25;	// 도착시간
		ps_temp->b_time = rand()%50+1;  // 서비스시간
		printf("ps[%d]arrival time : %d  처리시간: %d\n",ps_temp->ps_num+1,ps_temp->a_time,ps_temp->b_time);
		
	}

	printf("\n");


	R_Robin();// 라운드 로빈 함수 호출

	display();// 결과 출력

}
					
void R_Robin()
{
	int clock,  j=0, i=0, end_check=FALSE;
	

	printf("***********Program Start*************\n");
	
	for(clock=0 ; clock <= s_time; clock++)	// 시뮬레이션 타임 만큼의 클럭생성 
	{
	
		j=0;
		
		//프로세스가 타임 퀀텀동안 한번씩 실행됐음 swiching으로 체크 
		for(ps_temp = ps_head; ps_temp ; ps_temp = ps_temp->link)// 프로세스 전제 검색
		{
			// 스위치가 켜져 있는것의 갯수를 샘
			if(ps_temp->switching)		
			{
				j++;
			}

			if(j == numberPs)	// 모든 스위치가 켜져 있으면 (실행 안됐던 프로세스가 없으면)
			{
				for(ps_temp2 = ps_head; ps_temp2 ; ps_temp2 = ps_temp2->link)
				{
					ps_temp2->switching = FALSE; // 모두 초기화(전부 실행이 안된 상태로 전환)
				}	
			}
			


		}//end for


	

		//cpu가 사용중이 아닐때 사용될 프로세스 지정
		for(ps_temp = ps_head; ps_temp ; ps_temp = ps_temp->link)// 프로세스 전제 검색
		{	
			if(ps_temp->a_time == clock)printf("************* PS[%2d] arrive**********\n",ps_temp->ps_num+1);		
			/*프로세스가 하나 남았을때 그 프로세스가 작업을 끝내고  true가 돼도 
				끝난 프로세스는 ps_temp->switching를 ture로 바꿔주므로
				남은 하나만 계속 실행될 수 있음*/

			if(ps_temp->ps_End) ps_temp->switching = TRUE; //끝난 작업은 스위치를 내림
		
			if(clock < ps_temp->a_time) ps_temp->switching = TRUE;//아직 다른 프로세스가 안들어왔을때를 위해

			// 프로세스의 작업이 끝났는지 체크 ,그 클럭 시간에 작업이 있는지 검사
			if(!ps_temp->switching)
			{
				if(!Cpu_check)// 현재 Cpu가 사용중이 아니면 
				{
					UsingPs = ps_temp->ps_num;		// 지금 들어온 프로세스 저장
					if(ps_temp->s_check == FALSE)
					{
						ps_temp->start_time = clock;//프로세스 첫 시작시각
						ps_temp->s_check = TRUE;
						printf("**************PS[%2d] first start*****\n", UsingPs+1);
					}
					Cpu_check = TRUE;	// 현재 작업이 있음을 알려줌
					ps_temp->s_check = TRUE;   //서비스 중임을 체크
					printf("Current : %d, PS[%2d] is started\n", clock, UsingPs+1);
						
				}
			}
			//프로세스가 모두 종료되면 end_check를 증가시켜 끝났다는 것을 알림
			if(ps_temp->ps_End && clock >= ps_temp->a_time) i++;
		}//end for
	  

		if(Cpu_check) //cpu가 작업 중인지 검사 
		{
			for(ps_temp = ps_head; ps_temp ; ps_temp = ps_temp->link)	// 프로세스 전제 검색
			{
			
				if(clock >= ps_temp->a_time)		
				{    //그 클럭 시간에 작업이 있는지 검사

					if(ps_temp->ps_num == UsingPs)		
					{   // 위의 for문에서 선택된 프로세스이면

						if(ps_temp->b_time != 0)	// b_time가 0이 아닐때
						{
							ps_temp->rb_time++;	// 남은 b_time 증가
							ps_temp->q_time++;
							 
							//Qt만큼 경과하면 context switching을 하고 q_time을 초기화	
							if(ps_temp->q_time == Qt) 
							{
								ps_temp->r_check++;
								Cpu_check = FALSE; // 다음 작업으로 넘어감
								ps_temp->q_time = 0;

								ps_temp->switching = TRUE;
								printf("Current : %d, PS[%2d] is switched\n", clock, UsingPs+1);
							}
						}
					
						//프로세스 처리가 끝났으면 종료시킴
						if (ps_temp->rb_time == ps_temp->b_time)
						{
							Cpu_check = FALSE; // cpu사용권한 해제
							ps_temp->ps_End = TRUE; // 그 프로세스의 작업이 끝남 
							ps_temp->switching = TRUE;
							ps_temp->r_time = clock+1;//ps_temp->w_time + ps_temp->b_time;
							// ReturnTiem 구함
							printf("********: %d, PS[%2d] is ended********\n", clock+1, UsingPs+1);
						}
					}
					//작업이 끝나지 않았고 해당 작업이 아니면 wait time 증가
					if(ps_temp->rb_time==0) ps_temp->w_time++;
				}
			}//end for
		}//end if
			
		if(i == numberPs && end_check==FALSE ){// 프로그램이 끝났음을 알림
			Endtime = clock;
			printf("Endtime : %d\n*************Program End************\n",Endtime);
			end_check=TRUE;
		}		
	i=0;
	}//end for

	if(end_check==FALSE )printf("**************NOT END*************\n");//프로세스들이 다 처리되지 못하고 끝남
}//end R_Robin

void Init()
{
	for(ps_temp = ps_head; ps_temp ; ps_temp = ps_temp->link)
	{ 
		// 모든 프로세스의 필드들을 초기화
		ps_temp->a_time = 0;
		ps_temp->b_time = 0;	
		ps_temp->w_time = 0;	
		ps_temp->r_time = 0;	
		ps_temp->rb_time = 0;	
		ps_temp->q_time = 0;
		ps_temp->r_check= 0;
		ps_temp->ps_End = FALSE;	
		ps_temp->switching = FALSE;	
		ps_temp->s_check = FALSE;
	}
	Endtime = 0;

}

void display()
{
	int w_sum, r_sum, num, rc_sum,  p_sum/*처리시간의 합*/;
	float w_ave, r_ave, rc_ave;		
	// 평균을 구하기 위한 변수 초기화
	rc_ave=0.0;	w_ave=0.0;	r_ave=0.0;	w_sum=0;	
	r_sum=0;	num=0;	rc_sum=0; p_sum = 0;
	
	printf( "\n\n************************************************************\n");
	printf( "                  Round Robin Scheduling \n");
	printf( "************************************************************\n");
	

	printf("Process       Wait time       Turnaround time     Context swiching\n");

	
	

	for(ps_temp = ps_head; ps_temp ; ps_temp = ps_temp->link)	// 프로세스 전제 검색
	{
		printf("P%2d", ps_temp->ps_num+1); 
		printf("%18d", ps_temp->w_time);
		printf("%18d", ps_temp->r_time - ps_temp->start_time + ps_temp->w_time);
		printf("%18d", ps_temp->r_check); 
		w_sum += ps_temp->w_time;
		r_sum += (ps_temp->r_time- ps_temp->start_time);
		rc_sum += ps_temp->r_check;
		p_sum += ps_temp->b_time; //전체 서비스 시간의 합
		num++;
		printf("\n");
		
	}
	
			
	w_ave = (float)w_sum/num;
	r_ave = (float)r_sum/num;
	rc_ave = (float)rc_sum/num;
	printf("평균");
	printf("%17.2f", w_ave);	
	printf("%18.2f", r_ave);	
	printf("%18.2f", rc_ave);


	printf("\n\n\n************************************************************\n");
	printf("Number of arrival processes per unit time\n:  %3.3f\n",(float)numberPs/s_time);
	printf("\nNumber of completed processes per unit time\n:  %3.3f\n",(float)numberPs/p_sum);
	printf("\nthe fraction of time the processor is busy\n:  %3.2f%%\n",(((float)numberPs/s_time)/((float)numberPs/p_sum))*100);
	printf("************************************************************\n");


	
}

//프로세스를 동적으로 생성
void creat_ps(int top)
{
	Process *new_node;
	new_node = (Process *)malloc(sizeof(Process));
	
	new_node->ps_num = top;
	new_node->link = NULL;

	if(ps_l == NULL){
		ps_l = new_node;
		ps_head = ps_l;
	
	}
	else {
		ps_l->link = new_node;
		ps_l = ps_l->link;
	}

} 


