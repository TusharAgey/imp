#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#define MAX 100 //Maximum 100 elements in reference string
typedef struct pages{
	char val;//what is my value
	int num;//when did i come
	struct pages *next;
}page;
typedef struct linklist{
	page *head, *tail;
	int len;
}list;
int number = 4;//next page index
void init(list *ll){
	page *tmp = (page *)malloc(sizeof(page));
	tmp->val = '#';//invalid page initially inserted
	tmp->num = 1;
	tmp->next = NULL;
	ll->head = ll->tail = tmp;
	
	tmp = (page *) malloc(sizeof(page));
	tmp->val = '#';//invalid page initially inserted
	tmp->num = 2;
	tmp->next = NULL;
	ll->tail->next = tmp;
	ll->tail = tmp;

	tmp = (page *) malloc(sizeof(page));
	tmp->val = '#';//invalid page initially inserted
	tmp->num = 3;
	tmp->next = NULL;
	ll->tail->next = tmp;
	ll->tail = tmp;	
}
void copy(list *ll, int i, int output[3][MAX]){
	page *p = ll->head;
	int j = 0;
	while(p){
		if(p->val != '#')
			output[j][i] = (int)((p->val) - 48);
		else
			output[j][i] = 0;
		p = p->next;
		j++;
	}
}
page *smallestIndex(list *ll){
	page *p, *tmp;
	p = tmp = ll->head;
	int x = 1000;//initialized to find smallest oldest(came first) all currently in the frames
	while(p){
		if(p->num < x){
			tmp = p;
			x = p->num;
		}
		p = p->next;
	}
	return tmp;
}
int fifo(list *ll, char val){
	page *p = ll->head;
	while(p){
		if(p->val == val)
			return 0;//page is already present
		p = p->next;
	}
	p = ll->head;
	if(p->val == '#')p->val = val;//first frame is free
	else if(p->next->val == '#')p->next->val = val;//second frame is free
	else if(p->next->next->val == '#') p->next->next->val = val;//third frame is free
	else{//fifo
		p = smallestIndex(ll);
		p->val = val;
		p->num = number;
		number++;
	}
	return 1;
}
int main(){
	int output[3][MAX], pf = 0, len, i, j;
	char ref[MAX];
	list ll;
	init(&ll);
	printf("Enter the reference string:-\n");
	scanf("%s", ref);
	len = strlen(ref);
	for(i = 0; i < 3; i++){//initializing output array
		for(j = 0; j < len; j++){
			output[i][j] = 0;
		}
	}
	for(i = 0; i < len; i++){
		pf += fifo(&ll, ref[i]);
		copy(&ll, i, output);
	}
	printf("\nPage faults:- %d\n\n", pf);
	for(i = 0; i < 3; i++){
		for(j = 0; j < len; j++){
			printf("|%d|", output[i][j]);
		}
		printf("\n");
	}
	return 0;
}
