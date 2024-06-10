
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum {tipo_R = 0, tipo_I = 1, tipo_J = 2} Tipo_inst;

typedef struct {
    Tipo_inst tipo_inst;
    char inst_char[18];
    int opcode;
    int rs;
    int rt;
    int rd;
    int funct;
    int imm;
    int addr;
} Instrucao;

typedef struct No {
    int reg_backup[8];
    int pc_backup;
    int dados_backup[256];
    struct No *prox;
    struct No *ant;
} No;


typedef struct _backup{
	No *primeiro;
	No *ultimo;
	int tamanho;
}Backup;


void preencher_memoria_inst(FILE *file_mem, Instrucao *memoria_inst, int linhas_mem);
void distribuir_campos(Instrucao *memoria_inst, int linhas_mem);
int binario_para_decimal(const char *binario);
int binario_para_decimal_c2(const char *binario, int bits);
void controle(Instrucao *instrucoes, int linhas_mem, int *registradores, int *dados, int *pc, int *flag);
void ula(Instrucao instrucoes, int *registradores, int *i, int *flag);
void salva_estado_memoria(int *dados);
void salva_asm(Instrucao *memoria_inst, int linhas_mem);
void imprimeregistradores(int *registradores);
void imprimeMemoriadados(int *dados);
void executaUm(Instrucao *instrucoes, int linhas_mem, int *registradores, int *dados, int *pc, int *flag, Backup *backup);
void imprimeInstrucoes(Instrucao *memoria_inst, int linhas_mem);
void imprimeEstatisticas(Instrucao *instrucoes, int linhas_mem);
void imprime_asm(Instrucao *memoria_inst, int linhas_mem);
void carrega_memoria_dados(int *dados);
void overflow(Instrucao instrucoes, int *registradores, int *flag);
void printaBackups(Backup *backup);
No* criaNo(int *registradores, int pc, int *dados);
Backup *criaBackup();
void insereNo(Backup *backup, No *no);
void restauraUltimoBackup(Backup *backup, int *registradores, int *pc, int *dados);


int main() {
    int registradores[8] = {0};
	Instrucao instrucoes[256];
    int dados[256] = {0};
	FILE *file_mem;
    int linhas_mem = 0;
    char barraN;
    int menu;
    int pc = 0; // Inicializa pc com zero
    int flag=0;
	Backup *backup = criaBackup();

    do {
        printf("\n\n===================================MENU===================================\n");
        printf("\nSelecione uma opcao:\n"
               "(1) Para carregar memoria de instrucoes\n"
               "(2) Para executar todas as instrucoes (RUN)\n"
               "(3) Para executar instrucao a instrucao (STEP)\n"
               "(4) Para imprimir todas as instrucoes\n"
               "(5) Para imprimir registradores\n"
               "(6) Para imprimir memoria de dados\n"
               "(7) Para imprimir todo o simulador\n"
               "(8) Para salvar estado da memoria de dados\n"
               "(9) Para carregar memória de dados\n"
               "(10)Para salvar o programa em nível de montagem\n"
			   "(11)Para voltar uma instrucao\n"
			   "(12)Para ver backups\n"
               "(0) Para sair\n");
        scanf("%d", &menu);

        switch (menu) {
            case 1:
                if ((file_mem = fopen("multiplicacao.mem", "r")) == NULL) {
                    puts("Erro: memoria nao foi carregada.");
                    return 1;
                }

                while ((barraN = fgetc(file_mem)) != EOF) {
                    if (barraN == '\n')
                        linhas_mem++;
                }

                rewind(file_mem);
                preencher_memoria_inst(file_mem, instrucoes, linhas_mem);
                distribuir_campos(instrucoes, linhas_mem);
                fclose(file_mem);
                break;

            case 2:
                controle(instrucoes, linhas_mem, registradores, dados, &pc, &flag);
                break;

            case 3:
		executaUm(instrucoes, linhas_mem, registradores, dados, &pc, &flag, backup);
		puts("\n\n");
		imprime_asm(instrucoes, linhas_mem);
		imprimeregistradores(registradores);
		printf("\n\nPC = %d\n\n", pc);
                break;

            case 4:
                imprimeInstrucoes(instrucoes, linhas_mem);
		imprime_asm(instrucoes, linhas_mem);
				break;
	
            case 5:
                imprimeregistradores(registradores);
                break;

            case 6:
                imprimeMemoriadados(dados);
                break;

            case 7:
                imprimeInstrucoes(instrucoes, linhas_mem);
		imprime_asm(instrucoes, linhas_mem);
                imprimeregistradores(registradores);
                imprimeMemoriadados(dados);
                printf("\nFlag = %d", flag);
		printf("\n\nPC = %d\n\n", pc);
                imprimeEstatisticas(instrucoes, linhas_mem);
				break;

            case 8:
                salva_estado_memoria(dados);
                break;
           
           case 9:
		carrega_memoria_dados(dados);
		break;
		
           case 10:
                salva_asm(instrucoes, linhas_mem);
                break;
			case 11:
				restauraUltimoBackup(backup, registradores, &pc, dados);
				break;
			case 12:
				printaBackups(backup);
				break;
        }
    } while (menu != 0);

    return 0;
}

