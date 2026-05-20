#include "screen.h"
#include "cpu.h"
#include <stdint.h>
void kernel_main() {
    char vendor[13];

    clear_screen();

    print("NanoVMM v0.3\n\n");

    log_level(LOG_OK, "Screen ready");
log_level(LOG_INFO, "Detecting CPU...");

    get_cpu_vendor(vendor);

print("[CPU] Vendor: ");
print(vendor);
print("\n");
uint32_t ecx;

log_level(LOG_INFO, "Checking VMX support...");

int vmx = cpu_check_vmx(&ecx);

print("[CPU] ECX = ");
print_hex(ecx);
print("\n");

print("[CPU] VMX Bit (bit 5) = ");
print_int((ecx >> 5) & 1);
print("\n");

if (vmx) {
    log_level(LOG_OK, "VT-x Supported");
} else {
    log_level(LOG_ERR, "VT-x Not Supported");
}
uint32_t cr0 = read_cr0();
uint32_t cr4 = read_cr4();

print("\n[CPU] CR0 = ");
print_hex(cr0);
print("\n");

print("[CPU] CR4 = ");
print_hex(cr4);
print("\n");
cr4 |= (1 << 13);

write_cr4(cr4);

print("[VMX] CR4.VMXE enabled\n");
cr4 = read_cr4();

print("[CPU] New CR4 = ");
print_hex(cr4);
print("\n");
print("\nTesting numbers:\n");
uint64_t feature_msr = rdmsr(0x3A);

print("\n[CPU] MSR[0x3A] = ");
print_hex((uint32_t)(feature_msr >> 32));
print_hex((uint32_t)(feature_msr & 0xFFFFFFFF));
print("\n");
uint64_t cr4_fixed0 = rdmsr(0x488);
uint64_t cr4_fixed1 = rdmsr(0x489);

print("\n[VMX] CR4_FIXED0 = ");
print_hex((uint32_t)(cr4_fixed0 & 0xFFFFFFFF));
print("\n");

print("[VMX] CR4_FIXED1 = ");
print_hex((uint32_t)(cr4_fixed1 & 0xFFFFFFFF));
print("\n");
int lock_bit = feature_msr & 1;
int vmx_enable = (feature_msr >> 2) & 1;

print("[VMX] Lock: ");
if (lock_bit) {
    print("ON\n");
} else {
    print("OFF\n");
}

print("[VMX] Enabled: ");
if (vmx_enable) {
    print("YES\n");
} else {
    print("NO\n");
}
uint64_t cr0_fixed0 = rdmsr(0x486);
uint64_t cr0_fixed1 = rdmsr(0x487);

print("\n[VMX] CR0_FIXED0 = ");
print_hex((uint32_t)(cr0_fixed0 & 0xFFFFFFFF));
print("\n");

print("[VMX] CR0_FIXED1 = ");
print_hex((uint32_t)(cr0_fixed1 & 0xFFFFFFFF));
print("\n");
print("Decimal: ");
print_int(12345);
print("\n");

print("Hex: ");
print_hex(0x1A2B3C);
print("\n");
static uint8_t vmxon_region[4096] __attribute__((aligned(4096)));

print("\n[VMX] VMXON region allocated\n");

print("[VMX] VMXON addr = ");
print_hex((uint32_t)vmxon_region);
print("\n");
// =========================
// Day 13 - VMX Revision ID
// =========================

uint64_t vmx_basic = rdmsr(0x480);

// Lower 31 bits = revision ID
uint32_t vmx_revision_id = (uint32_t)(vmx_basic & 0x7FFFFFFF);

print("[VMX] Revision ID = ");
print_hex(vmx_revision_id);
print("\n");

// Write revision ID into VMXON region
*(uint32_t*)vmxon_region = vmx_revision_id;

print("[VMX] Revision written to VMXON region\n");
    while (1) { }

}
