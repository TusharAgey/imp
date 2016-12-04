#include<stdio.h>
#include<string.h>
#include<stdlib.h>

typedef struct memory{
	int msize;
	int pid;// -1 shows that memory block is free
	struct memory *next;
}mem;

typedef struct memlist{
	mem *head, *tail;
	int len;
}memll;

typedef struct process{
	int pid;
	int psize;
	int status; //0 = not in memory, 1 = in memory
	struct process *next;
}proc;
typedef struct proclist{
	proc *head, *tail;
	int len;
}procll;
mem *currmem;

void initp(procll *l){
	l->head = l->tail = NULL;
	l->len = 0;
}
void initm(memll *l){
	mem *tmp = (mem *)malloc(sizeof(mem));
	tmp->next = NULL;
	tmp->msize = 1024;
	tmp->pid = -1;

	l->head = l->tail = tmp;
	l->len = 0;
}

void insertm(memll *l, mem *mm, int size){
	mem *tmp = (mem *)malloc(sizeof(mem));

	tmp->next = NULL;
	tmp->msize = size;
	tmp->pid = -1;

	if(mm->next == NULL){
		mm->next = tmp;
		l->tail = tmp;
		l->len++;
	}
	else{
		tmp->next = mm->next;
		mm->next = tmp;
		l->len++;
	}
}


void insertp(procll *l, int pid, int psize){
	proc *tmp = (proc *)malloc(sizeof(proc));
	tmp->next = NULL;
	tmp->pid = pid;
	tmp->psize = psize;
	tmp->status = 0;
	if(l->head == NULL){
		l->head = l->tail = tmp;
		l->len++;
		return;
	}
	l->tail->next = tmp;
	l->tail = tmp;
	l->len++;
}

int releasep(procll *l, memll *m, int pid){
	proc *tmp = l->head, *prev = l->head;
	mem *x = m->head;
	while(x != NULL){
		if(x->pid == pid){
			x->pid = -1;
			goto wow;
		}
		x = x->next;
	}
	return -1;//the process was not allocated and still were asked to release!
	wow:	
	while(tmp != NULL){
		if(tmp->pid == pid){
			if(tmp == l->head){
				l->head = l->head->next;
				l->len--;
			}
			else if(tmp == l->tail){
				l->tail = prev;
				l->tail->next = NULL;
				l->len--;
			}
			else{
				prev->next = tmp->next;
				tmp->next = NULL;
				free(tmp);
				l->len--;
			}
			return 1;
		}
		prev = tmp;
		tmp = tmp->next;
	}
	return -1;

}


void firstFit(memll *m, procll *p){
	proc *pp = p->head;
	mem *mm = m->head;
	int newSize = 0;
	while(pp != NULL){
		if(pp->status == 0){//process is ready to be picked up into the memory
			while(mm != NULL){
				if((mm->msize >= pp->psize) && (pp->status == 0) && (mm->pid == -1)){
					mm->pid = pp->pid;
					newSize = mm->msize - pp->psize;//size of new memory block!
					mm->msize = pp->psize;
					if(newSize != 0)
						insertm(m, mm, newSize);
					pp->status = 1;
				}
				mm = mm->next;
			}
		}
		mm = m->head;
		pp = pp->next;
	}//working perfect


}

void bestFit1(memll *m, procll *p){
	proc *pp = p->head;
	mem *m1, *m2;
	m1 = m2 = m->head;//start from the start of the memory
	int size = 10000, psize, newSize;//maximum diff. between memSize & procSize

	while(pp != NULL){//go through all processes
		if(pp->status == 0){//if the process is not in memory
			psize = pp->psize;
			while(m1 != NULL){//travelling through the memory
				if(m1->pid == -1){
					if(size > (m1->msize - psize)){
						size = m1->msize - psize;
						m2 = m1;
					}
				}
				m1 = m1->next;
			}//m2 points to most efficient memory block
			if((m2->pid == -1) && (m2->msize >= pp->psize)){
				m2->pid = pp->pid;
				newSize = m2->msize - pp->psize;//size of new memory block!
				m2->msize = pp->psize;
				if(newSize != 0)
					insertm(m, m2, newSize);
				pp->status = 1;
			}
			m1 = m->head;
		}
		size = 10000;
		pp = pp->next;
	}//working perfect
}

