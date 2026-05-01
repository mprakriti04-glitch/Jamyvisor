#include <stdint.h>
#include "screen.h"
#define VGA_WIDTH 80
#define VGA_HEIGHT 25
#define VGA_MEMORY ((uint16_t*)0xB8000)
#define COLOR_WHITE 0x07
#define COLOR_GREEN 0x0A
#define COLOR_RED   0x0C
#define COLOR_YELLOW 0x0E
static uint16_t* const vga_buffer = VGA_MEMORY;
static int cursor_row = 0;
static int cursor_col = 0;
static uint8_t color = 0x07; // white on black

// Clear entire screen
void clear_screen() {
    for (int i = 0; i < VGA_WIDTH * VGA_HEIGHT; i++) {
        vga_buffer[i] = (color << 8) | ' ';
    }

    cursor_row = 0;
    cursor_col = 0;
}
void scroll() {
    // Move each row up
    for (int row = 1; row < VGA_HEIGHT; row++) {
        for (int col = 0; col < VGA_WIDTH; col++) {
            vga_buffer[(row - 1) * VGA_WIDTH + col] =
                vga_buffer[row * VGA_WIDTH + col];
        }
    }

    // Clear last row
    for (int col = 0; col < VGA_WIDTH; col++) {
        vga_buffer[(VGA_HEIGHT - 1) * VGA_WIDTH + col] =
            (color << 8) | ' ';
    }

    // Move cursor back to last row
    cursor_row = VGA_HEIGHT - 1;
}
// Low-level character output
static void putchar(char c) {
    if (c == '\n') {
        cursor_col = 0;
        cursor_row++;

        if (cursor_row >= VGA_HEIGHT) {
            scroll();
        }
        return;
    }

    int index = cursor_row * VGA_WIDTH + cursor_col;
    vga_buffer[index] = (color << 8) | c;

    cursor_col++;

    if (cursor_col >= VGA_WIDTH) {
        cursor_col = 0;
        cursor_row++;
    }

    if (cursor_row >= VGA_HEIGHT) {
        scroll();
    }
}

// Print string
void print(const char* str) {
    for (int i = 0; str[i] != '\0'; i++) {
        putchar(str[i]);
    }
}

// Logger functions (Day 3 upgrade)

// Informational message
void print_info(const char* msg) {
    print("[INFO] ");
    print(msg);
    print("\n");
}

// Success message
void print_ok(const char* msg) {
    print("[OK] ");
    print(msg);
    print("\n");
}

// Error message
void print_err(const char* msg) {
    print("[ERR] ");
    print(msg);
    print("\n");
}
void log_level(log_level_t level, const char* msg) {
    uint8_t old_color = color;

    const char* tag;

    switch (level) {
        case LOG_OK:
            color = COLOR_GREEN;
            tag = "OK";
            break;
        case LOG_ERR:
            color = COLOR_RED;
            tag = "ERR";
            break;
        case LOG_WARN:
            color = COLOR_YELLOW;
            tag = "WARN";
            break;
        default:
            color = COLOR_WHITE;
            tag = "INFO";
            break;
    }

    print("[");
    print(tag);
    print("] ");

    color = COLOR_WHITE;
    print(msg);
    print("\n");

    color = old_color;
}
char* itoa(int value, char* str, int base) {
    char* rc = str;
    char* ptr = str;
    char* low;


    // Check base
    if (base < 2 || base > 36) {
        *str = '\0';
        return str;
    }

    // Handle negative numbers (only for base 10)
    if (value < 0 && base == 10) {
        *ptr++ = '-';
    }

    low = ptr;

    // Convert number
    do {
        *ptr++ = "0123456789abcdefghijklmnopqrstuvwxyz"[value % base < 0 ? -(value % base) : value % base];
        value /= base;
    } while (value);

    *ptr-- = '\0';

    // Reverse string
    while (low < ptr) {
        char tmp = *low;
        *low++ = *ptr;
        *ptr-- = tmp;
    }

    return rc;
}
void print_int(int value) {
    char buffer[32];
    itoa(value, buffer, 10);
    print(buffer);
}

void print_hex(unsigned int value) {
    char buffer[32];
    itoa(value, buffer, 16);
    print("0x");
    print(buffer);
}
