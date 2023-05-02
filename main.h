#ifndef MAIN_H
#define MAIN_H

static void init_config(void);
void get_time(void);
void get_speed(void);
void store_key_log(char gear);
void print_to_console(int iter);
char get_password(void);
void go_to_menu(void);
void view_log(void);
void print_log_to_clcd(char LINE, char iter);
void change_password(void);
void set_time(void);
void delay(int ms);

#endif
