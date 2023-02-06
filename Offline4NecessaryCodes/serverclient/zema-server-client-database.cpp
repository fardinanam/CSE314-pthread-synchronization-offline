#include <bits/stdc++.h>
#include "zemaphore.h"
using namespace std;

#define MAX_SIZE 200
#define DEBUG //printf("DEBUG at line: %d of %s, threadID: %lu\n", __LINE__, __func__, pthread_self()%100)

enum state{INITIATED, WAITING_FOR_SERVER, WAITING_FOR_DATABASE, SERVED};
static const char* const states[] = {
    [INITIATED] = "INITIATED",
    [WAITING_FOR_SERVER] = "WAITING_FOR_SERVER",
    [WAITING_FOR_DATABASE] = "WAITING_FOR_DATABASE",
    [SERVED] = "SERVED"
};

struct request_{
    int clientid;
    int database;
    int operationtime;
    int serverbufferindex;
    enum state currentState;
    void print_string(){
        if(currentState == SERVED) printf("client: {id: %d, db: %d, ot: %d, ix: %d, st: %s}\n", clientid, database, operationtime, serverbufferindex, states[currentState]);
        return;
    }
};
typedef struct request_ request;

zem_t databas_locks[MAX_SIZE];
zem_t buffer_locks[MAX_SIZE];
zem_t client_server_buffer;
// zem_t empty_buffer;
// zem_t full_buffer;
request serverbuffer[MAX_SIZE];
int databaserequest[MAX_SIZE];

int serverbuffersize, databasecount, clientcount;

void initzemas(){
    for(int i=0; i<databasecount; i++){
        zem_init(&databas_locks[i], 1);
    }
    for(int i=0; i<serverbuffersize; i++){
        zem_init(&buffer_locks[i], 1);
    }
    zem_init(&client_server_buffer, 1);
}

void *writetodatabase(void *arg){
    int databaseid = *((int *) arg);
    while(1){
        int clientId;
        zem_down(&databas_locks[databaseid]);
        clientId = databaserequest[databaseid];
        zem_down(&buffer_locks[clientId]);
        if(serverbuffer[clientId].currentState != WAITING_FOR_DATABASE){
            zem_up(&buffer_locks[clientId]);
            zem_up(&databas_locks[databaseid]);
            continue;
        }
        usleep(serverbuffer[clientId].operationtime*1000);
        serverbuffer[clientId].currentState = SERVED;
        serverbuffer[clientId].print_string();
        serverbuffer[clientId].currentState = INITIATED;
        fflush(stdout);
        zem_up(&buffer_locks[clientId]);
        zem_up(&databas_locks[databaseid]);
        int index = zem_value(&client_server_buffer);
        // printf("DATA_INDEX: %d\n", index);
        if(index < serverbuffersize){
            // printf("call hocche\n");
            zem_up(&client_server_buffer);
            // printf("VALUE: %d\n", zem_value(&client_server_buffer));
        }

    }
}

void *serveclient(void * arg){
    while(true){
        int index = zem_value(&client_server_buffer);
        // printf("SERVER_INDEX: %d\n", index);
        zem_down(&buffer_locks[index]);
        if(serverbuffer[index].currentState != WAITING_FOR_SERVER){
            zem_up(&buffer_locks[index]);
            continue;
        }
        serverbuffer[index].currentState = WAITING_FOR_DATABASE;
        serverbuffer[index].print_string();
        zem_down(&databas_locks[serverbuffer[index].database]);
        databaserequest[serverbuffer[index].database] = index;
        zem_up(&databas_locks[serverbuffer[index].database]);
    }
}

void *fillupform(void * arg){
    int clientid = *((int *) arg);
    int database = rand() % databasecount;
    int operationTime = rand() % 5 + 1;
    request rs = {clientid, database, operationTime, -1, INITIATED};
    zem_down(&client_server_buffer);
    int index = zem_value(&client_server_buffer);
    // printf("CLI_INDEX: %d\n", index);
    rs.serverbufferindex = index;
    zem_down(&buffer_locks[index]);
    serverbuffer[index] = rs;
    serverbuffer[index].currentState = WAITING_FOR_DATABASE;
    serverbuffer[index].print_string();
    zem_up(&buffer_locks[index]);
    return NULL;
}

int main(int argc, char * argv[]){
    srand(time(0));
    serverbuffersize = 3;
    databasecount = 5;
    int noOfClients = 10;
    if(argc >= 2) serverbuffersize = stoi(argv[1]);
    if(argc >= 3) databasecount = stoi(argv[2]);
    if(argc >= 4) noOfClients = stoi(argv[3]);
    initzemas();
    pthread_t databaseThreads[databasecount], serverThread, clientThreads[noOfClients];
    int databaseThreadsID[databasecount], clientThreadsId[noOfClients];
    for(int i=0; i<serverbuffersize; i++){
        serverbuffer[i].currentState = INITIATED;
    }
    for(int i=0; i<databasecount; i++){
        databaseThreadsID[i] = i;
        pthread_create(&databaseThreads[i], NULL, writetodatabase, (void*) &databaseThreadsID[i]);
        
    }
    pthread_create(&serverThread, NULL, serveclient, NULL);
    for(int i=0; i<noOfClients; i++){
        clientThreadsId[i] = i;
        pthread_create(&clientThreads[i], NULL, fillupform, (void*) &clientThreadsId[i]);
    }
    for(int i=0; i<noOfClients; i++) pthread_join(clientThreads[i], NULL);
    for(int i=0; i<databasecount; i++) pthread_join(databaseThreads[i], NULL);
    pthread_join(serverThread, NULL);
    // //printf("done?\n");
    return 0;

}