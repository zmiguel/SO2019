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

    sv2cl msg2cl;
    cl2sv clResp;

    srand(time(NULL));
    int fd_servidor, fd_cliente;
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

        n = read(fd_servidor,&clResp, sizeof(clResp));

        if(n == 0){
            printf("[SERVER] deu merda!\n");
            sleep(1);
            continue;
        }else if(strcmp(clResp.cmd,"DESLIGAR")==0){
            printf("[SERVER] O servidor vai desligar\n");
            //implementar desligar outros clientes!
            sair = 1;
        }else if(strcmp(clResp.cmd,"send")==0){
            printf("funciona!\n");
            printf("%s\n",clResp.opts);    
        }else if(strcmp(clResp.cmd,"register")==0){
            msg2cl.code=1;
            //responde
            fd_cliente = open(clResp.fifostr,O_WRONLY);
            write(fd_cliente,&msg2cl,sizeof(msg2cl));
            close(fd_cliente);
            //fim resposta
        }else if(strcmp(clResp.cmd,"ping")==0){
            sprintf(msg2cl.resp,"[SV] Olá %s\n",clResp.fifostr);
            printf("[SERVER] A responder ao Cliente %s",clResp.fifostr);
            //responde
            fd_cliente = open(clResp.fifostr,O_WRONLY);
            write(fd_cliente,&msg2cl,sizeof(msg2cl));
            close(fd_cliente);
            //fim da resposta
        }

    }while(sair==0);

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