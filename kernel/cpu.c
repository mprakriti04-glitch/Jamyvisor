#include "cpu.h"

void get_cpu_vendor(char* vendor) {
    unsigned int ebx, ecx, edx;

    __asm__ volatile (
        "cpuid"
        : "=b"(ebx), "=c"(ecx), "=d"(edx)
        : "a"(0)
    );

    ((unsigned int*)vendor)[0] = ebx;
    ((unsigned int*)vendor)[1] = edx;
    ((unsigned int*)vendor)[2] = ecx;

    vendor[12] = '\0';
}

int cpu_has_vmx() {
    unsigned int ecx;

    __asm__ volatile (
        "cpuid"
        : "=c"(ecx)
        : "a"(1)
        : "ebx", "edx"
    );

    return (ecx >> 5) & 1;
}

