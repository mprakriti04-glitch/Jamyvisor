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
typedef enum {
    VMX_SUCCESS = 0,
    VMX_FAIL_INVALID = 1,
    VMX_FAIL_VALID = 2
} vmx_status_t;

vmx_status_t vmclear(uint64_t *vmcs_pointer);
vmx_status_t vmptrld(uint64_t *vmcs_pointer);
vmx_status_t vmread(uint32_t field, uint32_t *value);
#endif