void preencher_memoria_inst(FILE *file_mem, Instrucao *memoria_inst, int linhas_mem) {
    for (int i = 0; i < linhas_mem; i++)
        fgets(memoria_inst[i].inst_char, sizeof(memoria_inst[i].inst_char), file_mem);
}

void distribuir_campos(Instrucao *memoria_inst, int linhas_mem) {
    char opcode_temp[6], rs_temp[4], rt_temp[4], rd_temp[4], funct_temp[4], imm_temp[7], addr_temp[8];

    for (int i = 0; i < linhas_mem; i++) {
        strncpy(opcode_temp, memoria_inst[i].inst_char, 4);
        opcode_temp[4] = '\0';
        memoria_inst[i].opcode = binario_para_decimal(opcode_temp);

        if (memoria_inst[i].opcode == 0)
            memoria_inst[i].tipo_inst = tipo_R;
        else if (memoria_inst[i].opcode == 4 || memoria_inst[i].opcode == 11 || memoria_inst[i].opcode == 15 ||
                 memoria_inst[i].opcode == 8)
            memoria_inst[i].tipo_inst = tipo_I;
        else if (memoria_inst[i].opcode == 2)
            memoria_inst[i].tipo_inst = tipo_J;

        switch (memoria_inst[i].tipo_inst) {
            case tipo_R:
                strncpy(rs_temp, memoria_inst[i].inst_char + 4, 3);
                rs_temp[3] = '\0';
                strncpy(rt_temp, memoria_inst[i].inst_char + 7, 3);
                rt_temp[3] = '\0';
                strncpy(rd_temp, memoria_inst[i].inst_char + 10, 3);
                rd_temp[3] = '\0';
                strncpy(funct_temp, memoria_inst[i].inst_char + 13, 3);
                funct_temp[3] = '\0';
                memoria_inst[i].rs = binario_para_decimal(rs_temp);
                memoria_inst[i].rt = binario_para_decimal(rt_temp);
                memoria_inst[i].rd = binario_para_decimal(rd_temp);
                memoria_inst[i].funct = binario_para_decimal(funct_temp);
                break;

            case tipo_I:
                strncpy(rs_temp, memoria_inst[i].inst_char + 4, 3);
                rs_temp[3] = '\0';
                strncpy(rt_temp, memoria_inst[i].inst_char + 7, 3);
                rt_temp[3] = '\0';
                strncpy(imm_temp, memoria_inst[i].inst_char + 10, 6);
                imm_temp[6] = '\0';
                memoria_inst[i].rs = binario_para_decimal(rs_temp);
                memoria_inst[i].rt = binario_para_decimal(rt_temp);
                memoria_inst[i].imm = binario_para_decimal_c2(imm_temp, 6);
                break;

            case tipo_J:
                strncpy(addr_temp, memoria_inst[i].inst_char + 9, 7);
                addr_temp[7] = '\0';
                memoria_inst[i].addr = binario_para_decimal_c2(addr_temp, 7);
                break;
        }
    }
}

int binario_para_decimal(const char *binario) {
    int decimal = 0;
    while (*binario != '\0') {
        decimal = decimal * 2 + (*binario - '0');
        binario++;
    }
    return decimal;
}

int binario_para_decimal_c2(const char *binario, int bits) {
    int decimal = 0;
    int sinal = 0;


    if (binario[0] == '1') {
        sinal = 1;
    }

    for (int i = 0; i < bits; i++) {
        decimal = decimal * 2 + (binario[i] - '0');
    }


    if (sinal) {
        decimal -= (1 << bits);
    }

    return decimal;
}

