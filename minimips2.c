#include <stdio.h>
#include <stdlib.h>
#include <string.h>


typedef enum {tipo_I_addi = 0100, tipo_I_lw = 1011, tipo_I_beq = 1000, tipo_I_sw = 1111, tipo_J, tipo_R = 0000, tipo_OUTROS} Tipo_inst;

typedef struct{
	Tipo_inst tipo_inst;
	char inst_char[18];
	int opcode;
	char rs[4];
	char rt[4];
	char rd[4];
	int funct;
	int imm;
	int addr;
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
	for(int i=0; i<linhas_mem; i++)
	{
		printf("%s\n",instrucoes[i].rs);
		printf("%s\n",instrucoes[i].rt);
		printf("%s\n",instrucoes[i].rd);
	}
	
	fclose(file_mem);
return 0;
}




void preencher_memoria(FILE *file_mem, Instrucao *memoria, int linhas_mem){
	for (int i=0; i < linhas_mem; i++)
		fgets(memoria[i].inst_char, sizeof(memoria[i].inst_char), file_mem);
}


void distribuir_campos(Instrucao *memoria, int linhas_mem){
	char opcode_temp[5];
	for(int i=0; i<linhas_mem; i++)
	{
		strncpy(opcode_temp, memoria[i].inst_char, 4);
		memoria[i].opcode = atoi(opcode_temp);
		switch (memoria[i].opcode)
		{
			case tipo_R:
				strncpy(memoria[i].rs, memoria[i].inst_char + 4, 3);
				strncpy(memoria[i].rt, memoria[i].inst_char + 7, 3);
				strncpy(memoria[i].rd, memoria[i].inst_char + 10, 3);
				break;
		}

	}








}
