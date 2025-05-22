#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sched.h>
#include <unistd.h>

int matrix[8][8];
pthread_mutex_t mutex_matrix;

void *writer(void *arg) {
    FILE* file = fopen("rab.log", "a");
    if (file == NULL) {
        perror("Error opening file");
        pthread_exit(NULL);
    }
    while (1) {
        pthread_mutex_lock(&mutex_matrix);
        printf("Writer started\n");
        fprintf(file, "Writer started\n");
        for (int i = 0; i < 8; ++i)
            for (int j = 0; j < 8; ++j)
                matrix[i][j] = rand() % 100;
        fprintf(file, "Writer finished\n");
        fflush(file);
        printf("Writer finished\n");
        pthread_mutex_unlock(&mutex_matrix);
        sched_yield();
        sleep(1);
    }
    fclose(file);
    return NULL;
}

void *reader(void *arg) {
    FILE* file = fopen("rab.log", "a");
    if (file == NULL) {
        perror("Error opening file");
        pthread_exit(NULL);
    }
    while (1) {
        pthread_mutex_lock(&mutex_matrix);
        printf("Reader started\n");
        fprintf(file, "Reader started\n");
        for (int i = 0; i < 8; ++i) {
            for (int j = 0; j < 8; ++j) {
                printf("%3d ", matrix[i][j]);
                fprintf(file, "%3d ", matrix[i][j]);
            }
            printf("\n");
            fprintf(file, "\n");
        }
        fprintf(file, "Reader finished\n");
        fflush(file);
        printf("Reader finished\n");
        pthread_mutex_unlock(&mutex_matrix);
        sched_yield();
        sleep(1);
    }
    fclose(file);
    return NULL;
}

int main(void) {
    pthread_t tid1, tid2;
    pthread_mutex_init(&mutex_matrix, NULL);
    pthread_create(&tid1, NULL, writer, NULL);
    pthread_create(&tid2, NULL, reader, NULL);
    pthread_join(tid1, NULL);
    pthread_join(tid2, NULL);
    pthread_mutex_destroy(&mutex_matrix);
    return 0;
}
