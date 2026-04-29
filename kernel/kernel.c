#include "screen.h"
#include "cpu.h"

void kernel_main() {
    char vendor[13];

    clear_screen();

    print("NanoVMM v0.2\n\n");

    get_cpu_vendor(vendor);

    print("CPU Vendor: ");
    print(vendor);
    print("\n");

    if (cpu_has_vmx()) {
        print("VT-x: Supported\n");
    } else {
        print("VT-x: Not Supported\n");
    }

    while (1) { }
}
