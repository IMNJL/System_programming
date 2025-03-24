#include <stdio.h>
#include <locale.h>

int main(void) {
    setlocale(LC_ALL, "");
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

    int ax;
    __asm__(
            "movl $1,%%eax\n"
            "cpuid\n"
            "movl %%eax,%0\n"
            :"=a"(ax)
            );
    printf("\nEAX == 1\n");
    for(int i=31; i>0; --i)
    {
        if(i==28) printf("\nstepping: ");
        if(i==24) printf("\nModel number: ");
        if(i==20) printf("\nfamily: ");
        if(i==18) printf("\nprocessor type: ");
        if(i==16) printf("\nreserve: ");
        if(i==12) printf("\nextended model: ");
        if(i==4) printf("\nextended family: ");
        if(i==0) printf("\nreserve: ");
        printf("%d", ((ax&0x10000000)?1:0));
        ax<<=1;
    }

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