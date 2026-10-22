#
/* 
 * modern alloc functions for unix v4 userspace.
 * copyright 2026 daniel mcguire.
 * 
 * not safe to use with other manual allocators!
 * 
 * requires unmodified CC from unix v4 to compile:
 * cc allocator.c YOURFILE.c
 */
#define NULL    0
#define MINBLK	4
#define CHUNK	1024
#define KEEP	512
#define TRIM	2048
#define MAXREQ	32000

#define MC_OK           0
#define MC_BAD_ALIGN    1
#define MC_OUT_OF_HEAP  2
#define MC_BAD_SIZE     3
#define MC_ORDER        4
#define MC_NOT_COALESC  5
#define MC_PAST_END     6

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
	while ((p = q->mb_next) != NULL && p < b)
		q = p;

	e = b;
	e = e + b->mb_size;
	if (p != NULL && e > p)
		return(-1);
	if (q != &m_free) {
		f = q;
		f = f + q->mb_size;
		if (f > b)
			return(-1);
	}
	if (p != NULL && e == p) {
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
	int r;

	if (n < CHUNK)
		n = CHUNK;
	n = (n + 63) & ~63;

	if (m_hend == 0) {
		e = sbrk(0);
		i = e;
		if (i == -1)
			return(0);
		if (i & 1) {
			i = i + 1;
			r = brk(i);
			if (r == -1)
				return(0);
			e = e + 1;
		}
		m_hlo = e;
		m_hend = e;
	}

	a = m_hend;
	if (a + n < a)
		return(0);
	r = brk(a + n);
	if (r == -1)
		return(0);
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
		for (q = &m_free; (p = q->mb_next) != NULL; q = p) {
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
	char *sz;
	int rel;
	int r;

	b = nb;
	while (b->mb_size >= TRIM) {
		e = b;
		e = e + b->mb_size;
		if (e != m_hend)
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
		r = brk(m_hend - rel);
		if (r == -1)
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

	if (a == NULL)
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

	if (a == NULL)
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

char *
calloc(n, size)
int n, size;
{
	register char *p;
	register int i;
	int total;

	if (n <= 0 || size <= 0)
		return(0);

	if (n > MAXREQ / size)
		return(0);

	total = n * size;

	p = malloc(total);
	if (p == NULL)
		return(NULL);

	for (i=0;i<total;i++)
		p[i] = 0;

	return(p);
}

mcheck()
{
    register struct mblk *p;
    register char *e, *last;
    int i;

    last = 0;
    for (p = m_free.mb_next; p != NULL; p = p->mb_next) {
        e = p;
        i = e;
        if (i & 1)
            return(MC_BAD_ALIGN);
        if (e < m_hlo || e >= m_hend)
            return(MC_OUT_OF_HEAP);
        i = p->mb_size;
        if (i < MINBLK || (i & 1))
            return(MC_BAD_SIZE);
        if (last != NULL && e < last)
            return(MC_ORDER);
        if (last != NULL && e == last)
            return(MC_NOT_COALESC);
        last = e + p->mb_size;
        if (last > m_hend)
            return(MC_PAST_END);
    }
    return(MC_OK);
}

mfreebytes()
{
	register struct mblk *p;
	register int n;

	n = 0;
	for (p = m_free.mb_next; p!=NULL; p = p->mb_next)
		n = n + p->mb_size;
	return(n);
}

strlen(str)
char *str;
{
	register int l;
	l=0;

	while(str[l]) l++;

	return l;
}

char*
memcpy(dst, src, n)
char *dst;
char *src;
{
	register char *ret;
	register int i;

	if (dst == NULL || src == NULL)
		return NULL;

	ret = dst;

	for(i=0;i<n;i++)
		*dst++ = *src++;

	return ret;
}

char*
strcpy(dst, src)
char *dst;
char *src;
{
	register int l;

	if (dst == NULL || src == NULL) 
		return NULL;

	l=strlen(src);
	l++;

	memcpy(dst, src, l);
	return dst;
}

char*
strdup(src)
char *src;
{
	register char *dst;
	register int l;

	if (src == NULL) return NULL;

	l=strlen(src);
	l++;

	dst = malloc(l);
	if (dst == NULL) return NULL;
	memcpy(dst, src, l);

	return dst;
}
