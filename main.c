/*
Name: Niroop C Naik
Date: April 28 2023
*/

#include <xc.h>
#include "main.h"
#include "matrix_keypad.h"
#include "clcd.h"
#include "ds1307.h"
#include "eeprom.h"
#include "adc.h"
#include "uart.h"
#include "i2c.h"

static void init_config(void)
{
	/*Calling initializer for all peripherals*/
	init_clcd();
	init_matrix_keypad();
	init_adc();
	init_i2c();
	init_ds1307();
	init_uart();

	// Storing initial password 1 0 1 0 to Ext. EEPROM
	/*write_external_eeprom(100, 1);
	write_external_eeprom(101, 0);
	write_external_eeprom(102, 1);
	write_external_eeprom(103, 0);*/
}

/*Declare global variables*/
extern int log_iter, log_total_saved;
unsigned char time[9];
char speed[3];

void main(void)
{
	char gear[] = {'O', 'R', 'N', '1', '2', '3', '4'};
	int gear_iter = 0;
	init_config();

	clcd_print("ON", LINE2(9));
	get_time();
	get_speed();
	store_key_log('O');

	while (1)
	{
		clcd_print("TIME    EVENT SP", LINE1(0));

		get_time();
		get_speed();

		clcd_print(time, LINE2(0));
		clcd_print(speed, LINE2(14));

		if (gear_iter != 0)
		{
			clcd_putch('G', LINE2(9));
			clcd_putch(gear[gear_iter], LINE2(10));
		}
		else
		{
			clcd_print("ON", LINE2(9));
		}

		switch (read_switches(STATE_CHANGE))
		{
		case MK_SW1: /*Gear Up*/
		{
			store_key_log(gear[gear_iter]);

			if (++gear_iter > 6)
				gear_iter = 5;
		}
		break;
		case MK_SW2: /*Gear Down*/
		{
			store_key_log(gear[gear_iter]);

			clcd_putch('G', LINE2(9));
			clcd_putch(gear[gear_iter], LINE2(10));
			if (--gear_iter < 1)
				gear_iter = 1;
		}
		break;
		case MK_SW3: /*Collison*/
		{
			store_key_log('C');
			clcd_print(" C", LINE2(9));
		}
		break;
		case MK_SW11: /*Password Menu*/
		{
			delay(2);

			store_key_log('P');
			if (get_password() == 0)	/*Call password func. if matches it returns 0*/
			{
				go_to_menu();			/*call menu func*/
				clcd_print("                ", LINE2(0));
				CLEAR_DISP_SCREEN;
			}
		}
		break;
		}
	}
}

void get_time(void)
{
	static unsigned char clock_reg[3];

	clock_reg[0] = read_ds1307(HOUR_ADDR);
	clock_reg[1] = read_ds1307(MIN_ADDR);
	clock_reg[2] = read_ds1307(SEC_ADDR);

	time[0] = '0' + ((clock_reg[0] >> 4) & 0x03);
	time[1] = '0' + (clock_reg[0] & 0x0F);

	time[2] = ':';

	time[3] = '0' + ((clock_reg[1] >> 4) & 0x0F);
	time[4] = '0' + (clock_reg[1] & 0x0F);

	time[5] = ':';
	time[6] = '0' + ((clock_reg[2] >> 4) & 0x0F);
	time[7] = '0' + (clock_reg[2] & 0x0F);
	time[8] = '\0';
}

void get_speed(void)
{
	unsigned short adc_reg_val = (float)read_adc(CHANNEL4) / 1023 * 99;
	speed[0] = '0' + adc_reg_val / 10;
	speed[1] = '0' + adc_reg_val % 10;
	speed[2] = '\0';
}

