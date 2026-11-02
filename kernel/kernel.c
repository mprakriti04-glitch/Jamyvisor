#include "screen.h"
#include "cpu.h"
#include <stdint.h>
__attribute__((aligned(4096)))
uint8_t vmxon_region[4096];
// VMCS region
__attribute__((aligned(4096)))
uint8_t vmcs_region[4096];
// Page-aligned paging structures
__attribute__((aligned(4096)))
uint32_t page_directory[1024];

__attribute__((aligned(4096)))
uint32_t page_table[1024];
void setup_paging(void) {

    // Clear page directory
    for (int i = 0; i < 1024; i++) {
        page_directory[i] = 0;
    }

    // Identity-map first 4 MB
    for (int i = 0; i < 1024; i++) {
        page_table[i] = (i * 4096) | 3;
    }

    // Connect page directory to page table
    page_directory[0] = ((uint32_t)page_table) | 3;
}
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
uint32_t cr3 = read_cr3();

print("[CPU] CR3 = ");
print_hex(cr3);
print("\n");

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
print_hex64(cr4_fixed0);
print("\n");

print("[VMX] CR4_FIXED1 = ");
print_hex64(cr4_fixed1);
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
print_hex64(cr0_fixed0);
print("\n");

print("[VMX] CR0_FIXED1 = ");
print_hex64(cr0_fixed1);
print("\n");
print("Decimal: ");
print_int(12345);
print("\n");

print("Hex: ");
print_hex(0x1A2B3C);
print("\n");
print("\n[VMX] VMXON region allocated\n");

print("[VMX] VMXON addr = ");
print_hex((uint32_t)vmxon_region);
print("\n");
// =========================
// Day 13 - VMX Revision ID
// =========================

uint64_t vmx_basic = rdmsr(0x480);
uint32_t vmcs_size = (uint32_t)((vmx_basic >> 32) & 0x1FFF);
uint32_t memory_type = (uint32_t)((vmx_basic >> 50) & 0xF);
uint32_t physical_width = (uint32_t)((vmx_basic >> 48) & 1);

print("[VMCS] Region size = ");
print_int(vmcs_size);
print(" bytes\n");

print("[VMCS] Memory type = ");
print_int(memory_type);
print("\n");

print("[VMCS] Physical address width flag = ");
print_int(physical_width);
print("\n");
// Lower 31 bits = revision ID
uint32_t vmx_revision_id = (uint32_t)(vmx_basic & 0x7FFFFFFF);

print("[VMX] Revision ID = ");
print_hex(vmx_revision_id);
print("\n");

// Write revision ID into VMXON region
*(uint32_t*)vmxon_region = vmx_revision_id;

print("[VMX] Revision written to VMXON region\n");
// Write revision ID into VMCS region
*(uint32_t*)vmcs_region = vmx_revision_id;

print("[VMCS] Revision written to VMCS region\n");
// =========================
// Day 14 - Execute VMXON
// =========================

uint64_t vmxon_pointer = (uint64_t)(uint32_t)vmxon_region;

// =========================
// Configure paging
// =========================

setup_paging();

print("[PAGING] Page tables initialized\n");

// Load page directory address into CR3
write_cr3((uint32_t)page_directory);

print("[PAGING] CR3 loaded = ");
print_hex(read_cr3());
print("\n");

// =========================
// Apply VMX-required CR0/CR4 masks
// =========================

cr0 |= (uint32_t)(cr0_fixed0 & 0xFFFFFFFF);
cr0 &= (uint32_t)(cr0_fixed1 & 0xFFFFFFFF);

cr4 |= (uint32_t)(cr4_fixed0 & 0xFFFFFFFF);
cr4 &= (uint32_t)(cr4_fixed1 & 0xFFFFFFFF);

print("[DEBUG] CR0 value to write = ");
print_hex(cr0);
print("\n");

print("[DEBUG] CR4 value to write = ");
print_hex(cr4);
print("\n");

// Configure CR4 first
write_cr4(cr4);

print("[VMX] CR4 configured\n");

// Enable paging through CR0
write_cr0(cr0);

print("[PAGING] CR0 configured\n");
print("[VMX] About to execute VMXON...\n");
int vmxon_success = vmxon(&vmxon_pointer);
if (vmxon_success) {

    print("[VMX] Entered VMX Root Mode\n");

    // Prepare VMCS physical address
    uint64_t vmcs_pointer =
        (uint64_t)(uint32_t)vmcs_region;

    print("[VMCS] Region address = ");
    print_hex((uint32_t)vmcs_pointer);
    print("\n");

    // Execute VMCLEAR
// Execute VMCLEAR
print("[VMCS] Executing VMCLEAR...\n");

vmx_status_t clear_status = vmclear(&vmcs_pointer);

if (clear_status == VMX_SUCCESS) {

    print("[VMCS] VMCLEAR SUCCESS\n");

    // Execute VMPTRLD
    print("[VMCS] Executing VMPTRLD...\n");

    vmx_status_t load_status = vmptrld(&vmcs_pointer);

    if (load_status == VMX_SUCCESS) {

        print("[VMCS] VMPTRLD SUCCESS\n");

        // Execute VMREAD
        uint32_t vmcs_value = 0;

        print("[VMCS] Executing VMREAD...\n");

        vmx_status_t read_status = vmread(0x4400, &vmcs_value);

        if (read_status == VMX_SUCCESS) {

            print("[VMCS] VMREAD SUCCESS\n");
            print("[VMCS] VM-instruction error = ");
            print_hex(vmcs_value);
            print("\n");

        }
        else if (read_status == VMX_FAIL_INVALID) {
            print("[VMCS] VMREAD VMfailInvalid\n");
        }
        else {
            print("[VMCS] VMREAD VMfailValid\n");
        }

    }
    else if (load_status == VMX_FAIL_INVALID) {
        print("[VMCS] VMPTRLD VMfailInvalid\n");
    }
    else {
        print("[VMCS] VMPTRLD VMfailValid\n");
    }

}
else if (clear_status == VMX_FAIL_INVALID) {
    print("[VMCS] VMCLEAR VMfailInvalid\n");
}
else {
    print("[VMCS] VMCLEAR VMfailValid\n");
}

} else {

    print("[VMX] VMXON FAILED\n");

}

while (1) {
    __asm__ volatile ("hlt");
}
}
