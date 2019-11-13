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

    cl2sv msg2sv;
    sv2cl svResp;

    int fd_servidor, fd_cliente;

    /* VERIFICAR SE EXISTE "CP" NO SERVIDOR(access) */
    if(access("CPservidor", F_OK)!=0){
        printf("[ERROR] O servidor nao esta em execucao!\n");
        exit(1);
    };
    /* CRIAR "CP" DO CLIENTE - MINHA (mkfifo) */
    sprintf(msg2sv.fifostr, "CPcliente_%d", getpid());
    mkfifo(msg2sv.fifostr, 0600); //0600 READ && WRITE
    /* ABRIR "CP" DO SERVIDOR (open - O_WRONLY) */
    fd_servidor = open("CPservidor", O_WRONLY);

    //registar cliente no servidor
    strcpy(msg2sv.cmd,"register");
    write(fd_servidor, &msg2sv, sizeof(msg2sv));
    fd_cliente = open(msg2sv.fifostr,O_RDONLY);
    read(fd_cliente,&svResp,sizeof(svResp));
    close(fd_cliente);
    if(svResp.code==1){
        printf("[CLIENT] Cliente registado no servidor!\n");
    }else{
        printf("[ERROR] Resposta inesperado do servdor.\n");
        /* FECHAR "CP" DO SERVIDOR (close) */
        close(fd_servidor);
        /* REMOVER "cp" DO CLIENTE - EU (UNLINK) */
        unlink(msg2sv.fifostr);
        exit(1);
    }

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
            if(strcmp(palavra[0],"sair")==0){
                printf("[CLIENT] A sair!\n");
                sair = 1;
            }else if(strcmp(palavra[0],"send")==0){
                strcpy(msg2sv.cmd, "send");
                strcpy(msg2sv.opts, palavra[1]);
                /* ENVIAR PEDIDO PARA "CP" DO SERVIDOR (write) */
                i = write(fd_servidor, &msg2sv, sizeof(msg2sv));
            }else if(strcmp(palavra[0],"desligar")==0){
                printf("[CLIENT] A Desligar servidor!\n");
                strcpy(msg2sv.cmd, "DESLIGAR");
                write(fd_servidor, &msg2sv, sizeof(msg2sv));
                printf("[CLIENT] A sair!\n");
                sair = 1;
            }else if(strcmp(palavra[0],"ping")==0){
                strcpy(msg2sv.cmd, "ping");
                /* ENVIAR PEDIDO PARA "CP" DO SERVIDOR (write) */
                i = write(fd_servidor, &msg2sv, sizeof(msg2sv));
                //ler resposta do servidor
                fd_cliente = open(msg2sv.fifostr,O_RDONLY);
                read(fd_cliente,&svResp,sizeof(svResp)); 
                close(fd_cliente);
                //processar resposta
                printf(svResp.resp);   
            }
        }

    }while(sair==0);

    /* FECHAR "CP" DO SERVIDOR (close) */
    close(fd_servidor);
    /* REMOVER "cp" DO CLIENTE - EU (UNLINK) */
    unlink(msg2sv.fifostr);
}