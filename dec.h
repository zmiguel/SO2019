void printMenu(void);
int obtem_rand(int min, int max);
int verifyUserName(clients *users, int *nUsers, char *username);
clients *addUser(clients *users, int *nUsers, char *username, int pid, char *fifostr);
char *getUsernameFromfifo(clients *users, char *fifo);
void listusers(clients *users);
void closeapp(int sig);
void desligarClientes(clients *users);