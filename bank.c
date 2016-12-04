#include<stdio.h>
#include<stdlib.h>
#define MAX 10
int equal(int F[3], int H[3]){
	int test = 1;
	int i = 0;
	while(i < 3){
		if(F[i] != H[i])
			return 0;
		i++;
	}
	return 1;
}
int main(){
	int procNum, i, j, safe = 1, val = 0;
	int H[3], R[MAX][3], A[MAX][3], F[3], T[MAX][3], status[MAX];//considering there are 3 types of resources are available
	/*
		H = total number of resources in the system
		R = total request of each process
		A = total resources given to a process
		F = number of resources remaining in the system
		T = number of resources still needed by the processes
	
	*/
	printf("Enter the number of processes:-");
	scanf("%d", &procNum);
	printf("Enter the number of printer, fax machines and plotter available physically:-\n");
	scanf("%d%d%d", &H[0], &H[1], &H[2]);
	printf("Enter the number of resources required by each process:-\n");
	for(i = 0; i < procNum; i++){
		printf("Enter the printer, fax machines and plotter required by process P%d", i);
		for(j = 0; j < 3; j++){
			scanf("%d", &R[i][j]);
		}
		status[i] = 0;//the process is not yet finished.
	}
	
	printf("Enter the number of resources already assigned to each process:-\n");
	for(i = 0; i < procNum; i++){
		printf("Enter the printer, fax machines and plotter assigned to process P%d", i);
		for(j = 0; j < 3; j++){
			scanf("%d", &A[i][j]);
		}
	}
	for(i = 0; i < 3; i++){
		F[i] = H[i];//copying the total available processes in the system to Free pool
	}
	for(i = 0; i < procNum; i++){
		for(j = 0; j < 3; j++)
			F[j] -= A[i][j];//remove the resources that are already allocated to processes from Free pool
	}
	
	for(i = 0; i < procNum; i++){
		for(j = 0; j < 3; j++){
			T[i][j] = R[i][j] - A[i][j];//processese still needed
		}
	}
	while(!equal(F, H) && safe){//until all the resources are not freed.//If all the combinations fails, we will declare deadlock
		for(i = 0; i < procNum; i++){
			if((F[0] >= T[i][0]) && (F[1] >= T[i][1]) && (F[2] >= T[i][2]) && (status[i] == 0)){//process can be freed.
				val--;
				status[i] = 1;//the process if freed.
				F[0] += A[i][0];
				F[1] += A[i][1];
				F[2] += A[i][2];
			}
		}
		val++;
		if(val == procNum){
			safe = 0;//this means none of the processes can proceed.
		}
	}
	if(safe == 0){
		printf("This is a deadlock situation.");
	}
	return 0;
}
