#include "xtime.h"

#if 0
// $ man 3 tm
int         tm_sec;    /* Seconds          [0, 60] */
int         tm_min;    /* Minutes          [0, 59] */
int         tm_hour;   /* Hour             [0, 23] */
int         tm_mday;   /* Day of the month [1, 31] */
int         tm_mon;    /* Month            [0, 11]  (January = 0) */
int         tm_year;   /* Year minus 1900 */
int         tm_wday;   /* Day of the week  [0, 6]   (Sunday = 0) */
int         tm_yday;   /* Day of the year  [0, 365] (Jan/01 = 0) */
int         tm_isdst;  /* Daylight savings flag */
#endif

/*
 * Windows.
 */

#ifdef _WIN32
#include <Windows.h>  /* SYSTEMTIME, GetLocalTime() */

void xt_get(xtime *xt)
{
	(void) GetLocalTime(xt);
}

int xt_sec(const xtime *xt)  { return (int)xt->wSecond;      }
int xt_min(const xtime *xt)  { return (int)xt->wMinute;      }
int xt_hour(const xtime *xt) { return (int)xt->wHour;        }
int xt_wday(const xtime *xt) { return (int)xt->wDayOfWeek;   }
int xt_mday(const xtime *xt) { return (int)xt->wDay;         }
int xt_mon(const xtime *xt)  { return (int)xt->wMonth - 1;   }
int xt_year(const xtime *xt) { return (int)xt->wYear - 1900; }

long long xt_msec(const xtime *xt)
{
	return (long long)xt->wMilliseconds;
}


/*
 * Standard C11.
 */
#else

/* struct tm, struct timespec, timespec_get(), localtime_r() */
#include <time.h>

void xt_get(xtime *xt)
{
	struct timespec ts;

	/* Equivalent to `clock_gettime(CLOCK_REALTIME, &ts);` under POSIX. */
	(void) timespec_get(&ts, TIME_UTC);

	(void) localtime_r(&ts.tv_sec, &xt->tm);
	xt->msec = ts.tv_nsec / 1000000;
}

int xt_sec(const xtime *xt)  { return xt->tm.tm_sec;  }
int xt_min(const xtime *xt)  { return xt->tm.tm_min;  }
int xt_hour(const xtime *xt) { return xt->tm.tm_hour; }
int xt_wday(const xtime *xt) { return xt->tm.tm_wday; }
int xt_mday(const xtime *xt) { return xt->tm.tm_mday; }
int xt_mon(const xtime *xt)  { return xt->tm.tm_mon;  }
int xt_year(const xtime *xt) { return xt->tm.tm_year; }

long long xt_msec(const xtime *xt)
{
	return xt->msec;
}

#endif

