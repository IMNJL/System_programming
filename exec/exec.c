#include <stdio.h>
#include <stdarg.h>

int execl(char *name, char *arg1, ...) {
    va_list args;
    va_start(args, arg1);

    // Массив для хранения слов (максимум 100)
    char *words[100];
    int count = 0;

    // Добавляем первое слово
    words[count++] = arg1;

    // Собираем остальные аргументы до NULL
    char *next_word;
    while ((next_word = va_arg(args, char *)) != NULL && count < 100) {
        words[count++] = next_word;
    }

    // Вывод в обратном порядке
    if (count > 0) {
        printf("%s", words[count - 1]);
        for (int i = count - 2; i >= 0; i--) {
            printf(" %s", words[i]);
        }
        printf("\n");
    }

    va_end(args);
    return 0;
}

int main(void) {
    // Пример вызова: выводит "three two one"
    execl("dummy", "one", "two", "three", NULL);
    return 0;
}

// найти файл и его структуру и вывести - read