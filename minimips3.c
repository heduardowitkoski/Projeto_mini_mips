#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

typedef enum {tipo_R = 0, tipo_I = 1, tipo_J = 2} Tipo_inst;

typedef struct{
	Tipo_inst tipo_inst;
	char inst_char[18];
	char opcode[6];
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








void preencher_memoria_inst(FILE *file_mem, Instrucao *memoria_inst, int linhas_mem);
void distribuir_campos(Instrucao *memoria_inst, int linhas_mem);
int binario_para_decimal(int n);










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

	preencher_memoria_inst(file_mem, instrucoes, linhas_mem);
	
	distribuir_campos(instrucoes, linhas_mem);
	puts("\n||||||||||||||||||||||||||||||||||||||||||||");
	puts("TIPO R");
	for(int i=2; i<=3; i++)
	{
		putchar('\n');
		printf("Opcode %s\n", instrucoes[i].opcode);
		printf("RS %d\n",instrucoes[i].rs);
		printf("RT %d\n",instrucoes[i].rt);
		printf("RD %d\n",instrucoes[i].rd);
		printf("funct %d\n",instrucoes[i].funct);
	}
	putchar('\n');
	puts("||||||||||||||||||||||||||||||||||||||||||||");
	puts("TIPO I");
	for(int i=0; i<=1; i++)
	{
		printf("Opcode %s\n", instrucoes[i].opcode);
		printf("RS %d\n", instrucoes[i].rs);
		printf("RT %d\n", instrucoes[i].rt);
		printf("imm %d\n\n", instrucoes[i].imm);
	}
	puts("||||||||||||||||||||||||||||||||||||||||||||");
	puts("TIPO J");
		printf("Opcode %s\n", instrucoes[10].opcode);
		printf("addr %d\n", instrucoes[10].addr);
	
	fclose(file_mem);
return 0;
}









void preencher_memoria_inst(FILE *file_mem, Instrucao *memoria_inst, int linhas_mem){
	for (int i=0; i < linhas_mem; i++)
		fgets(memoria_inst[i].inst_char, sizeof(memoria_inst[i].inst_char), file_mem);
}











void distribuir_campos(Instrucao *memoria_inst, int linhas_mem){
	char opcode_temp[6], rs_temp[4], rt_temp[4], rd_temp[4], funct_temp[4], imm_temp[7], addr_temp[8]; 
	 


	
	for(int i=0; i<linhas_mem; i++)
	{
		strncpy(opcode_temp, memoria_inst[i].inst_char, 4);
		if(strcmp(opcode_temp, "0000") == 0)
			memoria_inst[i].tipo_inst = tipo_R;
		else if(strcmp(opcode_temp, "0100") == 0 || strcmp(opcode_temp, "1011") == 0 || strcmp(opcode_temp, "1111") == 0 || strcmp(opcode_temp, "1000") == 0)
			memoria_inst[i].tipo_inst = tipo_I;
		else if(strcmp(opcode_temp, "0010") == 0)
			memoria_inst[i].tipo_inst = tipo_J;
		
		switch (memoria_inst[i].tipo_inst)
		{
			case tipo_R:
				strcpy(memoria_inst[i].opcode, opcode_temp);
				strncpy(rs_temp, memoria_inst[i].inst_char + 4, 3);
				strncpy(rt_temp, memoria_inst[i].inst_char + 7, 3);
				strncpy(rd_temp, memoria_inst[i].inst_char + 10, 3);
				strncpy(funct_temp, memoria_inst[i].inst_char + 13, 3);
				memoria_inst[i].rs = binario_para_decimal(atoi(rs_temp));
				memoria_inst[i].rt = binario_para_decimal(atoi(rt_temp));
				memoria_inst[i].rd = binario_para_decimal(atoi(rd_temp));
				break;
			case tipo_I:
				strcpy(memoria_inst[i].opcode, opcode_temp);
				strncpy(rs_temp, memoria_inst[i].inst_char + 4, 3);
				strncpy(rt_temp, memoria_inst[i].inst_char + 7, 3);
				strncpy(imm_temp, memoria_inst[i].inst_char + 10, 6);
				memoria_inst[i].rs = binario_para_decimal(atoi(rs_temp));
				memoria_inst[i].rt = binario_para_decimal(atoi(rt_temp));
				memoria_inst[i].imm = binario_para_decimal(atoi(imm_temp));
				break;
			case tipo_J:
				strcpy(memoria_inst[i].opcode, opcode_temp);
				strncpy(addr_temp, memoria_inst[i].inst_char + 9, 7);
				memoria_inst[i].addr = binario_para_decimal(atoi(addr_temp));
				break;
		}

	}
}


int binario_para_decimal(int n){

	int decimal = 0, i = 0, resto;

	while (n != 0)
	{
		resto = n % 10;
		n /= 10;
		decimal += resto * pow(2, i);
		i++;
	}
	
	return decimal;

}
