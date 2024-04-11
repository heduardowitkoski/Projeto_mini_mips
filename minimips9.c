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



void preencher_memoria_inst(FILE *file_mem, Instrucao *memoria_inst, int linhas_mem); // função para ler as instruções no arquivo .mem
void distribuir_campos(Instrucao *memoria_inst, int linhas_mem); // função para distribuir os campos nas structs
int binario_para_decimal(int n); // função auxiliar para converter as instruções para int antes de passar aos campos da struct 
void controle(Instrucao *instrucoes, int linhas_mem, int *registradores, int *dados); // função controle (analisa a instrução e decide se vai chamar a ula, caso seja uma operação lógicas/aritmética)
void ula(Instrucao instrucoes, int *registradores, int i); // função ula (realiza as operações lógicas/aritméticas)

int main(){
	
	int registradores[8]; // 8 registradores
	Instrucao instrucoes[256]; // 256 endereços de instruções
	int dados[256]; // 256 endereços de dados
	FILE *file_mem;
	int linhas_mem=0; // variável que armazena a quantidade de instruções que há no arquivo .mem
	char barraN; // variável usada para contar a quantidade de instruções que há no arquivo .mem
	

	if((file_mem = fopen("programaTestaInstrucoes4.mem", "r")) == NULL)
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
	puts("MEMORIA DE DADOS:");
	for(int i = 0; i < 16; i++) // laço para imprimir toda a memória de dados, em formato de matriz 16x16(para melhor visualização, apenas)
	{
		for(int j = 0; j < 16; j++)
			printf("%d  ", dados[i * 16 + j]);
		putchar('\n');
	}
	putchar('\n');
	puts("REGISTRADORES:");
	for(int i=0; i < 8; i++) // laço para imprimir os 8 registradores;
		printf("registrador[%d]: %d\n",i, registradores[i]);
	fclose(file_mem);
return 0;
}


// função ula, caso a instrução lida seja uma instrução lógica/aritmética, a ula será chamada
void ula(Instrucao instrucoes, int *registradores, int i){
	switch(instrucoes.opcode)
	{
		case 0:		
			switch(instrucoes.funct)
			{
				case 0: // add
					registradores[instrucoes.rd] = registradores[instrucoes.rs] + registradores[instrucoes.rt];
					break;
				case 2: // sub
					registradores[instrucoes.rd] = registradores[instrucoes.rs] - registradores[instrucoes.rt];
					break;
				case 4:
					// and (falta implementar)
					break;
				case 5:
					// or (falta implementar)
					break;
			}
			break;
		case 4:
			registradores[instrucoes.rt] = registradores[instrucoes.rt] + instrucoes.imm;
			break;
	}



}

// função controle, analisa as instruções sequencialmente e determina se deve chamar a função ula ou não
void controle(Instrucao *instrucoes, int linhas_mem, int *registradores, int *dados){ 
	for(int pc = 0; pc < linhas_mem; pc++)// o pc acabou por ser a variável usada pra iterar sobre o vetor de instruções
	{
		switch(instrucoes[pc].opcode)
		{
			case 0: // caso o opcode seja 0, a função ula sera chamada
				ula(instrucoes[pc], registradores, pc);
				break;
			case 4: // addi
				ula(instrucoes[pc], registradores, pc); 
				break;
			case 15: // sw
				dados[instrucoes[pc].imm] = registradores[instrucoes[pc].rt];
				break;
			case 11: // lw
				registradores[instrucoes[pc].rt] = dados[instrucoes[pc].imm];
				break;
			case 8: // beq
				if(registradores[instrucoes[pc].rs] == registradores[instrucoes[pc].rt])
					pc += instrucoes[pc].imm;
				break;
			case 2: // j
				pc += instrucoes[pc].imm;
				break;
		}	
	}
}




// função para ler cada linha do arquivo .mem (cada linha corresponde a uma instrução) e armazenar essa instrução em uma struct do vetor de structs (instrucoes[256])
void preencher_memoria_inst(FILE *file_mem, Instrucao *memoria_inst, int linhas_mem){
	for (int i=0; i < linhas_mem; i++)
		fgets(memoria_inst[i].inst_char, sizeof(memoria_inst[i].inst_char), file_mem); //fgets lê até encontrar o '\n', e então passa para a próxima iteração
}




// função que pega a instrução lida anteriormente na linha do arquivo .mem e separa em seus devidos campos dentro das structs
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


// função auxiliar da função que distribui os campos (serve para transformar um binário representado em string em um decimal representado em int)
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
