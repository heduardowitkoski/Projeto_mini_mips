#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

typedef enum {tipo_R = 0, tipo_I = 1, tipo_J = 2} Tipo_inst;

typedef struct{
	Tipo_inst tipo_inst;
	char inst_char[18];
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




void preencher_memoria_inst(FILE *file_mem, Instrucao *memoria_inst, int linhas_mem);
void distribuir_campos(Instrucao *memoria_inst, int linhas_mem);
int binario_para_decimal(int n);
void controle(Instrucao *instrucoes, int linhas_mem, int *registradores, int *dados);
void ula(Instrucao instrucoes, int *registradores, int i);

int main(){
	
	int registradores[8];
	Instrucao instrucoes[256];
	int dados[256];
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
	
	controle(instrucoes, linhas_mem, registradores, dados);
	
	for(int i = 0; i < 16; i++)
	{
		for(int j = 0; j < 16; j++)
			printf("%d ", dados[i * 16 + j]);
		putchar('\n');
	}
	putchar('\n');
	for(int i=0; i < 8; i++)
		printf("registrador[%d]: %d\n",i, registradores[i]);
	fclose(file_mem);
return 0;
}


void ula(Instrucao instrucoes, int *registradores, int i){

	switch(instrucoes.funct)
	{
		case 0:
			registradores[instrucoes.rd] = registradores[instrucoes.rs] + registradores[instrucoes.rt];
			break;
		case 2:
			registradores[instrucoes.rd] = registradores[instrucoes.rs] - registradores[instrucoes.rt];
			break;
		case 4:
			// and
			break;
		case 5:
			// or
			break;
	}





}


void controle(Instrucao *instrucoes, int linhas_mem, int *registradores, int *dados){
	for(int i = 0; i < linhas_mem; i++)
	{
		switch(instrucoes[i].opcode)
		{
			case 0:
				ula(instrucoes[i], registradores, i);
				break;
			case 4:
				registradores[instrucoes[i].rt] = instrucoes[i].imm; 
				break;
			case 15:
				dados[instrucoes[i].imm] = registradores[instrucoes[i].rt];
				break;
			case 11:
				registradores[instrucoes[i].rt] = dados[instrucoes[i].imm];
				break;
			case 8:
				if(registradores[instrucoes[i].rs] == registradores[instrucoes[i].rt])
					i += instrucoes[i].imm;
				break;
			case 2:
				i += instrucoes[i].imm;
				break;
		}	
	}
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
		opcode_temp[4] = '\0';
		memoria_inst[i].opcode = binario_para_decimal(atoi(opcode_temp));
		if(memoria_inst[i].opcode == 0)
			memoria_inst[i].tipo_inst = tipo_R;
		else if(memoria_inst[i].opcode == 4 || memoria_inst[i].opcode == 11 || memoria_inst[i].opcode == 15 || memoria_inst[i].opcode == 8)
			memoria_inst[i].tipo_inst = tipo_I;
		else if(memoria_inst[i].opcode == 2)
			memoria_inst[i].tipo_inst = tipo_J;
		
		switch (memoria_inst[i].tipo_inst)
		{
			case tipo_R:
				strncpy(rs_temp, memoria_inst[i].inst_char + 4, 3);
				rs_temp[3] = '\0';
				strncpy(rt_temp, memoria_inst[i].inst_char + 7, 3);
				rt_temp[3] = '\0';
				strncpy(rd_temp, memoria_inst[i].inst_char + 10, 3);
				rd_temp[3] = '\0';
				strncpy(funct_temp, memoria_inst[i].inst_char + 13, 3);
				funct_temp[3] = '\0';
				memoria_inst[i].rs = binario_para_decimal(atoi(rs_temp));
				memoria_inst[i].rt = binario_para_decimal(atoi(rt_temp));
				memoria_inst[i].rd = binario_para_decimal(atoi(rd_temp));
				memoria_inst[i].funct = binario_para_decimal(atoi(funct_temp));
				break;
			case tipo_I:
				strncpy(rs_temp, memoria_inst[i].inst_char + 4, 3);
				rs_temp[3] = '\0';
				strncpy(rt_temp, memoria_inst[i].inst_char + 7, 3);
				rt_temp[3] = '\0';
				strncpy(imm_temp, memoria_inst[i].inst_char + 10, 6);
				imm_temp[6] = '\0';
				memoria_inst[i].rs = binario_para_decimal(atoi(rs_temp));
				memoria_inst[i].rt = binario_para_decimal(atoi(rt_temp));
				memoria_inst[i].imm = binario_para_decimal(atoi(imm_temp));
				break;
			case tipo_J:
				strncpy(addr_temp, memoria_inst[i].inst_char + 9, 7);
				addr_temp[7] = '\0';
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
