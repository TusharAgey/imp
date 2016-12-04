#include<stdio.h>
#include<string.h>

struct process{
	int pid;
	int size;
	int status; // 1 = finished, 0 = not-finished
}proc[4] = {{-1, -1, 1},{-1, -1, 1},{-1, -1, 1},{-1, -1, 1}};

struct memory{
	int mid;
	int size;
	int status; //1 = allocated, 0 = free
	int pid; //contains the respective process
	int IF;
}mem[4] = {{1, 20, 0, -1, 0}, {2, 50, 0, -1, 0}, {3, 30, 0, -1, 0}, {4, 28, 0, -1, 0}};
int flag = -2;
int insert(int pid, int size){
	int i = 0;
	while(i < 4){
		if(proc[i].status == 1){
			proc[i].status = 0;
			proc[i].pid = pid;
			proc[i].size = size;
			return 1;
		}
		i++;
	}
	return -1;
}

int delete(int pid){
	int i = 0, j;
	while(i < 4){
		if(proc[i].pid == pid){
			j = i;
			i = 0;
			while(i < 4){
				if(mem[i].pid == pid){
					mem[i].pid = -1;
					mem[i].status = 0;
					break;
				}
				i++;
			}
			proc[j].status = 1;
			proc[j].size = -1;
			proc[j].pid = -1;
			return 1;
		}
		i++;
	}
	return -1;
}

int maxAvailable(){
	int i = 0, max = -1;
	while(i < 4){
		if((mem[i].status == 0) && max < mem[i].size){
			max = mem[i].size;
		}
		i++;
	}
	return max;
}
int itsSize(int val){
	if(val < 10)
		return 1;
	if(val > 10 && val < 100)
		return 2;
	if(val > 99 && val < 1000)
		return 3;
}
void firstFit(int currproc, int psize){
	int i = 0, j = 0;
	while(i < 4){
		if(mem[i].status == 0){
			if(psize <= mem[i].size){
				if(mem[i].pid == -1){
					mem[i].pid = currproc;
					mem[i].status = 1;
					mem[i].IF = mem[i].size - psize;
					return;
				}
			}
		}
		i++;
	}
}
void drawIt(){
	int i = 1;
	if(flag == -2){
		while(i < 128){
			printf(" ");
			if(i == 15)
				printf("20Kb");
			else if(i == 50)
				printf("50Kb");
			else if(i == 80)
				printf("30Kb");
			else if(i == 110){
				printf("28Kb");
			}
			i++;
		}
		printf("\n");
		flag = 1;
		goto freako;
	}
	if(flag == 1)
		flag = -2;
freako:	i = 0;
	printf("+");
	while(i < 127){
		printf("-");
		i++;
	}
	
	printf("+\n");
}
int getSize(int pid){
	int i = 0;
	while(i < 4){
		if(proc[i].pid == pid)
			return proc[i].size;
		i++;
	}
}

int getmSize(int mid){
	int i = 0;
	while(i < 4){
		if(mem[i].mid == mid)
			return mem[i].size;
		i++;
	}

}
void showMem(){
	int i = 0, spaces = 0, j = 0;
	drawIt();
	printf("|");
	while(i < 4){
		if(mem[i].pid != -1){
			printf("pid = %d, size = %d", mem[i].pid, getSize(mem[i].pid));
			spaces = mem[i].size - strlen("pid = , size = ") - itsSize(mem[i].pid) - itsSize(getSize(mem[i].pid));
			j = 1;
			while(j < spaces){
				printf(" ");
				j++;
			}
			printf("|");
			j = 1;
		}
		else{
			spaces = mem[i].size;
			//printf("%d", spaces);
			
			j = 1;
			while(j < spaces){
				printf(" ");
				j++;
			}
			printf("|");
		}
		i++;
	}
	printf("\n");
	drawIt();
}
int getExternalFragment(){
	int i = 0, sum = 0;
	while(i < 4){
		if(mem[i].pid == -1)
			sum += mem[i].size;
		i++;
	}
	return sum;
}
int main(){
	int ch = -1, size, pid = 1, i = 0;
	while(ch != 4){
		printf("\n\n/********************** printing processes**********************************/\nPID\tSIZE\tSTATUS//0 = not-finished & 1 = finished\n");
		while(i < 4){
			printf("%d\t%d\t%d\n", proc[i].pid, proc[i].size, proc[i].status);
			i++;
		}
		i = 0;
		printf("/********************** printing memories**********************************/\nMID\tSIZE\tSTATUS\tPID\tIF\n");
		while(i < 4){
			printf("%d\t%d\t%d\t%d\t%d\n", mem[i].mid, mem[i].size, mem[i].status, mem[i].pid, mem[i].IF);
			i++;
		}
		if(getExternalFragment() != 0)
			printf("\n%dk is the external fragmentation!\n", getExternalFragment());
		i = 0;

		printf("Chose an option:-\n1)new process\n2)release process\n3)show memory\n4)exit\n");
		scanf("%d", &ch);
		switch(ch){
			case 1:
				printf("Enter size of the process:-\n");
				scanf("%d", &size);
				if(size <= maxAvailable()){
					if(insert(pid, size) == -1)
						printf("No space available for process! Discarded.\n");
					else{
						firstFit(pid, size);
						printf("inserted successfully!");
						pid++;
					}
				}
				else{
					printf("No space available for process! Discarded.\n");
				}
			break;
			case 2:
				printf("Enter process id to release:-\n");
				scanf("%d", &pid);
				if(delete(pid) == -1)
					printf("No such process in the memory!\n");
				else
					printf("The process is released successfully!\n");
			break;
			case 3:
				showMem();
			break;
		}
	}
	return 0;
}