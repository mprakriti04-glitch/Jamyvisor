#ifndef SCREEN_H
#define SCREEN_H
typedef enum {
    LOG_OK,
    LOG_INFO,
    LOG_ERR,
    LOG_WARN
} log_level_t;
void clear_screen();
void print(const char* str);

void print_info(const char* msg);
void print_ok(const char* msg);
void print_err(const char* msg);
void log_level(log_level_t level, const char* msg);
void print_int(int value);
void print_hex(unsigned int value);
#endif