void bestFit(memll *m, procll *p){
	proc *pp = p->head;
	mem *m1 = m->head, *m2;
	int msize, distance = 10000, psize, dist, newSize;
	while(pp != NULL){
		if(pp->status == 0){
			psize = pp->psize;
			while(m1 != NULL){
				if(m1->pid == -1){
					msize = m1->msize;
					dist = msize - psize;
					if(dist > -1){
						if(dist < distance){
							distance = dist;
							m2 = m1;
						}
					}
				}
				m1 = m1->next;
			}
			if((m2->pid == -1) && (m2->msize >= pp->psize)){//perfect block!
				m2->pid = pp->pid;
					newSize = m2->msize - pp->psize;//size of new memory block!
					m2->msize = pp->psize;
					if(newSize != 0)
						insertm(m, m2, newSize);
					pp->status = 1;
			}
			distance = 10000;
			m1 = m->head;
		}
		pp = pp->next;
	}
	pp = p->head;
	m1 = m->head;

}

void worstFit(memll *m, procll *p){
	mem *m1, *m2;//m1 travels through memory & m2 points to the biggest mem block;
	m1 = m2 = m->head;
	int size = m1->msize, newSize;//size of first block;
	proc *pp = p->head;


	while(pp != NULL){//go through all processes!
		if(pp->status == 0){//process is not allocated.
			while(m1 != NULL){//travel through memory finding largest block!
				if(m1->pid == -1){//the block is free!
					if(m1->msize >= size){
						size = m1->msize;
						m2 = m1;
					}
				}
				m1 = m1->next;
			}
			//now m2  is the greatest memory block.
			if((m2->pid == -1) && (pp->psize <= size)){//again check for the freeness of this block
				m2->pid = pp->pid;
				newSize = m2->msize - pp->psize;//size of new memory block!
				m2->msize = pp->psize;
				if(newSize != 0)
					insertm(m, m2, newSize);
				pp->status = 1;
			}
		}
		m1 = m2 = m->head;
		pp = pp->next;
		size = m1->msize, newSize;
	}//good
}

void nextFit(memll *m, procll *p){
	proc *pp = p->head;
	
	mem *mm = currmem;
	int newSize = 0;
	while(pp != NULL){
		if(pp->status == 0){//process is ready to be picked up into the memory
			while(mm != NULL){
				if((mm->msize >= pp->psize) && (pp->status == 0) && (mm->pid == -1)){
					mm->pid = pp->pid;
					newSize = mm->msize - pp->psize;//size of new memory block!
					mm->msize = pp->psize;
					if(newSize != 0)
						insertm(m, mm, newSize);
					pp->status = 1;
					currmem = mm->next;
					break;
				}
				mm = mm->next;
			}
			mm = m->head;
		}
		pp = pp->next;
	}//working perfect

}

int external(memll *l){
	mem *m = l->head;
	int sum = 0;
	while(m != NULL){
		if(m->pid == -1){
			sum += m->msize;
		}
		m = m->next;
	}
	return sum;
}

