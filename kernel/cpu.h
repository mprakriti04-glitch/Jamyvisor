#ifndef CPU_H
#define CPU_H

#include <stdint.h>

void cpuid(uint32_t code, uint32_t* a, uint32_t* b, uint32_t* c, uint32_t* d);
void get_cpu_vendor(char* vendor);
int cpu_check_vmx(uint32_t* ecx_out);
uint32_t read_cr0();
uint32_t read_cr4();
uint32_t read_cr3(void);
void write_cr3(uint32_t value);
uint64_t rdmsr(uint32_t msr);
void write_cr4(uint32_t value);
int vmxon(uint64_t *vmxon_pointer);
void write_cr0(uint32_t value);
void vmxoff();
#endif
