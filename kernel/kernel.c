void kernel_main() {
    volatile char* vga = (volatile char*)0xB8000;

    const char* msg = "NanoVMM starting...";

    for (int i = 0; msg[i] != '\0'; i++) {
        vga[i * 2] = msg[i];
        vga[i * 2 + 1] = 0x07;
    }

    while (1) { }
}
