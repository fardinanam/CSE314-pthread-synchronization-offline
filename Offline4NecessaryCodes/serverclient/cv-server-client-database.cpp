#include <bits/stdc++.h>
#include <pthread.h>
// #include <unistd.h>
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

int databaserequest[MAX_SIZE];
request serverbuffer[MAX_SIZE];

pthread_mutex_t database_locks[MAX_SIZE];
pthread_mutex_t server_buffer_lock[MAX_SIZE];
pthread_mutex_t server_client_cond_lock;
pthread_mutex_t buffer_size_lock;

pthread_cond_t  server_cond;
pthread_cond_t  client_cond;


int serverbuffersize;
int databasecount;

int serveIndex, servedIndex, numFull;
int nextServeIndex;
int findtoserve(){
    for(int i=0; i<serverbuffersize; i++){
        pthread_mutex_lock(&server_buffer_lock[i]);
        if(serverbuffer[i].currentState == WAITING_FOR_SERVER){
            pthread_mutex_unlock(&server_buffer_lock[i]);
            return i;
        }
        pthread_mutex_unlock(&server_buffer_lock[i]);
    }
    // //printf("terrible problem!");
    return NULL;
}

int findtobeserved(){
    for(int i=0; i<serverbuffersize; i++){
        pthread_mutex_lock(&server_buffer_lock[i]);
        if(serverbuffer[i].currentState == INITIATED || serverbuffer[i].currentState == SERVED){
            serverbuffer[i].currentState = WAITING_FOR_SERVER;
            pthread_mutex_unlock(&server_buffer_lock[i]);
            return i;
        }
        pthread_mutex_unlock(&server_buffer_lock[i]);
    }
    // //printf("terrible problem!");
    return NULL;
}

void initlocks(){
    if(serverbuffersize > MAX_SIZE || databasecount > MAX_SIZE){
        //printf("MAX_SIZE of serverbuffersize & databasecount is set to 20\n");
        pthread_exit(0);;
    }
    pthread_cond_init(&server_cond, NULL);
    pthread_cond_init(&client_cond, NULL);
    pthread_mutex_init(&server_client_cond_lock, NULL);
    pthread_mutex_init(&buffer_size_lock, NULL);
    for(int i=0; i<databasecount; i++){
        pthread_mutex_init(&database_locks[i], NULL);
    }
    for(int i=0; i<serverbuffersize; i++){
        pthread_mutex_init(&server_buffer_lock[i], NULL);
    }
}


void *writeToDatabase(void* arg){
    int databaseid = *((int *) arg);
    //printf("%dth database started!\n", databaseid);
    while(1){
        int clientId;
        pthread_mutex_lock(&database_locks[databaseid]);
        clientId = databaserequest[databaseid];
        // pthread_mutex_unlock(&database_locks[databaseid]);

        pthread_mutex_lock(&server_buffer_lock[clientId]);
        if(serverbuffer[clientId].currentState != WAITING_FOR_DATABASE){
            pthread_mutex_unlock(&server_buffer_lock[clientId]);
            pthread_mutex_unlock(&database_locks[databaseid]);
            continue;
        }
        // //printf("ekhane?\n");
        usleep(serverbuffer[clientId].operationtime*1000);
        serverbuffer[clientId].currentState = SERVED;
        serverbuffer[clientId].print_string();
        serverbuffer[clientId].currentState = INITIATED;

        fflush(stdout);
        pthread_mutex_unlock(&server_buffer_lock[clientId]);
        pthread_mutex_unlock(&database_locks[databaseid]);
        // pthread_mutex_lock(&server_client_cond_lock);
        pthread_mutex_lock(&buffer_size_lock);
        numFull--;
        pthread_mutex_unlock(&buffer_size_lock);
        pthread_cond_signal(&client_cond);
        // //printf("\n\njiboneo ekhane ashe....\n");
        // pthread_mutex_unlock(&server_client_cond_lock);
    }
}

