#define TAM_TITULO 128
#define TAM_CORPO 1001
#define TAM_NOME 100
#define TAM_SUB 32 //maximo nr de topics subscritos por utilizador
//tamanhos comunicacao cl<>sv
#define TAM_CMD 64
#define TAM_OPTS 128
#define TAM_RESP 512

typedef struct{
    int id;
    char titulo[TAM_TITULO];
    char corpo[TAM_CORPO];
    int duracao;
    int time_added;

    struct topic * topico;
    struct mensagem * prox;
}mensagem;

typedef struct{
    int id;
    char nome[TAM_NOME];
    struct mensagem * mensg;
    struct topic * prox;
}topic;
//guardar clientes
typedef struct{
    int pid;
    char nome[TAM_NOME];
    char fifostr[TAM_NOME];
    int topics[TAM_SUB];

    struct clients *prox;
}clients;

//estructura dados de cliente --> servidor
struct clmsg{
    char titulo[TAM_TITULO];
    char corpo[TAM_CORPO];
    int duracao;
};
typedef struct{
    char fifostr[TAM_NOME];
    //base
    int pid;
    char cmd[TAM_CMD];
    char opts[TAM_OPTS];
    int nopt;
    //mensagem
    struct clmsg msg;
}cl2sv;

//estructura de dados servidor --> cliente
typedef struct{
    int code;
    char type[TAM_CMD];
    char resp[TAM_RESP];
}sv2cl;
