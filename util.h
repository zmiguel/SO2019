#define TAM_TITULO 128
#define TAM_CORPO 1001
#define TAM_NOME 100

typedef struct msgdef
{
    int id;
    char titulo[TAM_TITULO];
    char corpo[TAM_CORPO];
    int duracao;
    int time_added;

    char addrss[20];

    struct topicdef * topico;
    struct msgdef * prox;
}msg;

typedef struct topicdef
{
    int id;
    char nome[TAM_NOME];
    struct msgdef * mensg;
    struct topicdef * prox;
}topic;
