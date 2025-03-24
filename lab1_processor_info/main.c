#include <stdio.h>

int main(void) {
    // Часть 1: Получение информации о производителе и базовых функциях
    int max_basic_func;
    int vendor_ebx, vendor_ecx, vendor_edx;

    // Вызов CPUID с EAX=0 для получения информации о производителе
    __asm__ volatile (
            "movl $0, %%eax\n"
            "cpuid\n"
            "movl %%eax, %0\n"
            "movl %%ebx, %1\n"
            "movl %%ecx, %2\n"
            "movl %%edx, %3\n"
            : "=r"(max_basic_func), "=r"(vendor_ebx), "=r"(vendor_ecx), "=r"(vendor_edx)
            : /* нет входных параметров */
            : "eax", "ebx", "ecx", "edx"  // Указываем, какие регистры изменяются
            );

    printf("Maximum basic function set: %d\n", max_basic_func);

    // Вывод названия производителя (12 символов)
    printf("Vendor ID: ");
    for(int i = 0; i < 4; i++) {
        printf("%c", (vendor_ebx >> (i*8)) & 0xFF);
    }
    for(int i = 0; i < 4; i++) {
        printf("%c", (vendor_edx >> (i*8)) & 0xFF);
    }
    for(int i = 0; i < 4; i++) {
        printf("%c", (vendor_ecx >> (i*8)) & 0xFF);
    }
    printf("\n");

    // Часть 2: Получение сигнатуры процессора
    int signature;
    __asm__ volatile (
            "movl $1, %%eax\n"
            "cpuid\n"
            "movl %%eax, %0\n"
            : "=r"(signature)
            : /* нет входных параметров */
            : "eax", "ebx", "ecx", "edx"
            );

    // Анализ битов сигнатуры
    printf("\nProcessor Signature Analysis (EAX=1):\n");
    printf("Stepping ID:    %d\n", (signature >> 0) & 0xF);
    printf("Model:          %d\n", (signature >> 4) & 0xF);
    printf("Family:         %d\n", (signature >> 8) & 0xF);
    printf("Processor Type: %d\n", (signature >> 12) & 0x3);
    printf("Extended Model: %d\n", (signature >> 16) & 0xF);
    printf("Extended Family:%d\n", (signature >> 20) & 0xFF);

    // Часть 3: Получение маркерной строки процессора
    printf("\nProcessor Brand String:\n");
    for(int i = 0x80000002; i <= 0x80000004; i++) {
        int part_a, part_b, part_c, part_d;

        __asm__ volatile (
                "movl %4, %%eax\n"
                "cpuid\n"
                "movl %%eax, %0\n"
                "movl %%ebx, %1\n"
                "movl %%ecx, %2\n"
                "movl %%edx, %3\n"
                : "=r"(part_a), "=r"(part_b), "=r"(part_c), "=r"(part_d)
                : "r"(i)
                : "eax", "ebx", "ecx", "edx"
                );

        // Вывод 16 символов за каждый вызов CPUID
        for(int j = 0; j < 4; j++) printf("%c", (part_a >> (j*8)) & 0xFF);
        for(int j = 0; j < 4; j++) printf("%c", (part_b >> (j*8)) & 0xFF);
        for(int j = 0; j < 4; j++) printf("%c", (part_c >> (j*8)) & 0xFF);
        for(int j = 0; j < 4; j++) printf("%c", (part_d >> (j*8)) & 0xFF);
    }
    printf("\n");

    return 0;
}