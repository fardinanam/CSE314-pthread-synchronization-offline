#include<stdio.h>
#include<pthread.h>

volatile int counter = 0;

void *inc_counter() {
    counter += 1000;
    pthread_exit(0);
}

int main(int argc, char *argv) {
    pthread_t tid[10];
    for (int i = 0; i < 10; i++) {
        int error = pthread_create(&(tid[i]), NULL, inc_counter, NULL);
        if (error != 0)
            printf("\nThread can't be created");
    }

    for (int i = 0; i < 10; i++) {
        pthread_join(tid[i], NULL);
    }

    printf("%d\n", counter);
    return 0;
}