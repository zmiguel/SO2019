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
    int sair=0, test=-1;
    char str[80], *palavra[10];
    int i;

    msg mensagem;

    int fd_servidor, fd_cliente;

    /* VERIFICAR SE EXISTE "CP" NO SERVIDOR(access) */
    if(access("CPservidor", F_OK)!=0){
        printf("[ERRO] O servidor nao esta em execucao!\n");
        exit(1);
    };
    /* CRIAR "CP" DO CLIENTE - MINHA (mkfifo) */
    sprintf(mensagem.addrss, "CPcliente_%d", getpid());
    mkfifo(mensagem.addrss, 0600); //0600 READ && WRITE
    /* ABRIR "CP" DO SERVIDOR (open - O_WRONLY) */
    fd_servidor = open("CPservidor", O_WRONLY);

    do{
        printf("> ");
        fgets(str, 80, stdin);
        str[strlen(str)-1]='\0';
        i = 0;
        palavra[i]=strtok(str, " ");
        while(palavra[i]!=NULL){
            i++;
            palavra[i] = strtok(NULL," ");
        }
        if(palavra[0]!=NULL){
            if(strcmp(palavra[0],"send")==0){
                strcpy(mensagem.titulo, "send");
                strcpy(mensagem.corpo, palavra[1]);
                /* ENVIAR PEDIDO PARA "CP" DO SERVIDOR (write) */
                i = write(fd_servidor, &mensagem, sizeof(mensagem));
            }
        }

    }while(true);

    /* FECHAR "CP" DO SERVIDOR (close) */
    close(fd_servidor);
    /* REMOVER "cp" DO CLIENTE - EU (UNLINK) */
    unlink(mensagem.addrss);
}