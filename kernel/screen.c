#include "screen.h"

volatile char* vga = (volatile char*)0xB8000;
int cursor = 0;

void clear_screen() {
    for (int i = 0; i < 80 * 25 * 2; i += 2) {
        vga[i] = ' ';
        vga[i + 1] = 0x07;
    }
    cursor = 0;
}

void print(const char* str) {
    while (*str) {
        vga[cursor++] = *str++;
        vga[cursor++] = 0x07;
    }
}

void print_color(const char* str, char color) {
    while (*str) {
        vga[cursor++] = *str++;
        vga[cursor++] = color;
    }
}
