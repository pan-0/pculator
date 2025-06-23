#ifndef XTIME_H
#define XTIME_H

#ifdef _WIN32
#	include <Windows.h>  /* SYSTEMTIME */
	typedef SYSTEMTIME xtime;
#else
#	include <time.h>  /* struct tm */
	typedef struct {
		struct tm tm;
		long long msec;  /* Milliseconds. */
	} xtime;
#endif

void xt_get(xtime *xt);
int xt_sec(const xtime *xt);
int xt_min(const xtime *xt);
int xt_hour(const xtime *xt);
int xt_wday(const xtime *xt);
int xt_mday(const xtime *xt);
int xt_mon(const xtime *xt);
int xt_year(const xtime *xt);
long long xt_msec(const xtime *xt);

#endif  /* XTIME_H */

