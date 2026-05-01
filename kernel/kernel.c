#include "screen.h"
#include "cpu.h"

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
if (cpu_has_vmx()) {
    log_level(LOG_OK, "VT-x Supported");
} else {
    log_level(LOG_ERR, "VT-x Not Supported");
}
print("\nTesting numbers:\n");

print("Decimal: ");
print_int(12345);
print("\n");

print("Hex: ");
print_hex(0x1A2B3C);
print("\n");
    while (1) { }

}
