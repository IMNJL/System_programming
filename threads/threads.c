#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>

typedef struct {
    pthread_mutex_t mutex;
    FILE* log_file;
} Logger;

void log_message(Logger* logger, const char* message) {
    time_t now;
    time(&now);
    char time_str[20];
    strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", localtime(&now));

    pthread_mutex_lock(&logger->mutex);
    fprintf(logger->log_file, "[%s] %s\n", time_str, message);
    fflush(logger->log_file);
    pthread_mutex_unlock(&logger->mutex);
}

// Структура для передачи данных в поток
typedef struct {
    int arg;                // Аргумент функции
    int thread_id;          // Идентификатор потока
    Logger* logger;         // Логгер
    double* results;        // Массив для результатов
    int result_index;       // Индекс для сохранения результата
} ThreadData;

// Пример вычисляемой функции (x^2 + 2x + 1)
double compute_function(int x) {
    return x * x + 2 * x + 1;
}

// Функция потока
void* thread_function(void* arg) {
    ThreadData* data = (ThreadData*)arg;
    char log_msg[100];

    sprintf(log_msg, "Thread %d started processing argument %d",
            data->thread_id, data->arg);
    log_message(data->logger, log_msg);

    // Имитация работы с использованием thread_yield
    for (int i = 0; i < 3; i++) {
        sprintf(log_msg, "Thread %d working on argument %d (step %d)",
                data->thread_id, data->arg, i+1);
        log_message(data->logger, log_msg);

        // Вызываем thread_yield (в Linux это sched_yield)
        sched_yield();
        usleep(100000); // Небольшая задержка для наглядности
    }

    // Вычисление функции
    double result = compute_function(data->arg);
    data->results[data->result_index] = result;

    sprintf(log_msg, "Thread %d finished. Result for %d: %.2f",
            data->thread_id, data->arg, result);
    log_message(data->logger, log_msg);

    pthread_exit(NULL);
}

int main() {
    // Инициализация логгера
    Logger logger;
    pthread_mutex_init(&logger.mutex, NULL);
    logger.log_file = fopen("/Users/pro/CLionProjects/System_programming/threads/thread_log.txt", "w");
    if (!logger.log_file) {
        perror("Failed to open log file");
        return 1;
    }

    log_message(&logger, "Program started");

    // Список аргументов для функции
    int args[] = {1, 2, 3, 4, 5};
    int num_args = sizeof(args) / sizeof(args[0]);
    double results[num_args];

    // Создаем массив потоков и их данных
    pthread_t threads[num_args];
    ThreadData thread_data[num_args];

    log_message(&logger, "Creating threads...");

    // Создаем потоки
    for (int i = 0; i < num_args; i++) {
        thread_data[i].arg = args[i];
        thread_data[i].thread_id = i + 1;
        thread_data[i].logger = &logger;
        thread_data[i].results = results;
        thread_data[i].result_index = i;

        char log_msg[100];
        sprintf(log_msg, "Creating thread %d for argument %d",
                i+1, args[i]);
        log_message(&logger, log_msg);

        int rc = pthread_create(&threads[i], NULL, thread_function, &thread_data[i]);
        if (rc) {
            sprintf(log_msg, "Error creating thread %d: %d", i+1, rc);
            log_message(&logger, log_msg);
            exit(1);
        }
    }

    log_message(&logger, "All threads created. Waiting for completion...");

    // Ожидаем завершения всех потоков
    for (int i = 0; i < num_args; i++) {
        pthread_join(threads[i], NULL);

        char log_msg[100];
        sprintf(log_msg, "Thread %d joined", i+1);
        log_message(&logger, log_msg);
    }

    // Выводим результаты
    log_message(&logger, "All threads completed. Results:");
    for (int i = 0; i < num_args; i++) {
        char log_msg[100];
        sprintf(log_msg, "f(%d) = %.2f", args[i], results[i]);
        log_message(&logger, log_msg);
    }

    // Завершение работы
    log_message(&logger, "Program finished");
    fclose(logger.log_file);
    pthread_mutex_destroy(&logger.mutex);

    printf("Program completed. See thread_log.txt for details.\n");
    return 0;
}