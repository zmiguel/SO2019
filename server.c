#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <ncurses.h>
//declaraao de funcoes
#include "dec.h"
//estructuras e outras coisas uteis
#include "util.h"

int main(int argc, char *argv[]){
    int sair=0, mopt=-1, n;

    msg mensagem;

    srand(time(NULL));
    int fd_servidor;
    /* VERIFICAR SE EXISTE "CP" DO SERVIDOR (access) -- APENAS UM!!!*/
    if(access("CPservidor", F_OK)==0){
        printf("[SERVIDOR] Ja existe um servidor!\n");
        exit(1);
    }
    /* CRIAR "CP" DO SERVIDOR - MINHA (mkfifo) */
    mkfifo("CPservidor", 0600);
    /* ABRIR "CP" DO SERVIDOR - MINHA (open - O_RDONLY) */
    fd_servidor = open("CPservidor", O_RDWR);

    printf("[SERVER] Servidor Iniciado!\n");

    /* Fazer coisas aqui! */
    do{

        n = read(fd_servidor,&mensagem, sizeof(mensagem));

        if(n == 0){
            printf("[SERVER] deu merda!\n");
            sleep(1);
            continue;
        }else if(strcmp(mensagem.titulo,"send")==0){
            printf("funciona!\n");
            printf("%s\n",mensagem.corpo);    
        }

    }while(true);

    printf("[SERVER] SERVIDOR DESLIGADO\n");

    /* FECHAR "CP" DO SERVIDOR - MINHA (close) */
    close(fd_servidor);
    /* REMOVER "CP" DO SERVIDOR- MINHA (UNLINK) */
    unlink("CPservidor");
    exit(0);
}

void printMenu(void){
    printf("[SERVER] Menu:\n");
    printf("\t1 - coisas\n");
    printf("\t0 - SAIR\n");

    printf("> ");
}

int obtem_rand(int min, int max){
    int random;
    random = min + (rand() % (max - min + 1));
    return random;
}