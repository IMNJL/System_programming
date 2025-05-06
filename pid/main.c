#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <stdbool.h>

int create_process_H() {
    pid_t pid0 = fork();
    if(pid0 == 0) {
        printf("Процесс H (PID=%d, PPID=%d)\n", getpid(), getppid());
        pid_t pid1 = fork();
        if (pid1 == 0) {
            printf("Процесс 1 (PID=%d, PPID=%d)\n", getpid(), getppid());

            pid_t pid2 = fork();
            if (pid2 == 0) {
                printf("Процесс 2 (PID=%d, PPID=%d)\n", getpid(), getppid());

                pid_t pid6 = fork();
                if (pid6 == 0) {
                    printf("Процесс 6 (PID=%d, PPID=%d)\n", getpid(), getppid());
                    while(1) sleep(1);
                }
                while(1) sleep(1);
            }

            pid_t pid3 = fork();
            if (pid3 == 0) {
                printf("Процесс 3 (PID=%d, PPID=%d)\n", getpid(), getppid());

                pid_t pid4 = fork();
                if (pid4 == 0) {
                    printf("Процесс 4 (PID=%d, PPID=%d)\n", getpid(), getppid());
                    while(1) sleep(1);
                }

                pid_t pid5 = fork();
                if (pid5 == 0) {
                    printf("Процесс 5 (PID=%d, PPID=%d)\n", getpid(), getppid());
                    while(1) sleep(1);
                }

                while(1) sleep(1);
            }

            while(1) sleep(1);
        }
        while(1) sleep(1);
    }
    return pid0;
}

void sigint_handler(int signo) {
    printf("\nПолучен SIGINT. Завершение работы...\n");
    exit(EXIT_SUCCESS);
}

int main() {
    printf("Создаем дерево процессов...\n");
    int rootpid = create_process_H();
    char pid_str[16];
    snprintf(pid_str, sizeof(pid_str), "%d", rootpid);
    while(1) {



        // execlp("pstree", "pstree", "-p", pid_str, NULL);

        sleep(1);
        signal(SIGINT, sigint_handler);
    }
}