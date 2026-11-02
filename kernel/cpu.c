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
int vmxon(uint64_t *vmxon_pointer) {
    uint8_t ret;

    __asm__ volatile (
        "vmxon %1\n\t"
        "setna %0"
        : "=qm"(ret)
        : "m"(*vmxon_pointer)
        : "memory", "cc"
    );

    return ret == 0;
}

vmx_status_t vmclear(uint64_t *vmcs_pointer) {
    unsigned char cf, zf;

    __asm__ volatile (
        "vmclear %2\n\t"
        "setc %0\n\t"
        "setz %1"
        : "=qm"(cf), "=qm"(zf)
        : "m"(*vmcs_pointer)
        : "memory", "cc"
    );

    if (cf)
        return VMX_FAIL_INVALID;

    if (zf)
        return VMX_FAIL_VALID;

    return VMX_SUCCESS;
}
vmx_status_t vmptrld(uint64_t *vmcs_pointer) {
    unsigned char cf, zf;

    __asm__ volatile (
        "vmptrld %2\n\t"
        "setc %0\n\t"
        "setz %1"
        : "=qm"(cf), "=qm"(zf)
        : "m"(*vmcs_pointer)
        : "memory", "cc"
    );

    if (cf)
        return VMX_FAIL_INVALID;

    if (zf)
        return VMX_FAIL_VALID;

    return VMX_SUCCESS;
}
vmx_status_t vmread(uint32_t field, uint32_t *value) {
    unsigned char cf, zf;
    uint32_t result;

    __asm__ volatile (
        "vmread %3, %2\n\t"
        "setc %0\n\t"
        "setz %1"
        : "=qm"(cf), "=qm"(zf), "=r"(result)
        : "r"(field)
        : "cc"
    );

    if (cf)
        return VMX_FAIL_INVALID;

    if (zf)
        return VMX_FAIL_VALID;

    *value = result;

    return VMX_SUCCESS;
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
uint32_t read_cr3(void) {
    uint32_t value;

    __asm__ volatile (
        "mov %%cr3, %0"
        : "=r"(value)
    );

    return value;
}
void write_cr3(uint32_t value) {
    __asm__ volatile (
        "mov %0, %%cr3"
        :
        : "r"(value)
        : "memory"
    );
}
