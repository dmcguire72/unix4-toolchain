#include "pdp11.h"
#include <string.h>
#include <stdlib.h>
#include <time.h>

int hooks_on;
static uint16_t a_gmtime, a_asctime, a_cbuf, a_xtime;
static int have_gm, have_asc;

static uint16_t find_xtime(uint16_t pc, uint16_t cbuf_addr)
{
	for (int i = 0; i < 512; i += 2) {
		uint16_t ir = rdw(pc + i);
		if ((ir & 0177770) == 012700) {
			uint16_t imm = rdw(pc + i + 2);
			if (cbuf_addr != 0) {
				int diff = (int)imm - (int)cbuf_addr;
				if (diff > 0 && diff < 256 && !(imm & 1))
					return imm;
			}
		}
	}
	return 0;
}

static void set_tzname(void)
{
	uint16_t tz, p;
	time_t now = time(NULL);
	struct tm tm;
	char nm[8];
	int i;
	if (!sym_lookup("tzname", &tz) || !localtime_r(&now, &tm))
		return;
	if (strlen(tm.tm_zone) <= 3)
		snprintf(nm, sizeof nm, "%s", tm.tm_zone);
	else
		snprintf(nm, sizeof nm, "%c%02d", tm.tm_gmtoff < 0 ? '-' : '+',
		    (int)(labs(tm.tm_gmtoff) / 3600 % 100));
	for (i = 0; i < 2; i++) {
		p = rdw(tz + 2 * i);
		if (p < 0400 || p > 0177770)
			continue;
		memset(&mem[p], 0, 4);
		memcpy(&mem[p], nm, strlen(nm));
	}
}

void hooks_load(void)
{
	uint16_t v;
	hooks_on = have_gm = have_asc = 0;
	a_xtime = 0;

	if (!sym_lookup("ldivr", &v))
		return;

	if (sym_lookup("asctime", &a_asctime) && sym_lookup("cbuf", &a_cbuf))
		have_asc = 1;

	if (sym_lookup("gmtime", &a_gmtime)) {
		have_gm = 1;
		a_xtime = find_xtime(a_gmtime, a_cbuf);
	}

	hooks_on = have_gm || have_asc;
	set_tzname();
}

static void do_gmtime(uint16_t ret)
{
	uint16_t tp = rdw(SP + 2);
	int32_t t = (int32_t)((uint32_t)rdw(tp) << 16 | rdw(tp + 2));
	time_t real = (time_t)t + guest_time_shift();
	struct tm tm;

	if (a_xtime) {
		R[0] = a_xtime;
		PC = ret;
		SP += 2;
	} else {
		int stop = SP + 2;
		while (!(PC == ret && SP == (uint16_t)stop))
			step();
		a_xtime = R[0];
	}

	if (!gmtime_r(&real, &tm))
		return;

	wrw(a_xtime + 0, tm.tm_sec);
	wrw(a_xtime + 2, tm.tm_min);
	wrw(a_xtime + 4, tm.tm_hour);
	wrw(a_xtime + 6, tm.tm_mday);
	wrw(a_xtime + 8, tm.tm_mon);
	wrw(a_xtime + 10, tm.tm_year);
	wrw(a_xtime + 12, tm.tm_wday);
	wrw(a_xtime + 14, tm.tm_yday);
	wrw(a_xtime + 16, 0);
}

static void do_asctime(void)
{
	static const char days[] = "SunMonTueWedThuFriSat";
	static const char mons[] = "JanFebMarAprMayJunJulAugSepOctNovDec";
	uint16_t tp = rdw(SP + 2), i;
	int f[9];
	char buf[40];

	for (i = 0; i < 9; i++)
		f[i] = (int16_t)rdw(tp + 2 * i);
	if (f[6] < 0 || f[6] > 6) f[6] = 0;
	if (f[4] < 0 || f[4] > 11) f[4] = 0;

	snprintf(buf, sizeof buf, "%.3s %.3s %2d %02d:%02d:%02d %04d\n",
	    days + 3 * f[6], mons + 3 * f[4], f[3] % 100, f[2] % 100,
	    f[1] % 100, f[0] % 100, (1900 + f[5]) % 10000);

	for (i = 0; i < 26; i++)
		mem[(uint16_t)(a_cbuf + i)] = (uint8_t)buf[i];

	R[0] = a_cbuf;
	PC = rdw(SP);
	SP += 2;
}

int jsr_hook(uint16_t target)
{
	if (have_asc && target == a_asctime) {
		do_asctime();
		return 1;
	}
	if (have_gm && target == a_gmtime) {
		do_gmtime(rdw(SP));
		return 1;
	}
	return 0;
}