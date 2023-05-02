#include <xc.h>
#include "clcd.h"
#include "main.h"
#include "uart.h"
#include "matrix_keypad.h"

extern int log_iter, log_total_saved;

void go_to_menu(void)
{
	const char *menu[] = {"View Logs", "Download Logs", "Clear Logs", "Change Pwd", "Set Time"};
	int i = 0, pos = 0, once = 0, ret_value, seconds = 0, exit_flag = 0;

	while (!exit_flag)
	{
		if (pos == 0)
			clcd_putch('*', LINE1(0));
		else
			clcd_putch('*', LINE2(0));

		clcd_print(menu[i], LINE1(2));
		clcd_print(menu[i + 1], LINE2(2));

		switch (read_switches(STATE_CHANGE))
		{
		case MK_SW11: /*Scroll Up*/
		{
			delay(5);
			// long press check
			if (read_switches(LEVEL_CHANGE) == MK_SW11)
			{
				switch (pos + i)
				{
				case 0: /*View Log*/
				{
					store_key_log('V');
					CLEAR_DISP_SCREEN;
					if (log_total_saved > 0)
					{
						view_log();
					}
					else
						clcd_print("NO LOGS TO PRINT", LINE1(0));

					delay(10);

					CLEAR_DISP_SCREEN;
				}
				break;
				case 1: /*Download Log*/
				{
					store_key_log('D');
					CLEAR_DISP_SCREEN;
					clcd_print("Downloading", LINE1(2));
					clcd_print("Logs.....", LINE2(3));
					delay(5);

					for (int i = 0; i < log_total_saved && i < 10; i++)
					{
						putch('\r');
						print_to_console(i);
					}

					CLEAR_DISP_SCREEN;
				}
				break;
				case 2:
					/*Clear Logs*/
					{
						store_key_log('X');
						log_iter = 0;
						log_total_saved = 0;
						CLEAR_DISP_SCREEN;
						clcd_print("LOGS CLEARED!", LINE1(0));
						delay(10);
						CLEAR_DISP_SCREEN;
					}
					break;
				case 3: /*Change Pwd*/
				{
					store_key_log('W');
					change_password();
				}
				break;
				case 4: /*Set time*/
				{
					CLEAR_DISP_SCREEN;
					set_time();
					CLEAR_DISP_SCREEN;
				}
				break;
				}
			}
			else
			{
				if (i > 0) // if key is pressed once, decrement count
				{
					CLEAR_DISP_SCREEN;
					if (--pos < 0)
					{
						i--;
						pos = 0;
					}
				}
			}
		}
		break;
		case MK_SW12: /*Scroll Down*/
		{
			delay(5);
			// long press check
			if (read_switches(LEVEL_CHANGE) == MK_SW12)
				exit_flag = 1;
			else
			{
				if (i < 3) // if key is pressed once, increment count
				{
					CLEAR_DISP_SCREEN;
					if (++pos >= 2)
					{
						i++;
						pos = 1;
					}
				}
			}
		}
		break;
		}
	}
}