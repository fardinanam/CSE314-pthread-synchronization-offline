#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>

void *print_i(void *i) {
    int num = atoi(i);
    printf("\n%d\n", num);
    pthread_exit(0);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "usage: print_i.out <N>");
        return -1;
    }

    int n = atoi(argv[1]);
    pthread_t *tid;
    tid = (pthread_t*)malloc(n * sizeof(pthread_t));

    for (int i = 1; i <= n; i++) {
        pthread_attr_t attr;
        pthread_attr_init(&attr);
        pthread_create(&(tid[i]), &attr, print_i, &i);
        pthread_join(tid[i], NULL);
    }
}