#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <ncurses.h>
#include <signal.h>
//estructuras e outras coisas uteis
#include "util.h"
//declaraao de funcoes
#include "dec.h"

int main(int argc, char *argv[]){
    int sair=0, test=-1;
    char str[80], *palavra[10], username[30];
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
    msg2sv.pid = getpid();
    /* ABRIR "CP" DO SERVIDOR (open - O_WRONLY) */
    fd_servidor = open("CPservidor", O_WRONLY);

    //registar cliente no servidor
    if(argc == 2){// nome de utilizador inserido como argumento
        char uname[32];
        i=0;
        do{
            strcpy(uname,argv[1]);
            if(i==0){//registo username sem numeros
                strcpy(msg2sv.cmd,"register");
                strcpy(msg2sv.opts,uname);
                //send message
                write(fd_servidor, &msg2sv, sizeof(msg2sv));
                //read reply
                fd_cliente = open(msg2sv.fifostr,O_RDONLY);
                read(fd_cliente,&svResp,sizeof(svResp));
                close(fd_cliente);
                //process reply
                if(svResp.code==1){
                    printf("[CLIENT] Cliente registado no servidor!\n");
                    printf("[CLIENT] O Teu Username: %s\n",uname);
                }else if(svResp.code==2){
                    printf("[ERROR] Nome de Utilizador ja existente...\n");
                    printf("[CLIENT] A tentar outro username...\n");
                    i++;
                }else{
                    printf("[ERROR] Erro inesperado...\nA sair...\n");
                    /* FECHAR "CP" DO SERVIDOR (close) */
                    close(fd_servidor);
                    /* REMOVER "cp" DO CLIENTE - EU (UNLINK) */
                    unlink(msg2sv.fifostr);
                    exit(1);
                }
            }else{// username ja existente... adicionar numeros
                sprintf(uname,"%s%d", argv[1], i); //gerar username novo
                //registar
                strcpy(msg2sv.cmd,"register");
                strcpy(msg2sv.opts,uname);
                //send message
                write(fd_servidor, &msg2sv, sizeof(msg2sv));
                //read reply
                fd_cliente = open(msg2sv.fifostr,O_RDONLY);
                read(fd_cliente,&svResp,sizeof(svResp));
                close(fd_cliente);
                //process reply
                if(svResp.code==1){
                    printf("[CLIENT] Cliente registado no servidor!\n");
                    printf("[CLIENT] O Teu Username: %s\n",uname);
                }else if(svResp.code==2){
                    printf("[ERROR] Nome de Utilizador ja existente...\n");
                    printf("[CLIENT] A tentar outro username...\n");
                    i++;
                }else{
                    printf("[ERROR] Erro inesperado...\nA sair...\n");
                    /* FECHAR "CP" DO SERVIDOR (close) */
                    close(fd_servidor);
                    /* REMOVER "cp" DO CLIENTE - EU (UNLINK) */
                    unlink(msg2sv.fifostr);
                    exit(1);
                }
            }
        }while(svResp.code!=1);
    }else{// argumentos diferente de 2
        do{
            printf("Nome de Utilizador: ");
            //get username
            fgets(username,32,stdin);
            username[strlen(username)-1]='\0';
            //build message to server
            strcpy(msg2sv.cmd,"register");
            strcpy(msg2sv.opts,username);
            //send message
            write(fd_servidor, &msg2sv, sizeof(msg2sv));
            //read reply
            fd_cliente = open(msg2sv.fifostr,O_RDONLY);
            read(fd_cliente,&svResp,sizeof(svResp));
            close(fd_cliente);
            //process reply
            if(svResp.code==1){
                printf("[CLIENT] Cliente registado no servidor!\n");
            }else if(svResp.code==2){
                printf("[ERROR] Nome de Utilizador ja existente\n");
            }else{
                printf("[ERROR] Erro inesperado...\nA sair...\n");
                /* FECHAR "CP" DO SERVIDOR (close) */
                close(fd_servidor);
                /* REMOVER "cp" DO CLIENTE - EU (UNLINK) */
                unlink(msg2sv.fifostr);
                exit(1);
            }
        }while(svResp.code!=1);
    }

    do{
        //sinais
        signal(10,closeapp);
        //cmd
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
                printf("[CLIENT] Avisar servidor para me remover\n");
                strcpy(msg2sv.cmd, "leaving");
                write(fd_servidor, &msg2sv, sizeof(msg2sv));
                printf("[CLIENT] A sair!\n");
                sair = 1;
            }else if(strcmp(palavra[0],"send")==0){
                strcpy(msg2sv.cmd, "send");
                strcpy(msg2sv.opts, palavra[1]);
                /* ENVIAR PEDIDO PARA "CP" DO SERVIDOR (write) */
                i = write(fd_servidor, &msg2sv, sizeof(msg2sv));
            }else if(strcmp(palavra[0],"shutdown")==0){
                printf("[CLIENT] A Desligar servidor!\n");
                strcpy(msg2sv.cmd, "shutdown");
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
            }else if(strcmp(palavra[0],"list")==0){
                strcpy(msg2sv.cmd, "list");
                /* ENVIAR PEDIDO PARA "CP" DO SERVIDOR (write) */
                i = write(fd_servidor, &msg2sv, sizeof(msg2sv));  
            }else if(strcmp(palavra[0],"verifica")==0){
                strcpy(msg2sv.cmd, "verifica");
                strcpy(msg2sv.opts, palavra[1]);
                /* ENVIAR PEDIDO PARA "CP" DO SERVIDOR (write) */
                i = write(fd_servidor, &msg2sv, sizeof(msg2sv));
                //ler resposta do servidor
                /*fd_cliente = open(msg2sv.fifostr,O_RDONLY);
                read(fd_cliente,&svResp,sizeof(svResp)); 
                close(fd_cliente);*/
                //processar resposta
                //printf(svResp.resp);
            }
        }

    }while(sair==0);

    /* FECHAR "CP" DO SERVIDOR (close) */
    close(fd_servidor);
    /* REMOVER "cp" DO CLIENTE - EU (UNLINK) */
    unlink(msg2sv.fifostr);
}

void closeapp(int sig){
    char cp[32];
    sprintf(cp, "CPcliente_%d", getpid());
    printf("[CLIENT] Vou sair a pedido do SERVIDOR!\n");
    unlink(cp);
    exit(0);
}