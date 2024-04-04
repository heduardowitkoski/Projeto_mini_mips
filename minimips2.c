#include <stdio.h>
#include <stdlib.h>
#include <string.h>


typedef enum {tipo_R = 0, tipo_I = 1, tipo_J = 2} Tipo_inst;

typedef struct{
	Tipo_inst tipo_inst;
	char inst_char[18];
	char opcode[5];
	char rs[4];
	char rt[4];
	char rd[4];
	char funct[4];
	char imm[7];
	char addr[8];
}Instrucao;

typedef struct{
	Instrucao *mem_inst;
	int tamanho;
}Memoria_instrucao;

typedef struct{
	char dado_char[8];
	int dado;
}Dado;

typedef struct{
	Instrucao *mem_dados;
	int tamanho;
}Memoria_dados;









void preencher_memoria(FILE *file_mem, Instrucao *memoria, int linhas_mem);
void distribuir_campos(Instrucao *memoria, int linhas_mem);











int main(){

	Instrucao instrucoes[256];
	FILE *file_mem;
	int linhas_mem=0;
	char barraN;
	

	if((file_mem = fopen("programaTestaInstrucoes.mem", "r")) == NULL)
	{
		puts("Erro: memoria nao foi carregada.");
		return 1;
	}
	
	while((barraN = fgetc(file_mem)) != EOF)
	{
		if(barraN == '\n')
			linhas_mem++;
	}
	rewind(file_mem);
	printf("%d linhas no arquivo.\n\n", linhas_mem);
	preencher_memoria(file_mem, instrucoes, linhas_mem);
	
	
	for(int i=0; i<linhas_mem; i++)
	{
		printf("%s",instrucoes[i].inst_char);
	}
	
	distribuir_campos(instrucoes, linhas_mem);
	puts("\n||||||||||||||||||||||||||||||||||||||||||||");
	puts("TIPO R");
	for(int i=2; i<=3; i++)
	{
		putchar('\n');
		printf("Opcode %s\n", instrucoes[i].opcode);
		printf("RS %s\n",instrucoes[i].rs);
		printf("RT %s\n",instrucoes[i].rt);
		printf("RD %s\n",instrucoes[i].rd);
		printf("funct %s\n",instrucoes[i].funct);
	}
	putchar('\n');
	puts("||||||||||||||||||||||||||||||||||||||||||||");
	puts("TIPO I");
	for(int i=0; i<=1; i++)
	{
		printf("Opcode %s\n", instrucoes[i].opcode);
		printf("RS %s\n", instrucoes[i].rs);
		printf("RT %s\n", instrucoes[i].rt);
		printf("imm %s\n\n", instrucoes[i].imm);
	}
	puts("||||||||||||||||||||||||||||||||||||||||||||");
	puts("TIPO J");
		printf("Opcode %s\n", instrucoes[10].opcode);
		printf("addr %s\n", instrucoes[10].addr);

	fclose(file_mem);
return 0;
}




void preencher_memoria(FILE *file_mem, Instrucao *memoria, int linhas_mem){
	for (int i=0; i < linhas_mem; i++)
		fgets(memoria[i].inst_char, sizeof(memoria[i].inst_char), file_mem);
}


void distribuir_campos(Instrucao *memoria, int linhas_mem){
	
	for(int i=0; i<linhas_mem; i++)
	{
		strncpy(memoria[i].opcode, memoria[i].inst_char, 4);
		if(strcmp(memoria[i].opcode, "0000") == 0)
			memoria[i].tipo_inst = tipo_R;
		else if(strcmp(memoria[i].opcode, "0100") == 0 || strcmp(memoria[i].opcode, "1011") == 0 || strcmp(memoria[i].opcode, "1111") == 0 || strcmp(memoria[i].opcode, "1000") == 0)
			memoria[i].tipo_inst = tipo_I;
		else if(strcmp(memoria[i].opcode, "0010") == 0)
			memoria[i].tipo_inst = tipo_J;
		switch (memoria[i].tipo_inst)
		{
			case tipo_R:
				strncpy(memoria[i].rs, memoria[i].inst_char + 4, 3);
				strncpy(memoria[i].rt, memoria[i].inst_char + 7, 3);
				strncpy(memoria[i].rd, memoria[i].inst_char + 10, 3);
				strncpy(memoria[i].funct, memoria[i].inst_char + 13, 3);
				break;
			case tipo_I:
				strncpy(memoria[i].rs, memoria[i].inst_char + 4, 3);
				strncpy(memoria[i].rt, memoria[i].inst_char + 7, 3);
				strncpy(memoria[i].imm, memoria[i].inst_char + 10, 6);
				break;
			case tipo_J:
				strncpy(memoria[i].addr, memoria[i].inst_char + 9, 7);
				break;
		}

	}








}