void controle(Instrucao *instrucoes, int linhas_mem, int *registradores, int *dados, int *pc, int *flag) {
    for (*pc = 0; *pc < linhas_mem; (*pc)++) {
        switch (instrucoes[*pc].opcode) {
            case 0:
                ula(instrucoes[*pc], registradores, pc, flag);
                break;
            case 4:
                ula(instrucoes[*pc], registradores, pc, flag);
                break;
            case 15:
                dados[instrucoes[*pc].imm] = registradores[instrucoes[*pc].rt];
                break;
            case 11:
                registradores[instrucoes[*pc].rt] = dados[instrucoes[*pc].imm];
                break;
            case 8:
                if (registradores[instrucoes[*pc].rs] == registradores[instrucoes[*pc].rt])
                    *pc = *pc + instrucoes[*pc].imm;
                break;
            case 2:
                *pc = instrucoes[*pc].addr - 1;
                break;
        }
    }
}


void ula(Instrucao instrucoes, int *registradores, int *i, int *flag){
	switch(instrucoes.opcode)
	{
		case 0:		
			switch(instrucoes.funct)
			{
				case 0: // add
					registradores[instrucoes.rd] = registradores[instrucoes.rs] + registradores[instrucoes.rt];
                    overflow(instrucoes, registradores, flag);
					break;
				case 2: // sub
					registradores[instrucoes.rd] = registradores[instrucoes.rs] - registradores[instrucoes.rt];
                    overflow(instrucoes, registradores, flag);
					break;
				case 4: // and
					registradores[instrucoes.rd] = registradores[instrucoes.rs] & registradores[instrucoes.rt];
					break;
				case 5: // or
					registradores[instrucoes.rd] = registradores[instrucoes.rs] | registradores[instrucoes.rt];
					break;
			}
			break;
		case 4:
			registradores[instrucoes.rt] = registradores[instrucoes.rs] + instrucoes.imm;
            overflow(instrucoes, registradores, flag);
			break;
	}



}


void salva_estado_memoria(int *dados) {
    FILE *arquivo;
    int i;
    if ((arquivo = fopen("memoriadados.data", "w")) == NULL) {
        printf("Erro na abertura do arquivo");
        return;
    }
    for (i = 0; i < 256; i++) {
        fprintf(arquivo, "%i\n", dados[i]);
    }
    fclose(arquivo);
}

void salva_asm(Instrucao *memoria_inst, int linhas_mem) {
    FILE *arquivo;
    if ((arquivo = fopen("ProgramaAssembly.asm", "w")) == NULL) {
        printf("Erro na abertura do arquivo");
        return;
    }
    for (int i = 0; i < linhas_mem; i++) {
        switch (memoria_inst[i].tipo_inst) {
            case tipo_R:
                switch (memoria_inst[i].funct) {
                    case 0:
                        fprintf(arquivo, "add $%i, $%i, $%i\n", memoria_inst[i].rd, memoria_inst[i].rs, memoria_inst[i].rt);
                        break;
                    case 2:
                        fprintf(arquivo, "sub $%i, $%i, $%i\n", memoria_inst[i].rd, memoria_inst[i].rs, memoria_inst[i].rt);
                        break;
                    case 4:
                        fprintf(arquivo, "and $%i, $%i, $%i\n", memoria_inst[i].rd, memoria_inst[i].rs, memoria_inst[i].rt);
                        break;
                    case 5:
                        fprintf(arquivo, "or $%i, $%i, $%i\n", memoria_inst[i].rd, memoria_inst[i].rs, memoria_inst[i].rt);
                        break;
                }
                break;
            case tipo_I:
                switch (memoria_inst[i].opcode) {
                    case 4:
                        fprintf(arquivo, "addi $%i, $%i, %i\n", memoria_inst[i].rs, memoria_inst[i].rt, memoria_inst[i].imm);
                        break;
                    case 11:
                        fprintf(arquivo, "lw $%i, %i($%i)\n", memoria_inst[i].rt, memoria_inst[i].imm, memoria_inst[i].rs);
                        break;
                    case 15:
                        fprintf(arquivo, "sw $%i, %i($%i)\n", memoria_inst[i].rt, memoria_inst[i].imm, memoria_inst[i].rs);
                        break;
                    case 8:
                        fprintf(arquivo, "beq $%i, $%i, %i\n", memoria_inst[i].rs, memoria_inst[i].rt, memoria_inst[i].imm);
                        break;
                }
                break;
            case tipo_J:
                fprintf(arquivo, "J %i\n", memoria_inst[i].addr);
                break;
        }
    }
    fclose(arquivo);
}