void *serveClient(void *arg){
    // //printf("server started!\n");
    while(true){
        pthread_mutex_lock(&server_client_cond_lock);
        int current_buffer_size;
        pthread_mutex_lock(&buffer_size_lock);
        current_buffer_size = numFull;
        pthread_mutex_unlock(&buffer_size_lock);
        // //printf("while check er age...\n");
        // //printf("buffer size: %d\n", current_buffer_size);
        // //printf("num full: %d\n", numFull);
        while(current_buffer_size <= 0){
            // //printf("\t\twating a atkay\n");
            pthread_cond_wait(&server_cond, &server_client_cond_lock);
            pthread_mutex_lock(&buffer_size_lock);
            current_buffer_size = numFull;
            pthread_mutex_unlock(&buffer_size_lock);
            // printf("eikhane koybar ashe\n");
        }
        // printf("CURRENT BUFFER SIZE: %d\n", current_buffer_size);
        serveIndex = findtoserve();

        // //printf("Will try to serve: client %d\n", serverbuffer[serveIndex].clientid);
        if(pthread_mutex_trylock(&database_locks[serverbuffer[serveIndex].database]) == 0){
            pthread_mutex_lock(&server_buffer_lock[serveIndex]);
            serverbuffer[serveIndex].currentState = WAITING_FOR_DATABASE;
            serverbuffer[serveIndex].print_string();
            pthread_mutex_unlock(&server_buffer_lock[serveIndex]);
            databaserequest[serverbuffer[serveIndex].database] = serveIndex; //dekha lagbe
            // databaserequest[serverbuffer[serveIndex].database] = serverbuffer[serveIndex].clientid; //dekha lagbe
            pthread_mutex_unlock(&database_locks[serverbuffer[serveIndex].database]);
        }else {}
        pthread_mutex_unlock(&server_client_cond_lock); 
    }
}

void *fillupform(void *arg){
    int clientid = *((int *) arg);
    int database = rand() % databasecount;
    int operationTime = rand() % 5 + 1;
    request rs = {clientid, database, operationTime, servedIndex, INITIATED};
    int currentBufferSize;
    pthread_mutex_lock(&server_client_cond_lock);
    pthread_mutex_lock(&buffer_size_lock);
    currentBufferSize = numFull;
    pthread_mutex_unlock(&buffer_size_lock);
    while(currentBufferSize == serverbuffersize){
        rs.print_string();
        pthread_cond_wait(&client_cond, &server_client_cond_lock);
        pthread_mutex_lock(&buffer_size_lock);
        currentBufferSize = numFull;
        pthread_mutex_unlock(&buffer_size_lock);
    }
    servedIndex = findtobeserved();
    
    pthread_mutex_lock(&server_buffer_lock[servedIndex]);
    rs.serverbufferindex = servedIndex;
    rs.currentState = WAITING_FOR_SERVER;
    rs.print_string();
    serverbuffer[servedIndex] = rs;
    pthread_mutex_unlock(&server_buffer_lock[servedIndex]);
    pthread_mutex_lock(&buffer_size_lock);
    numFull++;
    pthread_mutex_unlock(&buffer_size_lock);
    pthread_cond_signal(&server_cond);
    // pthread_cond_broadcast(&server_cond);
    // //printf("ei ekhane");
    pthread_mutex_unlock(&server_client_cond_lock);
    return NULL;
    // pthread_exit(0);
}


int main(int argc, char* argv[]){
    srand(time(0));
    serverbuffersize = 3;
    databasecount = 5;
    int noOfClients = 10;
    if(argc >= 2) serverbuffersize = stoi(argv[1]);
    if(argc >= 3) databasecount = stoi(argv[2]);
    if(argc >= 4) noOfClients = stoi(argv[3]);
    initlocks();
    pthread_t databaseThreads[databasecount], serverThread, clientThreads[noOfClients];
    int databaseThreadsID[databasecount], clientThreadsId[noOfClients];
    for(int i=0; i<serverbuffersize; i++){
        serverbuffer[i].currentState = INITIATED;
    }
    for(int i=0; i<databasecount; i++){
        databaseThreadsID[i] = i;
        pthread_create(&databaseThreads[i], NULL, writeToDatabase, (void*) &databaseThreadsID[i]);
        
    }
    pthread_create(&serverThread, NULL, serveClient, NULL);
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