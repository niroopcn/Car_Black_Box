#include <xc.h>
#include "main.h"
#include "eeprom.h"
#include "uart.h"
#include "i2c.h"
#include "matrix_keypad.h"
#include "clcd.h"

int log_iter, log_total_saved;

extern char speed[3];
extern unsigned char time[9];

void store_key_log(char gear)
{
    write_external_eeprom(log_iter * 10 + 0, time[0]);
    write_external_eeprom(log_iter * 10 + 1, time[1]);
    write_external_eeprom(log_iter * 10 + 2, time[3]);
    write_external_eeprom(log_iter * 10 + 3, time[4]);
    write_external_eeprom(log_iter * 10 + 4, time[6]);
    write_external_eeprom(log_iter * 10 + 5, time[7]);

    if (gear == 'C')
    {
        write_external_eeprom(log_iter * 10 + 6, ' ');
        write_external_eeprom(log_iter * 10 + 7, 'C');
    }
    else if (gear == 'O')
    {
        write_external_eeprom(log_iter * 10 + 6, 'O');
        write_external_eeprom(log_iter * 10 + 7, 'N');
    }
    else if(gear == 'P')
    {
        write_external_eeprom(log_iter * 10 + 6, 'P');
        write_external_eeprom(log_iter * 10 + 7, 'A');
    }
    else if(gear == 'V')
    {
        write_external_eeprom(log_iter * 10 + 6, 'V');
        write_external_eeprom(log_iter * 10 + 7, 'L');
    }
    else if(gear == 'D')
    {
        write_external_eeprom(log_iter * 10 + 6, 'D');
        write_external_eeprom(log_iter * 10 + 7, 'L');
    }
    else if(gear == 'X')
    {
        write_external_eeprom(log_iter * 10 + 6, 'C');
        write_external_eeprom(log_iter * 10 + 7, 'L');
    }
    else if(gear == 'W')
    {
        write_external_eeprom(log_iter * 10 + 6, 'C');
        write_external_eeprom(log_iter * 10 + 7, 'P');
    }
    else
    {
        write_external_eeprom(log_iter * 10 + 6, 'G');
        write_external_eeprom(log_iter * 10 + 7, gear);
    }

    write_external_eeprom(log_iter * 10 + 8, speed[0]);
    write_external_eeprom(log_iter * 10 + 9, speed[1]);

    log_iter = (log_iter + 1) % 10;
    log_total_saved++;
}

void print_to_console(int iter)
{
    putch(read_external_eeprom(iter * 10 + 0));
    putch(read_external_eeprom(iter * 10 + 1));
    putch(':');
    putch(read_external_eeprom(iter * 10 + 2));
    putch(read_external_eeprom(iter * 10 + 3));
    putch(':');
    putch(read_external_eeprom(iter * 10 + 4));
    putch(read_external_eeprom(iter * 10 + 5));
    putch(' ');
    putch(read_external_eeprom(iter * 10 + 6));
    putch(read_external_eeprom(iter * 10 + 7));
    putch(' ');
    putch(read_external_eeprom(iter * 10 + 8));
    putch(read_external_eeprom(iter * 10 + 9));
    putch('\n');
}

void print_log_to_clcd(char LINE, char iter)
{
    clcd_putch(read_external_eeprom(iter * 10 + 0), LINE + 2);
    clcd_putch(read_external_eeprom(iter * 10 + 1), LINE + 3);
    clcd_putch(':', LINE + 4);
    clcd_putch(read_external_eeprom(iter * 10 + 2), LINE + 5);
    clcd_putch(read_external_eeprom(iter * 10 + 3), LINE + 6);
    clcd_putch(':', LINE + 7);
    clcd_putch(read_external_eeprom(iter * 10 + 4), LINE + 8);
    clcd_putch(read_external_eeprom(iter * 10 + 5), LINE + 9);
    clcd_putch(' ', LINE + 10);
    clcd_putch(read_external_eeprom(iter * 10 + 6), LINE + 11);
    clcd_putch(read_external_eeprom(iter * 10 + 7), LINE + 12);
    clcd_putch(' ', LINE + 13);
    clcd_putch(read_external_eeprom(iter * 10 + 8), LINE + 14);
    clcd_putch(read_external_eeprom(iter * 10 + 9), LINE + 15);
}

void view_log(void)
{
    int i = 0, exit_flag = 0;
    char string[16] = {0};

    while (!exit_flag)
    {
        clcd_print("Logs:", LINE1(0));
        clcd_putch('0' + i, LINE2(0));
        clcd_putch('.', LINE2(1));
        print_log_to_clcd(0xC0, i); // LINE2

        switch (read_switches(STATE_CHANGE))
        {
        case MK_SW11: /*Scroll Up*/
        {
            if (i > 0)
                i--;
        }
        break;
        case MK_SW12: /*Scroll Down*/
        {
            delay(5);
            // long press check
            if (read_switches(LEVEL_CHANGE) == MK_SW12)
                exit_flag = 1;
            else if (log_total_saved > 9)
            {
                if (i < 9)
                    i++;
            }
            else if (i < log_iter)
                i++;
        }
        }
    }
    CLEAR_DISP_SCREEN;
}