void imprime_asm(Instrucao *memoria_inst, int linhas_mem) {
    printf("INSTRUCOES .ASM:\n");
	for (int i = 0; i < linhas_mem; i++) {
        switch (memoria_inst[i].tipo_inst) {
            case tipo_R:
                switch (memoria_inst[i].funct) {
                    case 0:
                        printf("add $%i, $%i, $%i\n", memoria_inst[i].rd, memoria_inst[i].rs, memoria_inst[i].rt);
                        break;
                    case 2:
                        printf("sub $%i, $%i, $%i\n", memoria_inst[i].rd, memoria_inst[i].rs, memoria_inst[i].rt);
                        break;
                    case 4:
                        printf("and $%i, $%i, $%i\n", memoria_inst[i].rd, memoria_inst[i].rs, memoria_inst[i].rt);
                        break;
                    case 5:
                        printf("or $%i, $%i, $%i\n", memoria_inst[i].rd, memoria_inst[i].rs, memoria_inst[i].rt);
                        break;
                }
                break;
            case tipo_I:
                switch (memoria_inst[i].opcode) {
                    case 4:
                        printf("addi $%i, $%i, %i\n", memoria_inst[i].rs, memoria_inst[i].rt, memoria_inst[i].imm);
                        break;
                    case 11:
                        printf("lw $%i, %i($%i)\n", memoria_inst[i].rt, memoria_inst[i].imm, memoria_inst[i].rs);
                        break;
                    case 15:
                        printf("sw $%i, %i($%i)\n", memoria_inst[i].rt, memoria_inst[i].imm, memoria_inst[i].rs);
                        break;
                    case 8:
                        printf("beq $%i, $%i, %i\n", memoria_inst[i].rs, memoria_inst[i].rt, memoria_inst[i].imm);
                        break;
                }
                break;
            case tipo_J:
                printf("J %i\n", memoria_inst[i].addr);
                break;
        }
    }
}

void imprimeregistradores(int *registradores) {
    puts("\nREGISTRADORES:");
    for (int i = 0; i < 8; i++)
        printf("registrador[%d]: %d\n", i, registradores[i]);
    putchar('\n');
}

void imprimeMemoriadados(int *dados) {
    puts("MEMORIA DE DADOS:");
    for (int i = 0; i < 16; i++) {
        for (int j = 0; j < 16; j++)
            printf("%d  ", dados[i * 16 + j]);
        putchar('\n');
    }
}

void imprimeEstatisticas(Instrucao *instrucoes, int linhas_mem){

	int classe_r=0, classe_i=0, classe_j=0;
	for(int i=0; i<linhas_mem; i++)
	{
		switch(instrucoes[i].tipo_inst)
		{
			case tipo_R:
				classe_r++;
			break;
			case tipo_I:
				classe_i++;
			break;
			case tipo_J:
				classe_j++;
			break;
		}
	
	}
	printf("Numero de instrucoes = %d\n", linhas_mem);
	printf("\nInstrucoes classe R: %d", classe_r);
	printf("\nInstrucoes classe I: %d", classe_i);
	printf("\nInstrucoes classe J: %d\n", classe_j);
}


void executaUm(Instrucao *instrucoes, int linhas_mem, int *registradores, int *dados, int *pc, int *flag, Backup *backup) {
    if (*pc == linhas_mem) {
        printf("\n\n#########Última instrução já foi executada#########.\n");
        return;
    }

    No *no = criaNo(registradores, *pc, dados);
    insereNo(backup, no);

    switch (instrucoes[*pc].opcode) {
        case 0:
            ula(instrucoes[*pc], registradores, pc, flag);
            break;
        case 4:
            ula(instrucoes[*pc], registradores, pc, flag);
            break;
        case 15:
            dados[instrucoes[*pc].imm] = registradores[instrucoes[*pc].rt];
            break;
        case 11:
            registradores[instrucoes[*pc].rt] = dados[instrucoes[*pc].imm];
            break;
        case 8:
            if (registradores[instrucoes[*pc].rs] == registradores[instrucoes[*pc].rt])
                *pc = *pc + instrucoes[*pc].imm + 1;
            else
                (*pc)++;
            return;
            break;
        case 2:
            *pc = instrucoes[*pc].addr - 1;
            break;
    }

    if(*pc < linhas_mem)
        (*pc)++;
}


