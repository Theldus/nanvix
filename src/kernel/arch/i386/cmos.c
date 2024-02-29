/*
 * Copyright(C) 2016-2016 Subhra S. Sarkar <rurtle.coder@gmail.com>
 *              2011-2016 Pedro H. Penna   <pedrohenriquepenna@gmail.com>
 * 
 * This file is part of Nanvix.
 * 
 * Nanvix is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 * 
 * Nanvix is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with Nanvix. If not, see <http://www.gnu.org/licenses/>.
 */

#include <nanvix/hal.h>
#include <nanvix/clock.h>

/* CMOS register addresses */
#define CMOS_REG_SEC          0x00  /* seconds register */
#define CMOS_REG_MIN          0x02  /* minutes register */
#define CMOS_REG_HOUR         0x04  /* hours register */
#define CMOS_REG_DOM          0x07  /* day of month register */
#define CMOS_REG_MON          0x08  /* month register */
#define CMOS_REG_YEAR         0x09  /* year register */
#define CMOS_REG_STATUS_A     0x0A  /* status register A */
#define CMOS_REG_STATUS_B     0x0B  /* status register B */

/* Macro to convert binary-coded decimal (BCD) to binary */
#define BCD_TO_BIN(x) (((x)&0x0f) + ((x) / 16) * 10)

/* Flags for CMOS register STATUS_B */
#define BCD_FORMAT_FLAG  0x04  /* BCD time format flag */
#define TIME_FORMAT_FLAG 0x02  /* 24-hour time format flag */

/* CMOS Ports and Masks */
#define CMOS_ADDR_PORT 0x70        /* CMOS address port. */
#define CMOS_DATA_PORT 0x71        /* CMOS data port.    */
#define CMOS_NMI_DISABLE_MASK 0x80 /* NMI disable mask   */

/**
 * @brief CMOS Time Structure.
 */
struct cmos_time
{
	unsigned char sec;   /**< Seconds.      */
	unsigned char min;   /**< Minutes.      */
	unsigned char hour;  /**< Hour.         */
	unsigned char dom;   /**< Day of Month. */
	unsigned char mon;   /**< Month.        */
	unsigned short year; /**< Year.         */
} cmos_time;

/**
 * @brief Current millennium.
 */
#define CURR_MILLENNIUM 2000

/**
 * @brief Read a byte from the CMOS device.
 *
 * @param addr Target address.
 * @return The value read.
 */
static uint8_t cmos_read(uint8_t addr)
{
	/* Disable NMI at the highest order bit of the address. */
	outputb(CMOS_ADDR_PORT, CMOS_NMI_DISABLE_MASK | addr);
	return inputb(CMOS_DATA_PORT);
}

/**
 * @brief Convert CMOS time to Unix timestamp.
 *
 * @NOTE  Since register 0x09 gives only last 2 digits of the year, it's our
 *        responsibility to add it with right offset.
 *
 * @param time The CMOS time to convert.
 * @return The Unix timestamp.
 */
static int32_t cmos_to_unix_time(void)
{
	int year   = CURR_MILLENNIUM + cmos_time.year;
	int month  = cmos_time.mon;
	int day    = cmos_time.dom;
	int hour   = cmos_time.hour;
	int minute = cmos_time.min;
	int second = cmos_time.sec;

	int era      = 0; /* Era is a 400 yr period  */
	int yoe      = 0; /* Year of era [0, 399]    */
	int doy      = 0; /* Day of year [0, 365]    */
	int doe      = 0; /* Day of era  [0, 146096] */
	int num_days = 0; /* # of days since Epoch   */
	int num_secs = 0; /* # of clock-ticks since Epoch */

	/* Subtract 1 from year if month is January or February */
	year -= month <= 2;
	/* Determine the Gregorian era based on year */
	era = (year >= 0 ? year : year - 399) / 400;
	/* Determine the year of era */
	yoe = (year - era * 400);
	/* Determine the day of year (1-365) */
	doy = (153 * (month + (month > 2 ? -3 : 9)) + 2) / 5 + day - 1;
	/* Determine the day of era */
	doe = yoe * 365 + yoe / 4 - yoe / 100 + doy;
	/* Determine the number of days since 1/1/1970 */
	num_days = era * 146097 + doe - 719468;
	/* Determine the total number of seconds */
	num_secs = (num_days * 86400) + (hour * 3600) + (minute * 60) + second;

	return num_secs;
}

/**
 * @brief Read the current time from the CMOS device and return it as a Unix
 *        timestamp.
 *
 * @return The current Unix timestamp.
 */
PUBLIC void cmos_init(void)
{
	uint8_t register_b;

	/* Read the time from the CMOS device. */
	do
	{
		cmos_time.sec  = cmos_read(CMOS_REG_SEC);
		cmos_time.min  = cmos_read(CMOS_REG_MIN);
		cmos_time.hour = cmos_read(CMOS_REG_HOUR);
		cmos_time.dom  = cmos_read(CMOS_REG_DOM);
		cmos_time.mon  = cmos_read(CMOS_REG_MON);
		cmos_time.year = cmos_read(CMOS_REG_YEAR);
	} while (cmos_time.sec != cmos_read(CMOS_REG_SEC));

	register_b = cmos_read(CMOS_REG_STATUS_B);
	if (!(register_b & BCD_FORMAT_FLAG))
	{
		/* Convert BCD values to binary. */
		cmos_time.sec  = BCD_TO_BIN(cmos_time.sec);
		cmos_time.min  = BCD_TO_BIN(cmos_time.min);
		cmos_time.hour = BCD_TO_BIN(cmos_time.hour & 0x7f) | (cmos_time.hour & 0x80);
		cmos_time.dom  = BCD_TO_BIN(cmos_time.dom);
		cmos_time.mon  = BCD_TO_BIN(cmos_time.mon);
		cmos_time.year = BCD_TO_BIN(cmos_time.year);
	}

	/* Adjust hour if 12-hour format. */
	if (!(register_b & TIME_FORMAT_FLAG) && (cmos_time.hour & 0x80))
		cmos_time.hour = ((cmos_time.hour & 0x7f) + 12) % 24;

	startup_time = cmos_to_unix_time();
}

