#define TAM_TITULO 128
#define TAM_CORPO 1001
#define TAM_NOME 100
#define TAM_SUB 32 //maximo nr de topics subscritos por utilizador
//tamanhos comunicacao cl<>sv
#define TAM_CMD 64
#define TAM_OPTS 128
#define TAM_RESP 512

typedef struct st_mensagem{
    int id;
    char titulo[TAM_TITULO];
    char corpo[TAM_CORPO];
    int duracao;
    int time_added;

    struct st_topic * topico;
    struct st_mensagem * prox;
}mensagem;

typedef struct st_topic{
    int id;
    char nome[TAM_NOME];
    struct st_mensagem * mensg;
    struct st_topic * prox;
}topic;


//guardar clientes
typedef struct st_clients{
    int pid;
    char nome[TAM_NOME];
    char fifostr[TAM_NOME];
    int topics[TAM_SUB];

    struct st_clients *prox;
}clients;

//estructura dados de cliente --> servidor
struct clmsg{
    char titulo[TAM_TITULO];
    char corpo[TAM_CORPO];
    int duracao;
};
typedef struct st_cl2sv{
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
typedef struct st_sv2cl{
    int code;
    char type[TAM_CMD];
    char resp[TAM_RESP];
}sv2cl;
