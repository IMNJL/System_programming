#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <string.h>
#include <sys/wait.h>

#define FILENAME "/Users/pro/CLionProjects/System_programming/signals/f1.txt"

pid_t child_pid = 0;

// Обработчик сигнала SIGUSR1
void sig_usr1_handler(int sig) {
    printf("[DEBUG] [Родитель] Получен сигнал SIGUSR1: запись завершена.\n");
}

// Обработчик сигнала SIGCHLD
void sig_chld_handler(int sig) {
    int status;
    pid_t pid;

    while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {
        printf("[DEBUG] [Родитель] Дочерний процесс %d завершился.\n", pid);
        if (WIFEXITED(status)) {
            printf("[DEBUG] Завершился с кодом: %d\n", WEXITSTATUS(status));
            exit(EXIT_SUCCESS);
        }
    }
}

int main() {
    int fd;
    pid_t pid;
    char c;

    // Открытие файла
    printf("[DEBUG] [Родитель] Открытие файла '%s'...\n", FILENAME);
    fd = open(FILENAME, O_RDWR | O_CREAT | O_TRUNC, 0644);
    if (fd == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }

    // --- Установка обработчика SIGUSR1 ---
    struct sigaction act_usr1;
    {
        sigset_t set;

        // Очистка структуры
        memset(&act_usr1, 0, sizeof(act_usr1));

        // Указываем обработчик
        act_usr1.sa_handler = sig_usr1_handler;

        // Блокируем другие сигналы во время обработки
        sigemptyset(&set);
        sigaddset(&set, SIGUSR1);     // Можно добавить больше, если нужно
        act_usr1.sa_mask = set;

        // Флаги
        act_usr1.sa_flags = SA_RESTART; // Перезапуск прерванных системных вызовов

        // Привязываем к сигналу
        if (sigaction(SIGUSR1, &act_usr1, NULL) == -1) {
            perror("[ERROR] sigaction (SIGUSR1)");
            close(fd);
            exit(EXIT_FAILURE);
        }
    }

    // --- Установка обработчика SIGCHLD ---
    struct sigaction act_chld;
    {
        sigset_t set;

        memset(&act_chld, 0, sizeof(act_chld));
        act_chld.sa_handler = sig_chld_handler;

        sigemptyset(&set);
        sigaddset(&set, SIGCHLD);
        act_chld.sa_mask = set;

        act_chld.sa_flags = SA_RESTART | SA_NOCLDSTOP;

        if (sigaction(SIGCHLD, &act_chld, NULL) == -1) {
            perror("[ERROR] sigaction (SIGCHLD)");
            close(fd);
            exit(EXIT_FAILURE);
        }
    }

    // Создание дочернего процесса
    printf("[DEBUG] [Родитель] Создание дочернего процесса...\n");
    pid = fork();
    if (pid == -1) {
        perror("fork");
        close(fd);
        exit(EXIT_FAILURE);
    }

    if (pid == 0) { // Процесс 2 (потомок)
        const char *data = "Hello from chld proc";

        // Повторно открываем файл, чтобы иметь свой указатель
        int child_fd = open(FILENAME, O_WRONLY);
        if (child_fd == -1) {
            perror("open (потомок)");
            exit(EXIT_FAILURE);
        }

        printf("[DEBUG] [Потомок] Начало записи в файл...\n");

        for (int i = 0; data[i] != '\0'; i++) {
            write(child_fd, &data[i], 1);
            printf("[DEBUG] [Потомок] Записан байт #%d: '%c'\n", i + 1, data[i]);
            usleep(100000); // Эмуляция задержки
        }

        // Уведомляем родителя о завершении
        kill(getppid(), SIGUSR1);

        close(child_fd);
        exit(EXIT_SUCCESS);
    } else { // Процесс 1 (родитель)
        child_pid = pid;

        // Закрываем и заново открываем файл, чтобы иметь независимый указатель
        close(fd);
        fd = open(FILENAME, O_RDONLY);
        if (fd == -1) {
            perror("open (родитель)");
            exit(EXIT_FAILURE);
        }

        printf("[DEBUG] [Родитель] Начало чтения из файла...\n");

        int bytes_read_total = 0;
        while (1) {
            ssize_t bytes_read = read(fd, &c, 1);
            if (bytes_read == 0) {
                // Проверяем, жив ли потомок
                int status;
                if (waitpid(child_pid, &status, WNOHANG) > 0) {
                    printf("[DEBUG] [Родитель] Дочерний процесс завершился, EOF достигнут.\n");
                    break;
                } else {
                    // Ждём немного, возможно, потомок ещё пишет
                    usleep(50000);
                    continue;
                }
            } else if (bytes_read == -1) {
                perror("[ERROR] read");
                break;
            } else {
                bytes_read_total++;
                printf("[DEBUG] [Родитель] Прочитан байт #%d: '%c'\n", bytes_read_total, c);
                write(STDOUT_FILENO, &c, 1);
                fflush(stdout);
            }
        }

        printf("[DEBUG] [Родитель] Чтение завершено. Ожидание завершения потомка...\n");
        wait(NULL);

        close(fd);
        printf("[DEBUG] [Родитель] Работа завершена. Файл '%s' остаётся на диске.\n", FILENAME);
    }

    return 0;
}