int main(){
	memll ff, nf, bf, wf;
	procll ffpro, nfpro, bfpro, wfpro;
	initm(&ff);
	initm(&nf);
	currmem = nf.head;
	initm(&bf);
	initm(&wf);
	initp(&ffpro);
	initp(&wfpro);
	initp(&nfpro);
	initp(&bfpro);


	int ans = 0, ch = 0, si, id = 1, hm = 1;
	while(ans != 4){
		printf("Chose an option:-\n1)new process\n2)release process\n3)show memory\n4)Exit\n");
		scanf("%d", &ans);
		switch(ans){
			case 1:
				printf("Enter process size:-");
				scanf("%d", &si);
				insertp(&ffpro, id, si);
				insertp(&nfpro, id, si);
				insertp(&bfpro, id, si);
				insertp(&wfpro, id, si);
				id++;
			break;

			case 2:
				printf("Enter process id:-");
				scanf("%d", &si);
				if(releasep(&ffpro, &ff, si) == -1)
					printf("No such process in the memory!\n");
				if(releasep(&nfpro, &nf, si) == -1)
					printf("No such process in the memory!\n");
				if(releasep(&bfpro, &bf, si) == -1)
					printf("No such process in the memory!\n");
				if(releasep(&wfpro, &wf, si) == -1)
					printf("No such process in the memory!\n");
			break;

			case 3:
				
				ch = 0;
				while(ch != 5){
					printf("MENU:-\n1)First Fit\n2)Next Fit\n3)Best Fit\n4)Worst Fit\n5)Exit\n");
					scanf("%d", &ch);
					switch(ch){
						case 1:
							firstFit(&ff, &ffpro);
							printf("***************printing processes***************\nPID\tSIZE\tSTATUS\n");
							proc *p1 = ffpro.head;
							while(p1 != NULL){
								printf("%d\t%d\t%d\n", p1->pid, p1->psize, p1->status);
								p1 = p1->next;
							}
							printf("***************printing first Fit Output***************//if pid is -1, then block is free\nMID\tSIZE\tPID\n");
							mem *m1 = ff.head;
							while(m1 != NULL){
								printf("%d\t%d\t%d\n", hm, m1->msize, m1->pid);
								m1 = m1->next;
								hm++;
							}
							printf("External fragmentaion of:- %dk\n", external(&ff));
							hm = 1;
						break;

						case 2:
							nextFit(&nf, &nfpro);
							printf("***************printing processes***************\nPID\tSIZE\tSTATUS\n");
							p1 = nfpro.head;
							while(p1 != NULL){
								printf("%d\t%d\t%d\n", p1->pid, p1->psize, p1->status);
								p1 = p1->next;
							}
							printf("***************printing next Fit Output***************//if pid is -1, then block is free\nMID\tSIZE\tPID\n");
							m1 = nf.head;
							while(m1 != NULL){
								printf("%d\t%d\t%d\n", hm, m1->msize, m1->pid);
								m1 = m1->next;
								hm++;
							}
							printf("External fragmentaion of:- %dk\n", external(&nf));
						
							hm = 1;
					
						break;

						case 3:
							bestFit(&bf, &bfpro);
							printf("***************printing processes***************\nPID\tSIZE\tSTATUS\n");
							p1 = bfpro.head;
							while(p1 != NULL){
								printf("%d\t%d\t%d\n", p1->pid, p1->psize, p1->status);
								p1 = p1->next;
							}
							printf("***************printing best Fit Output***************//if pid is -1, then block is free\nMID\tSIZE\tPID\n");
							m1 = bf.head;
							while(m1 != NULL){
								printf("%d\t%d\t%d\n", hm, m1->msize, m1->pid);
								m1 = m1->next;
								hm++;
							}
							printf("External fragmentaion of:- %dk\n", external(&bf));
						
							hm = 1;
						break;

						case 4:
							worstFit(&wf, &wfpro);
							printf("***************printing processes***************\nPID\tSIZE\tSTATUS\n");
							p1 = wfpro.head;
							while(p1 != NULL){
								printf("%d\t%d\t%d\n", p1->pid, p1->psize, p1->status);
								p1 = p1->next;
							}
							printf("***************printing worst Fit Output***************//if pid is -1, then block is free\nMID\tSIZE\tPID\n");
							m1 = wf.head;
							while(m1 != NULL){
								printf("%d\t%d\t%d\n", hm, m1->msize, m1->pid);
								m1 = m1->next;
								hm++;
							}
							printf("External fragmentaion of:- %dk\n", external(&wf));
							hm = 1;
						break;

						case 5:
						break;

						default:
						break;
					}
				}
			break;
			case 4:
				exit(0);
			break;
			
			case 6:
				currmem = nf.head;
			break;

			default:
			break;
		}
	
	}

	return 0;
}