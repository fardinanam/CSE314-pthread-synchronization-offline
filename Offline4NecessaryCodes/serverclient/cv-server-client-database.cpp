#include <bits/stdc++.h>
#include <pthread.h>
// #include <unistd.h>
using namespace std;

#define MAX_SIZE 20

enum state{NONE, WAITING_FOR_SERVER, IN_SERVER, WAITING_FOR_DATABASE, IN_DATABASE, SERVED};

struct request_{
    int clientid;
    int database;
    int operationtime;
    int serverbufferindex;
    enum state currentState;
};
typedef struct request_ request;

request databaserequest[MAX_SIZE];
request serverbuffer[MAX_SIZE];

pthread_mutex_t  database_locks[MAX_SIZE];

pthread_mutex_t server_client_cond_lock;
pthread_cond_t  server_cond;
pthread_cond_t  client_cond;

pthread_mutex_t index_lock;

int serverbuffersize;
int databasecount;

int serveIndex, servedIndex, numFull;
int nextServeIndex;
int findtoserve(){
    nextServeIndex %= serverbuffersize;
    while(serverbuffer[nextServeIndex++].currentState != WAITING_FOR_SERVER){
        return nextServeIndex;
    }
    printf("Terrible matter!");
    exit(0);
}

int findtobeserved(){
    for(int i=0; i<serverbuffersize; i++){
        if(serverbuffer[i].currentState == NONE || serverbuffer[i].currentState == SERVED){
            return i;
        }
    }
    printf("terrible problem!");
    exit(0);
}

void initlocks(){
    if(serverbuffersize > 20 || databasecount > 20){
        printf("MAX_SIZE of serverbuffersize & databasecount is set to 20\n");
        exit(0);
    }
    pthread_cond_init(&server_cond, NULL);
    pthread_cond_init(&client_cond, NULL);
    pthread_mutex_init(&server_client_cond_lock, NULL);
    pthread_mutex_init(&index_lock, NULL);
    for(int i=0; i<databasecount; i++){
        pthread_mutex_init(&database_locks[i], NULL);
    }
}


void *writeToDatabase(void* arg){
    int databaseid = *((int *) arg);
    printf("%dth database started!\n", databaseid);
    while(1){
        request rs = databaserequest[databaseid];
        if(rs.currentState != WAITING_FOR_DATABASE) continue;
        pthread_mutex_lock(&database_locks[databaseid]);
        printf("%dth database will be serving client%d for %d seconds.\n", databaseid, rs.clientid, rs.operationtime);
        sleep(rs.operationtime);
        printf("%dth database has served client%d for %d seconds.\n", databaseid, rs.clientid, rs.operationtime);
        databaserequest[databaseid].currentState = SERVED;
        pthread_cond_signal(&client_cond); // ekta client ke serve kora shesh.
        pthread_mutex_unlock(&database_locks[databaseid]);
    }
}

void *serveClient(void *arg){
    printf("server started!\n");
    while(true){
        pthread_mutex_lock(&server_client_cond_lock);
        while(numFull == 0){
            pthread_cond_wait(&server_cond, &server_client_cond_lock);
        }
        pthread_mutex_lock(&index_lock);
        serveIndex = findtoserve();
        pthread_mutex_unlock(&index_lock);
        printf("Will try to serve: client %d\n", serverbuffer[serveIndex].clientid);
        if(pthread_mutex_trylock(&database_locks[serverbuffer[serveIndex].database]) == 0){
            serverbuffer[serveIndex].currentState = WAITING_FOR_DATABASE;
            pthread_mutex_unlock(&database_locks[serverbuffer[serveIndex].database]);
        }else {}
        pthread_mutex_unlock(&server_client_cond_lock); //eita kokhon charbo?
    }
}

void *fillupform(void *arg){
    int clientid = *((int *) arg);
    printf("%dth client started!\n", clientid);
    pthread_mutex_lock(&server_client_cond_lock);
    while(numFull == serverbuffersize){
        pthread_cond_wait(&client_cond, &server_client_cond_lock);
    }
    int database = rand() % databasecount;
    int operationTime = rand() % 5 + 1;
    pthread_mutex_lock(&index_lock);
    servedIndex = findtobeserved();
    pthread_mutex_unlock(&index_lock);
    request rs = {clientid, database, operationTime, servedIndex, WAITING_FOR_SERVER};
    printf("client %d will use %dth database for %d seconds\n", clientid, database, operationTime);
    serverbuffer[servedIndex] = rs;
    pthread_cond_signal(&server_cond);
    pthread_mutex_unlock(&server_client_cond_lock);
    exit(0);
}


int main(int argc, char* argv[]){
    srand(time(0));
    serverbuffersize = 3;
    databasecount = 5;
    int noOfClients = 3;
    if(argc >= 2) serverbuffersize = stoi(argv[1]);
    if(argc >= 3) databasecount = stoi(argv[2]);
    if(argc >= 4) noOfClients = stoi(argv[3]);
    initlocks();
    pthread_t databaseThreads[databasecount], serverThread, clientThreads[noOfClients];
    int databaseThreadsID[databasecount], clientThreadsId[noOfClients];
    for(int i=0; i<serverbuffersize; i++){
        serverbuffer[i].currentState = NONE;
    }
    for(int i=0; i<databasecount; i++){
        databaseThreadsID[i] = i;
        pthread_create(&databaseThreads[i], NULL, writeToDatabase, (void*) &databaseThreadsID[i]);
        
    }
    pthread_create(&serverThread, NULL, serveClient, NULL);
    for(int i=0; i<noOfClients; i++){
        clientThreadsId[i] = i;
        printf("%d\n", i);
        pthread_create(&clientThreads[i], NULL, fillupform, (void*) &clientThreadsId[i]);
    }
    for(int i=0; i<noOfClients; i++) pthread_join(clientThreads[i], NULL);
    for(int i=0; i<databasecount; i++) pthread_join(databaseThreads[i], NULL);
    pthread_join(serverThread, NULL);
    printf("done?\n");
    return 0;
}