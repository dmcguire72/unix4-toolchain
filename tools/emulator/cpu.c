#include "pdp11.h"

static void setcc_add(uint32_t s, uint32_t d, uint32_t r, int byte)
{
	uint32_t m = byte ? 0x80 : 0x8000, mask = byte ? 0xff : 0xffff;
	r &= 0x1ffff;
	(void)mask;
	fV = ((s & m) == (d & m)) && ((r & m) != (s & m));
	fC = (r >> (byte ? 8 : 16)) & 1;
}

static void branch(int cond, int off)
{
	if (cond) PC += 2 * (int8_t)off;
}

void step(void)
{
	uint16_t ir = rdw(PC), s, d, r;
	int sm, sr, dm, dr, op, byte;
	uint16_t at = PC;
	(void)at;
	PC += 2;
	op = ir >> 12;
	byte = (ir >> 15) & 1;
	sm = (ir >> 9) & 7;
	sr = (ir >> 6) & 7;
	dm = (ir >> 3) & 7;
	dr = ir & 7;

	switch (op & 7) {
	case 1: /* MOV / MOVB */
		getop(sm, sr, byte);
		s = load(byte);
		getop(dm, dr, byte);
		nz(s, byte);
		fV = 0;
		if (byte) storeb_sext(s); else store(s, 0);
		return;
	case 2: /* CMP */
		getop(sm, sr, byte);
		s = load(byte);
		getop(dm, dr, byte);
		d = load(byte);
		r = s - d;
		nz(r, byte);
		{
			uint32_t m = byte ? 0x80 : 0x8000;
			fV = ((s & m) != (d & m)) && ((r & m) == (d & m));
			fC = byte ? ((s & 0xff) < (d & 0xff)) : (s < d);
		}
		return;
	case 3: /* BIT */
		getop(sm, sr, byte);
		s = load(byte);
		getop(dm, dr, byte);
		d = load(byte);
		nz(s & d, byte);
		fV = 0;
		return;
	case 4: /* BIC */
		getop(sm, sr, byte);
		s = load(byte);
		getop(dm, dr, byte);
		d = load(byte);
		r = d & ~s;
		nz(r, byte);
		fV = 0;
		store(r, byte);
		return;
	case 5: /* BIS */
		getop(sm, sr, byte);
		s = load(byte);
		getop(dm, dr, byte);
		d = load(byte);
		r = d | s;
		nz(r, byte);
		fV = 0;
		store(r, byte);
		return;
	case 6: /* ADD / SUB */
		getop(sm, sr, 0);
		s = load(0);
		getop(dm, dr, 0);
		d = load(0);
		if (!byte) {
			uint32_t t = (uint32_t)s + d;
			r = t;
			nz(r, 0);
			setcc_add(s, d, t, 0);
		} else {
			r = d - s;
			nz(r, 0);
			fV = ((s & 0x8000) != (d & 0x8000)) && ((r & 0x8000) == (s & 0x8000));
			fC = d < s;
		}
		store(r, 0);
		return;
	}

	/* op & 7 == 0 or 7 */
	if (op == 7) { /* EIS */
		int rg = (ir >> 6) & 7;
		switch ((ir >> 9) & 7) {
		case 0: { /* MUL */
			int32_t a, res;
			getop(dm, dr, 0);
			s = load(0);
			a = (int16_t)R[rg];
			res = a * (int16_t)s;
			if (rg & 1) R[rg] = res;
			else { R[rg] = res >> 16; R[rg | 1] = res; }
			fN = res < 0;
			fZ = res == 0;
			fV = 0;
			fC = res < -32768 || res > 32767;
			return;
		}
		case 1: { /* DIV */
			int32_t dv, q, rm;
			int16_t sv;
			getop(dm, dr, 0);
			sv = load(0);
			if (sv == 0) { fV = fC = 1; return; }
			dv = (int32_t)((uint32_t)R[rg] << 16 | R[rg | 1]);
			q = dv / sv;
			rm = dv % sv;
			if (q > 32767 || q < -32768) { fV = 1; fC = 0; return; }
			R[rg] = q;
			R[rg | 1] = rm;
			fN = q < 0;
			fZ = q == 0;
			fV = 0;
			fC = 0;
			return;
		}
		case 2: { /* ASH */
			int n_;
			int16_t v = R[rg];
			int32_t res;
			getop(dm, dr, 0);
			n_ = load(0) & 077;
			if (n_ & 040) n_ -= 64;
			fV = 0;
			fC = 0;
			if (n_ > 0) {
				int32_t t = v;
				int k;
				for (k = 0; k < n_; k++) { fC = (t >> 15) & 1; t = (int16_t)(t << 1); }
				res = t;
				fV = ((int16_t)res < 0) != (v < 0);
			} else if (n_ < 0) {
				int32_t t = v;
				int k;
				for (k = 0; k < -n_; k++) { fC = t & 1; t >>= 1; }
				res = t;
			} else {
				res = v;
			}
			R[rg] = res;
			nz(R[rg], 0);
			return;
		}
		case 3: { /* ASHC */
			int n_;
			int32_t v = (int32_t)((uint32_t)R[rg] << 16 | R[rg | 1]);
			getop(dm, dr, 0);
			n_ = load(0) & 077;
			if (n_ & 040) n_ -= 64;
			fV = 0;
			fC = 0;
			if (n_ > 0) {
				int k;
				int32_t t = v;
				for (k = 0; k < n_; k++) { fC = (t >> 31) & 1; t = (int32_t)((uint32_t)t << 1); }
				fV = (t < 0) != (v < 0);
				v = t;
			} else if (n_ < 0) {
				int k;
				for (k = 0; k < -n_; k++) { fC = v & 1; v >>= 1; }
			}
			R[rg] = (uint32_t)v >> 16;
			R[rg | 1] = v;
			fN = v < 0;
			fZ = v == 0;
			return;
		}
		case 4: /* XOR */
			getop(dm, dr, 0);
			d = load(0);
			r = d ^ R[rg];
			nz(r, 0);
			fV = 0;
			store(r, 0);
			return;
		case 7: /* SOB */
			if (--R[rg] != 0) PC -= 2 * (ir & 077);
			return;
		}
		die("bad EIS instruction");
	}

	if ((ir & 0170000) == 0170000) { fstep(ir); return; }

	/* op == 0 or 8 (byte forms of single-operand/branch/trap) */
	if (!byte) {
		switch (ir >> 6) {
		case 0:
			if (ir == 0) die("HALT");
			if (ir == 5 || ir == 6 || ir == 1 || ir == 2 || ir == 3 || ir == 4)
				die("unsupported privileged instruction");
			break;
		case 1: /* JMP */
			getop(dm, dr, 0);
			if (regdst >= 0) die("JMP to register");
			PC = ea;
			return;
		case 2:
			if ((ir & 070) == 0) { /* RTS */
				PC = R[dr];
				R[dr] = rdw(SP);
				SP += 2;
				return;
			}
			if ((ir & 0177760) == 0000240 || (ir & 0177760) == 0000260) { /* CCC/SCC */
				int set = ir & 020;
				if (ir & 1) fC = set != 0;
				if (ir & 2) fV = set != 0;
				if (ir & 4) fZ = set != 0;
				if (ir & 010) fN = set != 0;
				return;
			}
			break;
		case 3: /* SWAB */
			getop(dm, dr, 0);
			d = load(0);
			r = d >> 8 | d << 8;
			store(r, 0);
			fN = (r & 0x80) != 0;
			fZ = (r & 0xff) == 0;
			fV = fC = 0;
			return;
		}
	}

	if (!byte && (ir >> 8) >= 001 && (ir >> 8) <= 003) { /* BR..BLE handled below via branch table */ }
	{
		int hi = (ir >> 8) & 0377;
		/* branches: 0004xx-0003xx and 1000xx-1034xx */
		switch (ir & 0177400) {
		case 0000400: branch(1, ir); return;
		case 0001000: branch(!fZ, ir); return;
		case 0001400: branch(fZ, ir); return;
		case 0002000: branch(!(fN ^ fV), ir); return;
		case 0002400: branch(fN ^ fV, ir); return;
		case 0003000: branch(!(fZ | (fN ^ fV)), ir); return;
		case 0003400: branch(fZ | (fN ^ fV), ir); return;
		case 0100000: branch(!fN, ir); return;
		case 0100400: branch(fN, ir); return;
		case 0101000: branch(!(fC | fZ), ir); return;
		case 0101400: branch(fC | fZ, ir); return;
		case 0102000: branch(!fV, ir); return;
		case 0102400: branch(fV, ir); return;
		case 0103000: branch(!fC, ir); return;
		case 0103400: branch(fC, ir); return;
		case 0104400: v4sys(ir & 0377); return; /* TRAP */
		}
		(void)hi;
	}

	if (!byte && (ir & 0177000) == 0004000) { /* JSR */
		int rg = (ir >> 6) & 7;
		getop(dm, dr, 0);
		if (regdst >= 0) die("JSR to register");
		d = ea;
		SP -= 2;
		wrw(SP, R[rg]);
		R[rg] = PC;
		PC = d;
		return;
	}

	/* single operand group (byte flag selects b forms): 0050xx-0063xx, 0067xx */
	{
		int sub = (ir >> 6) & 077;
		uint16_t v;
		int c;
		switch (sub) {
		case 050:
			getop(dm, dr, byte); store(0, byte); fN = 0; fZ = 1; fV = fC = 0; return;
		case 051:
			getop(dm, dr, byte); v = load(byte); v = ~v; store(v, byte); nz(v, byte); fV = 0; fC = 1; return;
		case 052:
			getop(dm, dr, byte); v = load(byte);
			{ uint16_t m = byte ? 0x7f : 0x7fff; fV = (v & (byte ? 0xff : 0xffff)) == m; }
			v++; store(v, byte); nz(v, byte); return;
		case 053:
			getop(dm, dr, byte); v = load(byte);
			{ uint16_t m = byte ? 0x80 : 0x8000; fV = (v & (byte ? 0xff : 0xffff)) == m; }
			v--; store(v, byte); nz(v, byte); return;
		case 054:
			getop(dm, dr, byte); v = load(byte);
			{ uint16_t m = byte ? 0x80 : 0x8000; fV = (v & (byte ? 0xff : 0xffff)) == m; }
			v = -v; store(v, byte); nz(v, byte); fC = (v & (byte ? 0xff : 0xffff)) != 0; return;
		case 055:
			getop(dm, dr, byte); v = load(byte); c = fC;
			{ uint16_t m = byte ? 0x7f : 0x7fff; fV = c && (v & (byte ? 0xff : 0xffff)) == m; fC = c && ((v & (byte ? 0xff : 0xffff)) == (byte ? 0xff : 0xffff)); }
			v += c; store(v, byte); nz(v, byte); return;
		case 056:
			getop(dm, dr, byte); v = load(byte); c = fC;
			{ uint16_t m = byte ? 0x80 : 0x8000; fV = (v & (byte ? 0xff : 0xffff)) == m; fC = (v & (byte ? 0xff : 0xffff)) == 0 && c; }
			v -= c; store(v, byte); nz(v, byte); return;
		case 057:
			getop(dm, dr, byte); v = load(byte); nz(v, byte); fV = fC = 0; return;
		case 060:
			getop(dm, dr, byte); v = load(byte);
			{ uint16_t sb = byte ? 0x80 : 0x8000; int nc = v & 1; v = (v & (byte ? 0xff : 0xffff)) >> 1; if (fC) v |= sb;
			  store(v, byte); nz(v, byte); fC = nc; fV = fN ^ fC; } return;
		case 061:
			getop(dm, dr, byte); v = load(byte);
			{ uint16_t sb = byte ? 0x80 : 0x8000; int nc = (v & sb) != 0; v = v << 1; if (fC) v |= 1;
			  store(v, byte); nz(v, byte); fC = nc; fV = fN ^ fC; } return;
		case 062:
			getop(dm, dr, byte); v = load(byte);
			{ uint16_t sb = byte ? 0x80 : 0x8000; int nc = v & 1; v = (v & (byte ? 0xff : 0xffff)) >> 1 | (v & sb);
			  store(v, byte); nz(v, byte); fC = nc; fV = fN ^ fC; } return;
		case 063:
			getop(dm, dr, byte); v = load(byte);
			{ uint16_t sb = byte ? 0x80 : 0x8000; int nc = (v & sb) != 0; v = v << 1;
			  store(v, byte); nz(v, byte); fC = nc; fV = fN ^ fC; } return;
		case 067:
			if (!byte) { /* SXT */
				getop(dm, dr, 0); store(fN ? 0xffff : 0, 0); fZ = !fN; fV = 0; return;
			}
		}
	}
	die("illegal instruction");
}
