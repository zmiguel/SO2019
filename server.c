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
    int sair=0, mopt=-1, n;
    
    //cliente <> server comms
    sv2cl msg2cl;
    cl2sv clResp;

    //utilizadores
    clients *users = NULL;
    clients *uinit = users;
    int userCounter = 0;

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
            printf("[SERVER] nao da\n");
            sleep(1);
            continue;
        }else if(strcmp(clResp.cmd,"shutdown")==0){
            printf("[SERVER] O servidor vai desligar\n");
            desligarClientes(users);
            //TODO implementar desligar outros clientes!
            sair = 1;
        }else if(strcmp(clResp.cmd,"send")==0){
            printf("funciona!\n");
            printf("%s\n",clResp.opts);    
        }else if(strcmp(clResp.cmd,"register")==0){
            clResp.opts[strlen(clResp.opts)-1]='\0';
            if(verifyUserName(users,&userCounter,clResp.opts)==1){
                msg2cl.code=2;
                //responde
                fd_cliente = open(clResp.fifostr,O_WRONLY);
                write(fd_cliente,&msg2cl,sizeof(msg2cl));
                close(fd_cliente);
                //fim resposta
            }else{
                printf("[SERVER] Novo utilizador adicionado!\n");
                users = addUser(users,&userCounter,clResp.opts,clResp.pid,clResp.fifostr);
                msg2cl.code=1;
                //responde
                fd_cliente = open(clResp.fifostr,O_WRONLY);
                write(fd_cliente,&msg2cl,sizeof(msg2cl));
                close(fd_cliente);
                //fim resposta
            }
            
        }else if(strcmp(clResp.cmd,"ping")==0){
            sprintf(msg2cl.resp,"[SV] Olá %s (%s)\n",getUsernameFromfifo(users,clResp.fifostr), clResp.fifostr);
            printf("[SERVER] A responder ao Cliente %s",clResp.fifostr);
            //responde
            fd_cliente = open(clResp.fifostr,O_WRONLY);
            write(fd_cliente,&msg2cl,sizeof(msg2cl));
            close(fd_cliente);
            //fim da resposta
        }else if(strcmp(clResp.cmd,"list")==0){
            printf("[SERVER] A listar %d clientes...\n",userCounter);
            listusers(users);
        }else if(strcmp(clResp.cmd,"leaving")==0){
            printf("[SERVER] Cliente desligado...\n");
            users=removeUser(users,clResp.fifostr,&userCounter);
            printf("[SERVER] Cliente removido!\n");
            uinit = users;
        }

    }while(sair==0);

    printf("[SERVER] SERVIDOR DESLIGADO\n");

    /* FECHAR "CP" DO SERVIDOR - MINHA (close) */
    close(fd_servidor);
    /* REMOVER "CP" DO SERVIDOR- MINHA (UNLINK) */
    unlink("CPservidor");
    exit(0);
}

int obtem_rand(int min, int max){
    int random;
    random = min + (rand() % (max - min + 1));
    return random;
}

int verifyUserName(clients *users, int *nUsers, char *username){
    clients *uinit = users;
    while(users!=NULL){
        if(strcmp(users->nome,username)==0){
            users = uinit;
            return 1;
        }else{
            users=users->prox;
        }
    }
    users = uinit;
    return 0;
}

clients *addUser(clients *users, int *nUsers, char *username, int pid, char *fifostr){
    (*nUsers)++;
    clients *uinit = users;
    if(users == NULL){//adicionar primeiro user
        users = malloc(sizeof(clients));
        strcpy(users->nome,username);
        strcpy(users->fifostr,fifostr);
        users->pid=pid;
        users->prox = NULL;
        uinit = users;
    }else{//adicionar outros users
        while(users->prox!=NULL){
            users=users->prox;
        }
        users->prox = malloc(sizeof(clients));
        users = users->prox;
        strcpy(users->nome,username);
        strcpy(users->fifostr,fifostr);
        users->pid=pid;
        users->prox = NULL;
        users = uinit;
    }
    return users;
}

char *getUsernameFromfifo(clients *users, char *fifo){
    clients *uinit = users;
    char send[100];
    while(users!=NULL){
        if(strcmp(users->fifostr,fifo)==0){
            strcpy(send,users->nome);
            users = uinit;
            return send;
        }
        users=users->prox;
    }
    strcpy(send,"-ERROR");
    return send;
}

void listusers(clients *users){
    clients *uinit = users;
    while(users!=NULL){
        printf("Nome: %s, Fifo: %s\n",users->nome,users->fifostr);
        users=users->prox;
    }
    users = uinit;
}

void desligarClientes(clients *users){
    while(users!=NULL){
        kill(users->pid,10);
        users=users->prox;
    }
}

clients *removeUser(clients *users, char *cp, int *nUsers){
    (*nUsers)--;
    clients *uinit = users;
    clients *prev;
    //verificar se o user que saiu foi o 1. na lista
    if(strcmp(users->fifostr,cp)==0){
        //é o primeiro do lista
        uinit=users->prox;
        free(users);
    }else{
        //nao é o primeiro da lista
        while(users!=NULL){
            prev = users;
            users = users->prox;
            if(strcmp(users->fifostr,cp)==0){
                prev->prox = users->prox;
                break;
            }
        }
    }
    return uinit;
}