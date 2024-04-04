#include <stdio.h>
#include <stdlib.h>
#include <string.h>


typedef enum {tipo_I, tipo_J, tipo_R, tipo_OUTROS} Tipo_inst;

typedef struct{
	Tipo_inst tipo_inst;
	char inst_char[20];
	int opcode;
	int rs;
	int rt;
	int rd;
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
	fclose(file_mem);
return 0;
}




void preencher_memoria(FILE *file_mem, Instrucao *memoria, int linhas_mem){
	for (int i=0; i < linhas_mem; i++)
		fgets(memoria[i].inst_char, sizeof(memoria[i].inst_char), file_mem);
}
