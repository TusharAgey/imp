//Assembler
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
int alreadyInserted(char label[100]){
	FILE *sym = fopen("symtab", "r");
	char label2[100];
	int lc, i;
	while(fscanf(sym, "%s %d %d\n", label2, &lc, &i) != EOF){
		if(strcmp(label, label2) == 0){
			fclose(sym);
			return 1;//already inserted, Error
		}
	}
	fclose(sym);
	return 0;//New label. okay!
}
int EqualSign(char label[100]){
	int i = 0;
	int len = strlen(label);
	while(i < len){
		if(label[i] == '=')
			return 1;
		i++;
	}
	return 0;
}
int getInt(char mem[100]){
	int val = 0, i = 0, len = strlen(mem);
	while(i < len){
		if(mem[i] != '=' && mem[i] != '\"'){
			val = (val*10) + (mem[i] - 48);
		}
		i++;
	}
	return val;
}
int ad(char directive[100]){
	if(strcmp(directive, "START") == 0 || strcmp(directive, "END") == 0 || strcmp(directive, "ORIGIN") == 0 || strcmp(directive, "EQU") == 0 || strcmp(directive, "LTORG") == 0) return 1;
	else 
		return 0;
}
int main(){
	FILE *fp = fopen("src.asm", "r");
	FILE *sym = fopen("symtab", "w");
	FILE *is = fopen("assembler_directives", "r");
	FILE *lit = fopen("littab", "w");	
	int lc = 0, i = 0, lcc, litval;
	char label[100], directive[100], reg[100], mem[100], ist[100], tmpchar[100];
	while(fscanf(fp, "%s %s %s %s\n", label, directive, reg, mem) != EOF){//this loop generates the SYMTAB
		if(strcmp(directive, "START") == 0 && strcmp(mem, "$") != 0) lc = atoi(mem);		
		if(strcmp(label, "$") != 0){
			i++;
			fclose(sym);
			if(!alreadyInserted(label)){
				FILE *sym = fopen("symtab", "a+");
				fprintf(sym, "%s %d %d\n", label, lc, i);//label, location counter & logical value.
			}
			else{
				printf("Error in source file:- Cannot use same label more than once");
			}
		}
		if(EqualSign(mem)) fprintf(lit, "%d %d\n", getInt(mem), lc); //literal.
		lc++;
	}
	fclose(sym);//symtab is generated
	fclose(lit);//littab is generated
	sym = fopen("symtab", "r");
	lit = fopen("littab", "r");
	rewind(fp);
	rewind(is);
	lc = 0;
	while(fscanf(fp, "%s %s %s %s\n", label, directive, reg, mem) != EOF){//this loop generates the Pass1 o/p
		if(strcmp(directive, "START") == 0 && strcmp(mem, "$") != 0){
			lc = atoi(mem);
			printf("(AD, 01)");
		}
		else if(strcmp(directive, "START") == 0) printf("(AD, 01)");
		if(strcmp(directive, "END") == 0) printf("(AD, 02)");
		if(strcmp(directive, "ORIGIN") == 0){
			printf("(AD, 03)");
			lc =  atoi(mem);//changing value of lc by ORIGIN directive.
		}
		if(strcmp(directive, "EQU") == 0) printf("(AD, 04)");
		if(strcmp(directive, "LTORG") == 0) printf("(AD, 05)");
		while(fscanf(is, "%s %d\n", ist, &lcc) != EOF){
			if(strcmp(ist, directive) == 0){
				printf("(IS, %d)", lcc);
				break;
			}
		}
		if(strcmp(directive, "DC") == 0) printf("(DS, 01)");
		if(strcmp(directive, "DS") == 0) printf("(DS, 02)");
		rewind(is);
		if(strcmp(reg, "AREG,") == 0) printf("(1)");
		if(strcmp(reg, "BREG,") == 0) printf("(2)");
		if(strcmp(reg, "CREG,") == 0) printf("(3)");
		if(strcmp(reg, "DREG,") == 0) printf("(4)");
		if(strcmp(reg, "LT,") == 0) printf("(1)");
		if(strcmp(reg, "LE,") == 0) printf("(2)");
		if(strcmp(reg, "EQ,") == 0) printf("(3)");
		if(strcmp(reg, "GT,") == 0) printf("(4)");
		if(strcmp(reg, "GE,") == 0) printf("(5)");
		if(strcmp(mem, "$") != 0){
			rewind(sym);
			if((atoi(mem) == 0) && mem[0] != '0'){
				if(EqualSign(mem)) printf("(L, %d)", getInt(mem));
				else
					while(fscanf(sym, "%s %d %d\n", tmpchar, &litval, &i) != EOF)
						if(strcmp(tmpchar, mem) == 0) printf("(S, %d)", i);
			}
			else
				printf("(C, %d)", atoi(mem));
		}
		printf("\n");
		lc++;
	}
	rewind(fp);
	rewind(sym);
	rewind(lit);
	rewind(is);
	while(fscanf(fp, "%s %s %s %s\n", label, directive, reg, mem) != EOF){//this loop generates the pass2 o/p
		if(strcmp(directive, "START") == 0 && strcmp(mem, "$") != 0){
			lc = atoi(mem);
		}
		if(!ad(directive)){//if the line doesn't contain assembler directive,
			printf("%d)", lc);
			if(!(strcmp(directive, "DS") == 0)){
				printf(" +  ");
			}
			rewind(is);
			while(fscanf(is, "%s %d\n", ist, &lcc) != EOF){
				if(strcmp(ist, directive) == 0){
					printf(" %d ", lcc);
					break;
				}
			}
			if(strcmp(directive, "DC") == 0) printf(" 0  0 ");
			if(strcmp(reg, "AREG,") == 0) printf(" 1 ");
			if(strcmp(reg, "BREG,") == 0) printf(" 2 ");
			if(strcmp(reg, "CREG,") == 0) printf(" 3 ");
			if(strcmp(reg, "DREG,") == 0) printf(" 4 ");
			if(strcmp(reg, "LT,") == 0) printf(" 1 ");
			if(strcmp(reg, "LE,") == 0) printf(" 2 ");
			if(strcmp(reg, "EQ,") == 0) printf(" 3 ");
			if(strcmp(reg, "GT,") == 0) printf(" 4 ");
			if(strcmp(reg, "GE,") == 0) printf(" 5 ");
			if((strcmp(reg, "$") == 0) && (strcmp(directive, "DS") != 0)) printf(" 0 ");
			if(strcmp(mem, "$") != 0 && !(strcmp(directive, "DS") == 0)){
				rewind(sym);
				if((atoi(mem) == 0) && mem[0] != '0'){
					if(EqualSign(mem)) printf(" %d ", getInt(mem));
					else
						while(fscanf(sym, "%s %d %d\n", tmpchar, &litval, &i) != EOF)
							if(strcmp(tmpchar, mem) == 0) printf(" %d ", litval);
				}
			}
			else
				if(!(strcmp(directive, "DS") == 0)) printf(" 0 00 ");
			
			lc++;
		}
		printf("\n");
	}
	return 0;
}
