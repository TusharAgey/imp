//Arrival time is random
//Processes are created by a thread randomly
//Preemption is added.
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <graphics.h>
#define TIMEQUANTUM 4
int when = 1;
typedef struct process{
	int pid;
	int pburst;
	int status; //0 = not considered, 1 = considered
	int wait;
	int arrive;	//process arrival time, should be given in ascending order only
	int prio;
	int when;  //when it is assigned to processor
	struct process *next;
}proc;
typedef struct proclist{
	proc *head, *tail;
	int len;
}procll;
void (*fun_ptr)(procll *);
procll pro;
int TIME = 0, ch = 0;
void initp(procll *l){
	l->head = l->tail = NULL;
	l->len = 0;
}

void insertp(procll *l, int pid, int pburst, int arrive, int prio){
	proc *tmp = (proc *)malloc(sizeof(proc));
	tmp->next = NULL;
	tmp->pid = pid;
	tmp->arrive = arrive;
	tmp->pburst = pburst;
	tmp->prio = prio;
	tmp->status = 0;
	tmp->when = when + 2000;
	if(l->head == NULL){
		l->head = l->tail = tmp;
		l->len++;
		return;
	}
	l->tail->next = tmp;
	l->tail = tmp;
	l->len++;
}

void printProcesses(){
	proc *pp = pro.head;
	printf("PID\tBT\tPrio\n");
	while(pp){
		printf("%d\t%d\t%d\n", pp->pid, pp->pburst, pp->prio);
		pp = pp->next;
	}
}
void *processCreator(){//will create processes
	int id = 1, totburst = 0, si, ar, pr;
	srand(time(NULL));
	while(totburst < 126){
		if (totburst == 0){
			si = rand() % 50 + 2;//burst time(min 2 burst time)
		}
		else{
			si = rand() % (126 - totburst) + 2;//burst time(min 2 burst time) & less than 126
		}
		ar = TIME;//Arrival Time of the process
		pr = rand() % 6;//priority
		totburst += si;
		insertp(&pro, id, si, ar, pr);
		printProcesses();
		id++;
		TIME++;//counts every second!
		if(ch != 7 && ch != 5)
			(*fun_ptr)(&pro);
		delay(3000);
	}
	if(ch == 7 || ch == 5){
		(*fun_ptr)(&pro);//its round robin or prio non - preemptive
	} 
}
void printEm(procll *p){//works best
	int i = 0, wh = 1, x = 0;
	proc *pp = p->head;
	printf("+");
	while(i < 128){
		printf("-");
		i++;
	}
	printf("+\n|");
	while(wh != when){
		while(pp){
			if((pp->when == wh) && pp->status == 1){
				//printing gantt chart
				printf("p%d", pp->pid);
				i = 0;
				while(i < (pp->pburst - 3)){
					x++;
					printf(" ");
					i++;
				}
				x += 3;
				printf("|");
			}
			pp = pp->next;
		}
		pp = p->head;
		wh++;
	}
	while(x < 128){
		printf(" ");
		x++;
	}
	printf("|\n");
	printf("+");
	i = 0;
	while(i < 128){
		printf("-");
		i++;
	}
	printf("+\n");
}
void FCFS(procll *p){//first come first served(working fine!)
	int i = -1, sum = 0, x = 7;
	proc *pp = p->head;
	printf("+");
	while(i++ < 126){
		printf("-");
	}
	printf("+\n");
	printf("|");
	while(pp){
		pp->wait = sum;
		if(pp->next != NULL){
			sum += (sum + pp->pburst);
			x += 3;
		}
		printf("p%d", pp->pid);
		i = -1;
		while(i++ <= (pp->pburst - 7)){
			printf(" ");
			x++;
		}
		printf("|");
		pp = pp->next;
	}
	while(x < 131){
		printf(" ");
		x++;
	}
	printf("|\n");
	i = -1;
	printf("+");
	while(i++ < 126){
		printf("-");
	}
	printf("+\n");
	printf("\nAverage waiting time:-%d\n", (sum/p->len));
}
int allnotone(procll *p){//works fine
	proc *pp = p->head;
	while(pp){
		if(pp->status == 0)
			return 1;
		pp = pp->next;
	}
	return 0;
}
proc *shortest(procll *p){//works fine
	proc *pp, *tmp;
	pp = tmp = p->head;
	//printf("hell\n");
	int b = 1000;
	while(pp){
		if((pp->pburst < b) && (pp->status == 0)){
			tmp = pp;
			b = pp->pburst;
		}
		pp = pp->next;
	}
	tmp->status = 1;
	return tmp;
}
void SJF(procll *p){//shortest job first//works fine
	int i = -1, sum = 0, x = 7, len = 0;
	proc *pp = p->head;
	while(allnotone(p)){
		pp = shortest(p);
		pp->status = 1;
		pp->when = when;
		when++;
		pp->wait = sum;
		if(len < (p->len - 1)){
			len++;
			sum += (sum + pp->pburst);
		}
	}
	printEm(p);
	printf("\nAverage waiting time:-%d\n", (sum/p->len));
}
void RR(procll *p){//round robin//wokrs fine. works geniously
	int i = -1, sum = 0, x = 0, nop = p->tail->pid, strt, end, flag = 0, no = 0;
	proc *pp = p->head;
	printf("+");
	while(i++ < 126){
		printf("-");
	}
	printf("+\n");
	printf("|");
	while(pp){
		pp->wait = sum;
		if(pp->pburst > TIMEQUANTUM){
			insertp(p, pp->pid, pp->pburst - TIMEQUANTUM, 0, 0);//considering arrival time 0 now
			pp->pburst = TIMEQUANTUM;
		}
		if(pp->next != NULL){
			sum += (sum + pp->pburst);
			x += 3;
		}
		printf("p%d", pp->pid);
		i = -1;
		while(i++ <= (pp->pburst - 7)){
			printf(" ");
			x++;
		}
		printf("|");
		pp = pp->next;
	}
	while(x < 124){
		printf(" ");
		x++;
	}
	printf("|\n");
	i = -1;
	printf("+");
	while(i++ < 126){
		printf("-");
	}
	pp = p->head;
	sum = 0;
	printf("+\n");
	i = 1;
	while(i <= nop){
		sum = 0;
		while(pp){
			if(flag == 0 && pp->pid == i){
				strt = sum;
				flag = 1;
			}
			else{
				end = sum;
			}
			sum += pp->pburst;
			pp = pp->next;
		}
		pp = p->head;
		i++;
		no += (end - strt);
	}
	printf("\nAverage waiting time:-%d\n", (no/nop));
}
proc *checkResponseRate(procll *p){
	proc *pp, *tmp, *oh;
	pp = tmp = oh = p->head;
	int tottime = 0;
	//find out addition of bursts of all processes that have status 1
	while(oh){
		if(oh->status == 1){
			tottime += oh->pburst;
		}
		oh = oh->next;
	}
	float responseR = 0, rate;
	while(pp){
		rate = (((float)tottime - (float)pp->arrive) + (float)pp->pburst) / ((float)pp->pburst);
		if((rate > responseR) && (pp->status == 0)){
			tmp = pp;
			responseR = rate;
		}
		pp = pp->next;
	}
	tmp->status = 1;
	return tmp;
}
void HRRN(procll *p){//highest response ratio next//works best
	int i = -1, sum = 0, x = 0, len = 0;
	proc *pp = p->head;
	while(pp){
		pp->status = 0;
		pp = pp->next;
	}
	printf("+");
	while(i++ < 126){
		printf("-");
	}
	printf("+\n");
	printf("|");
	while(allnotone(p)){
		pp = checkResponseRate(p);
		pp->wait = sum;
		if(len < (p->len - 1)){
			len++;
			sum += (sum + pp->pburst);
		}
		x += 3;
		printf("p%d", pp->pid);
		i = -1;
		while(i++ <= (pp->pburst - 7)){
			printf(" ");
			x++;
		}
		printf("|");
	}
	while(x < 127){
		printf(" ");
		x++;
	}
	printf("|\n");
	i = -1;
	printf("+");
	while(i++ < 126){
		printf("-");
	}
	printf("+\n");
	printf("\nAverage waiting time:-%d\n", (sum/p->len));
}
proc *checkPriority(procll *p){//works fine!
	proc *pp, *tmp;
	pp = tmp = p->head;
	//printf("hell\n");
	int b = 1000;
	while(pp){
		if((pp->prio < b) && (pp->status == 0)){
			tmp = pp;
			b = pp->prio;
		}
		pp = pp->next;
	}
	tmp->status = 1;
	return tmp;
}
void PrioNonPre(procll *p){//priority based, non-preemptive//works fine
	int i = -1, sum = 0, x = 7, len = 0;
	proc *pp = p->head;
	while(allnotone(p)){
		pp = checkPriority(p);
		pp->status = 1;
		pp->when = when;
		when++;
		if(len < (p->len - 1)){
			len++;
			sum += (sum + pp->pburst);
		}
	}
	printEm(p);
	printf("\nAverage waiting time:-%d\n", (sum/p->len));
}
void forkIt(proc *p, int burst, int prio){
	proc *tmp = (proc *)malloc(sizeof(proc));
	tmp->pid = p->pid + 10;//the forked process will have 1 as prefix to original pid
	tmp->pburst = burst;
	tmp->prio = prio;
	tmp->status = 0;
	tmp->next = p->next;
	p->next = tmp;
	tmp->when = when + 2000;
}
void PrioPre(procll *p){//priority based, preemitive//fine
	int i = -1, sum = 0, x = 7, len = 0, burstInSec;
	proc *pp = p->head;
	while(allnotone(p)){
		pp = checkPriority(p);
		if(pp->pburst > 30){
			forkIt(pp, (pp->pburst - 30), pp->prio);
			pp->pburst = 30;
			x = 1;
		}
		pp->status = 1;
		pp->when = when;
		when++;
		if(len < (p->len - 1)){
			len++;
			sum += (sum + pp->pburst);
		}
		if(x == 1){
			x = 0;
			break;
		}
	}
	printEm(p);
	printf("\nAverage waiting time:-%d\n", (sum/p->len));
}
void SRTN(procll *p){//shortest remaining time next//fine
	int i = -1, sum = 0, x = 7, len = 0, burstInSec;
	proc *pp = p->head;
	while(allnotone(p)){
		pp = shortest(p);
		if(pp->pburst > 30){
			forkIt(pp, (pp->pburst - 30), pp->prio);
			pp->pburst = 30;
		}
		pp->status = 1;
		pp->when = when;
		when++;
		if(len < (p->len - 1)){
			len++;
			sum += (sum + pp->pburst);
		}
	}
	printEm(p);
	printf("\nAverage waiting time:-%d\n", (sum/p->len));
}
int main(){
	pthread_t t;
	int ans = 0, si, id = 1, hm = 1, ar, pr;
	while(ch != 8){
		printf("MENU:-\n1)FCFS\n2)SJF\n3)SRTF\n4)priority based preemptive\n5)priority based non-preemptive\n6)HRRN\n7)Round Robin\n8)exit\n");
		scanf("%d", &ch);
		when = 1;
		initp(&pro);
		switch(ch){
			case 1:
				fun_ptr = &FCFS; 
				pthread_create(&t, NULL, &processCreator, NULL);
				break;
			case 2:
				fun_ptr = &SJF; 
				pthread_create(&t, NULL, &processCreator, NULL);
				break;
			case 3:
				fun_ptr = &SRTN; 
				pthread_create(&t, NULL, &processCreator, NULL);
				break;
			case 4:
				fun_ptr = &PrioPre; 
				pthread_create(&t, NULL, &processCreator, NULL);
				break;
			case 5:
				fun_ptr = &PrioNonPre; 
				pthread_create(&t, NULL, &processCreator, NULL);
				break;
			case 6:
				fun_ptr = &HRRN; 
				pthread_create(&t, NULL, &processCreator, NULL);
				break;
			case 7:
				fun_ptr = &RR; 
				pthread_create(&t, NULL, &processCreator, NULL);
				break;
			default:
				break;
		}
	}
	return 0;
}