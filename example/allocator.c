#define MINBLK	4
#define CHUNK	1024
#define KEEP	512
#define TRIM	2048
#define MAXREQ	32000

struct mblk {
	char *mb_size;
	struct mblk *mb_next;
};

struct mblk m_free;
char *m_hlo;
char *m_hend;

m_insert(nb)
struct mblk *nb;
{
	register struct mblk *p, *q, *b;
	char *e, *f;

	b = nb;
	q = &m_free;
	while ((p = q->mb_next) != 0 && p < b)
		q = p;

	e = b;
	e = e + b->mb_size;
	if (p != 0 && e > p)
		return(-1);
	if (q != &m_free) {
		f = q;
		f = f + q->mb_size;
		if (f > b)
			return(-1);
	}
	if (p != 0 && e == p) {
		b->mb_size = b->mb_size + p->mb_size;
		b->mb_next = p->mb_next;
	} else
		b->mb_next = p;
	f = q;
	f = f + q->mb_size;
	if (q != &m_free && f == b) {
		q->mb_size = q->mb_size + b->mb_size;
		q->mb_next = b->mb_next;
	} else
		q->mb_next = b;
	return(0);
}

m_grow(n)
int n;
{
	register char *a, *e;
	register struct mblk *b;
	int i;

	if (n < CHUNK)
		n = CHUNK;
	n = (n + 63) & ~63;
	e = sbrk(0);
	i = e;
	if (i & 1) {
		if (sbrk(1) == -1)
			return(0);
		e = e + 1;
	}
	if (e + n < e)
		return(0);
	a = sbrk(n);
	if (a == -1)
		return(0);
	if (m_hlo == 0 || a < m_hlo)
		m_hlo = a;
	if (a + n > m_hend)
		m_hend = a + n;
	b = a;
	b->mb_size = n;
	m_insert(b);
	return(1);
}

char *malloc(n)
int n;
{
	register struct mblk *p, *q;
	register char *sz;
	struct mblk *t;
	char *r;

	if (n <= 0 || n > MAXREQ)
		return(0);
	sz = (n + 3) & ~1;
	if (sz < MINBLK)
		sz = MINBLK;
	for (;;) {
		for (q = &m_free; (p = q->mb_next) != 0; q = p) {
			if (p->mb_size < sz)
				continue;
			if (p->mb_size - sz >= MINBLK) {
				r = p;
				r = r + sz;
				t = r;
				t->mb_size = p->mb_size - sz;
				t->mb_next = p->mb_next;
				q->mb_next = t;
				p->mb_size = sz;
			} else
				q->mb_next = p->mb_next;
			return(&p->mb_next);
		}
		if (m_grow(sz) == 0)
			return(0);
	}
}

m_trim(nb)
struct mblk *nb;
{
	register struct mblk *b;
	register char *e;
	char *cur, *sz;
	int rel;

	b = nb;
	while (b->mb_size >= TRIM) {
		e = b;
		e = e + b->mb_size;
		if (e != m_hend)
			return;
		cur = sbrk(0);
		if (cur != m_hend)
			return;
		sz = b->mb_size - KEEP;
		if (sz > 16384)
			rel = 16384;
		else {
			rel = sz;
			rel = rel & ~63;
		}
		if (rel <= 0)
			return;
		if (sbrk(0 - rel) == -1)
			return;
		b->mb_size = b->mb_size - rel;
		m_hend = m_hend - rel;
	}
}

free(a)
char *a;
{
	register struct mblk *b;
	register char *e;
	int i;

	if (a == 0)
		return(0);
	e = a - 2;
	i = e;
	if ((i & 1) || e < m_hlo || e >= m_hend)
		return(-1);
	b = e;
	i = b->mb_size;
	if (i < MINBLK || (i & 1) || e + b->mb_size > m_hend)
		return(-1);
	if (m_insert(b) < 0)
		return(-1);

	b = &m_free;
	while (b->mb_next != 0)
		b = b->mb_next;
	if (b != &m_free)
		m_trim(b);
	return(0);
}

char *realloc(a, n)
char *a;
int n;
{
	register struct mblk *b;
	register char *p, *q;
	char *e;
	int cap, i;

	if (a == 0)
		return(malloc(n));
	if (n <= 0) {
		free(a);
		return(0);
	}
	e = a - 2;
	b = e;
	cap = b->mb_size - 2;
	if (n <= cap)
		return(a);
	p = malloc(n);
	if (p == 0)
		return(0);
	q = a;
	for (i = 0; i < cap; i++)
		p[i] = q[i];
	free(a);
	return(p);
}

char *calloc(n, size)
int n, size;
{
	register char *p;
	register int i;

	i = n * size;
	if (n > 0 && i / n != size)
		return(0);
	p = malloc(i);
	if (p == 0)
		return(0);
	while (i > 0)
		p[--i] = 0;
	return(p);
}

mcheck()
{
	register struct mblk *p;
	register char *e, *last;
	int i;

	last = 0;
	for (p = m_free.mb_next; p != 0; p = p->mb_next) {
		e = p;
		i = e;
		if (i & 1)
			return(1);
		if (e < m_hlo || e >= m_hend)
			return(2);
		i = p->mb_size;
		if (p->mb_size < MINBLK || (i & 1))
			return(3);
		if (last != 0 && e <= last)
			return(4);
		if (last != 0 && e == last)
			return(5);
		last = e + p->mb_size;
		if (last > m_hend)
			return(6);
		if (p->mb_next != 0 && p->mb_next == last)
			return(5);
	}
	return(0);
}

mfreebytes()
{
	register struct mblk *p;
	register int n;

	n = 0;
	for (p = m_free.mb_next; p != 0; p = p->mb_next)
		n = n + p->mb_size;
	return(n);
}

char*
memcpy(dst, src, n)
char *dst;
char *src;
{
	char *ret;
	int i;

	ret = dst;

	for(i=0;i<n;i++)
		*dst++ = *src++;

	return ret;
}
