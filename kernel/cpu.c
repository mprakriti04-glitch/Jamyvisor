#include "cpu.h"
#include <stdint.h>

void cpuid(uint32_t code, uint32_t* a, uint32_t* b, uint32_t* c, uint32_t* d) {
    __asm__ volatile (
        "cpuid"
        : "=a"(*a), "=b"(*b), "=c"(*c), "=d"(*d)
        : "a"(code)
    );
}

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

int cpu_check_vmx(uint32_t* ecx_out) {
    uint32_t eax, ebx, ecx, edx;

    cpuid(1, &eax, &ebx, &ecx, &edx);

    if (ecx_out) {
        *ecx_out = ecx;
    }

    return (ecx >> 5) & 1;
}
uint32_t read_cr0() {
    uint32_t value;
    __asm__ volatile (
        "mov %%cr0, %0"
        : "=r"(value)
    );
    return value;
}

uint32_t read_cr4() {
    uint32_t value;
    __asm__ volatile (
        "mov %%cr4, %0"
        : "=r"(value)
    );
    return value;
}
uint64_t rdmsr(uint32_t msr) {
    uint32_t low, high;

    __asm__ volatile (
        "rdmsr"
        : "=a"(low), "=d"(high)
        : "c"(msr)
    );

    return ((uint64_t)high << 32) | low;
}
void write_cr4(uint32_t value) {
    __asm__ volatile (
        "mov %0, %%cr4"
        :
        : "r"(value)
    );
}
int vmxon(uint32_t vmxon_region_phys) {
    uint8_t status;

    __asm__ volatile (
        "vmxon %[addr]\n\t"
        "setna %[status]"
        : [status] "=rm"(status)
        : [addr] "m"(vmxon_region_phys)
        : "cc", "memory"
    );

    return status == 0;
}
void write_cr0(uint32_t value) {
    __asm__ volatile (
        "mov %0, %%cr0"
        :
        : "r"(value)
    );
}
void vmxoff() {
    __asm__ volatile (
        "vmxoff"
        :
        :
        : "memory"
    );
}