void set_time(void)
{
	CLEAR_DISP_SCREEN;
	clcd_print("Enter New Time", LINE1(1));

	unsigned char dummy;

	/* Setting the CH bit of the RTC to Stop the Clock */
	dummy = read_ds1307(SEC_ADDR);
	write_ds1307(SEC_ADDR, dummy | 0x80);

	int hour = 0, min = 0, seconds = 0, field_select = 0;
	unsigned int blinker = 0;
	char time_buffer[9] = "00:00:00";
	clcd_print(time_buffer, LINE2(0));
	char exit_flag = 0;

	while (!exit_flag)
	{

		/*Assign digits to time_buffer array*/
		time_buffer[0] = hour / 10 + '0';
		time_buffer[1] = hour % 10 + '0';
		time_buffer[3] = min / 10 + '0';
		time_buffer[4] = min % 10 + '0';
		time_buffer[6] = seconds / 10 + '0';
		time_buffer[7] = seconds % 10 + '0';

		clcd_putch(time_buffer[2], LINE2(2));
		clcd_putch(time_buffer[5], LINE2(5));

		if (blinker++ <= 1500)
		{
			switch (field_select)
			{
			case 0:
				clcd_putch(' ', LINE2(0));
				clcd_putch(' ', LINE2(1));
				break;
			case 1:
				clcd_putch(' ', LINE2(3));
				clcd_putch(' ', LINE2(4));
				break;
			case 2:
				clcd_putch(' ', LINE2(6));
				clcd_putch(' ', LINE2(7));
				break;
			}
		}
		else
		{
			if (blinker == 3000)
				blinker = 0;
			switch (field_select)
			{
			case 0:
				clcd_putch(time_buffer[0], LINE2(0));
				clcd_putch(time_buffer[1], LINE2(1));
				break;
			case 1:
				clcd_putch(time_buffer[3], LINE2(3));
				clcd_putch(time_buffer[4], LINE2(4));
				break;
			case 2:
				clcd_putch(time_buffer[6], LINE2(6));
				clcd_putch(time_buffer[7], LINE2(7));
				break;
			}
		}

		switch (read_switches(STATE_CHANGE))
		{
		case MK_SW11: /*Increment Time*/
			clcd_print(time_buffer, LINE2(0));
			field_select = (field_select + 1) % 3;
			break;
		case MK_SW12: /*Decrement Time*/
		{
			delay(5);
			// long press check
			if (read_switches(LEVEL_CHANGE) == MK_SW12)
				exit_flag = 1;
			else
			{
				switch (field_select)
				{
				case 0:
					hour = (hour + 1) % 24;
					break;
				case 1:
					min = (min + 1) % 60;
					break;
				case 2:
					seconds = (seconds + 1) % 60;
					break;
				}
			}
		}
		break;
		}
	}

	/*Update Time and write to RTC*/
	static unsigned char clock_reg[3];

	clock_reg[0] = read_ds1307(HOUR_ADDR);
	clock_reg[1] = read_ds1307(MIN_ADDR);
	clock_reg[2] = read_ds1307(SEC_ADDR);

	/*Storing Values in BCD Form*/
	/*Update Hour*/
	write_ds1307(HOUR_ADDR, (clock_reg[0] & 0xCF) | (hour / 10 << 4));
	write_ds1307(HOUR_ADDR, (clock_reg[0] & 0xF0) | (hour % 10));

	/*Update Minutes*/
	write_ds1307(MIN_ADDR, (clock_reg[1] & 0x0F) | (min / 10 << 4));
	write_ds1307(MIN_ADDR, (clock_reg[1] & 0xF0) | min % 10);

	/*Update Seconds*/
	write_ds1307(SEC_ADDR, (clock_reg[2] & 0x0F) | (seconds / 10 << 4));
	write_ds1307(SEC_ADDR, (clock_reg[2] & 0xF0) | seconds % 10);

	/* Clearing the CH bit of the RTC to Start the Clock */
	dummy = read_ds1307(SEC_ADDR);
	write_ds1307(SEC_ADDR, dummy & 0x7F);

	CLEAR_DISP_SCREEN;
	clcd_print("Time Updated!", LINE1(1));
	delay(5);
	CLEAR_DISP_SCREEN;
}

void delay(int ms)
{
	for (int i = 0; i < ms; i++)
		for (unsigned int wait = 50000; wait--;)
			;
}