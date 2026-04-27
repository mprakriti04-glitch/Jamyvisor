#include "screen.h"

void kernel_main() {
    clear_screen();

    print("NanoVMM Day 2 ");
    print_color("ONLINE ", 0x0A);
    print(" | ");

    print_color("Screen Driver Loaded ", 0x0B);
    print(" | ");

    print_color("Ready", 0x0E);

    while (1) { }
}