void imprimeInstrucoes(Instrucao *memoria_inst, int linhas_mem) {
    puts("\nINSTRUCOES:");
    for (int i = 0; i < linhas_mem; i++)
        printf("%s", memoria_inst[i].inst_char);
    putchar('\n');
}

No* criaNo(int *registradores, int pc, int *dados) {
    No *no = (No *)malloc(sizeof(No));
    for (int i = 0; i < 8; i++) {
        no->reg_backup[i] = registradores[i];
    }
    no->pc_backup = pc;
    for (int i = 0; i < 256; i++) {
        no->dados_backup[i] = dados[i];
    }
    no->prox = NULL;
    no->ant = NULL;
    return no;
}


void insereNo(Backup *backup, No *no) {
    if (backup->tamanho >= 256) {
        No *primeiro = backup->primeiro;
        backup->primeiro = primeiro->prox;
        if (backup->primeiro != NULL) {
            backup->primeiro->ant = NULL;
        } else {
            backup->ultimo = NULL;
        }
        free(primeiro);
    } else {
        if (backup->tamanho == 0) {
            backup->primeiro = no;
            backup->ultimo = no;
        } else {
            no->ant = backup->ultimo;
            backup->ultimo->prox = no;
            backup->ultimo = no;
        }
        backup->tamanho++;
    }
}





Backup *criaBackup(){
	Backup *backup = (Backup *)calloc(1, sizeof(No));
	backup->primeiro = NULL;
	backup->ultimo = NULL;
	backup->tamanho = 0;

	return backup;
}


void printaBackups(Backup *backup){
    if (backup == NULL || backup->primeiro == NULL) {
        printf("Backup vazio.\n");
        return;
    }

    No *aux = backup->primeiro;
    while (aux != NULL) {
		printf("PC [%d]    ", aux->pc_backup);
        for (int j = 0; j < 8; j++)
            printf("%d ", aux->reg_backup[j]);
        putchar('\n');
        aux = aux->prox;
    }
}

void restauraUltimoBackup(Backup *backup, int *registradores, int *pc, int *dados) {
    if (backup == NULL || backup->ultimo == NULL) {
        printf("Nenhum backup para restaurar.\n");
        return;
    }

    No *ultimo = backup->ultimo;

    
    for (int i = 0; i < 8; i++) {
        registradores[i] = ultimo->reg_backup[i];
    }

    
    *pc = ultimo->pc_backup;

    
    for (int i = 0; i < 256; i++) {
        dados[i] = ultimo->dados_backup[i];
    }

    
    if (ultimo->ant != NULL) {
        ultimo->ant->prox = NULL;
    } else {
        
        backup->primeiro = NULL;
    }
    backup->ultimo = ultimo->ant;
    backup->tamanho--;

    free(ultimo);
    printf("Backup restaurado com sucesso.\n");
}






void carrega_memoria_dados(int *dados)
{
FILE *arquivo;
    int i;
    char aux[16];
    if ((arquivo = fopen("memoriadados.data", "r")) == NULL) {
        printf("\nErro na abertura do arquivo");
        return;
    }
    for (i = 0; i < 256; i++) {
        fscanf(arquivo, "%s", aux);
        dados[i]=atoi(aux);
    }
    fclose(arquivo);
}

void overflow(Instrucao instrucoes, int *registradores, int *flag){

    
    if (instrucoes.imm < -32 || instrucoes.imm > 31 || registradores[instrucoes.rt] < -128 || registradores[instrucoes.rt] > 127){
        printf("\nImpossível realizar operação! Overflow!");
        registradores[instrucoes.rt]=0;
        *flag=1;
    }
    else{
         if (registradores[instrucoes.rd] < -128 || registradores[instrucoes.rd] > 127){
        registradores[instrucoes.rd]=0;
        printf("\nImpossível realizar operação! Overflow!");
        *flag=1;
        }

    }
}
