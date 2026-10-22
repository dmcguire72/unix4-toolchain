#define _CRT_SECURE_NO_WARNINGS
#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <cstring>
#include <csignal>

static const char *knowledge_file = "ttt.k";

#define KBUF_WORDS 500
#define STM_WORDS  16

static char      board[9];
static uint16_t  stm[STM_WORDS];
static uint16_t *stp = stm;
static uint16_t  kbuf[KBUF_WORDS];
static uint16_t *kend = kbuf;
static int       cnt, best;
static char     *bestc;

static const uint8_t perms[8][9] = {
    {0,1,2,3,4,5,6,7,8},
    {6,3,0,7,4,1,8,5,2},
    {8,7,6,5,4,3,2,1,0},
    {2,5,8,1,4,7,0,3,6},
    {2,1,0,5,4,3,8,7,6},
    {8,5,2,7,4,1,6,3,0},
    {6,7,8,3,4,5,0,1,2},
    {0,3,6,1,4,7,2,5,8},
};

static const uint8_t lines[8][3] = {
    {0,1,2},{3,4,5},{6,7,8},
    {0,3,6},{1,4,7},{2,5,8},
    {0,4,8},{2,4,6},
};

static size_t write_le16(FILE *f, const uint16_t *buf, size_t count) {
    unsigned char tmp[1024];
    size_t words_done = 0;
    while (words_done < count) {
        size_t chunk = count - words_done;
        if (chunk > sizeof(tmp) / 2) chunk = sizeof(tmp) / 2;
        for (size_t i = 0; i < chunk; i++) {
            tmp[i*2]     = (unsigned char)( buf[i]        & 0xFF);
            tmp[i*2 + 1] = (unsigned char)((buf[i] >> 8)  & 0xFF);
        }
        size_t written = fwrite(tmp, 2, chunk, f);
        words_done += written;
        if (written < chunk) break;
        buf += chunk;
    }
    return words_done;
}

static size_t read_le16(FILE *f, uint16_t *buf, size_t max_words) {
    unsigned char tmp[1024];
    size_t words_done = 0;
    while (words_done < max_words) {
        size_t chunk = max_words - words_done;
        if (chunk > sizeof(tmp) / 2) chunk = sizeof(tmp) / 2;
        size_t got = fread(tmp, 1, chunk * 2, f);
        size_t full = got / 2;
        for (size_t i = 0; i < full; i++)
            buf[words_done + i] = (uint16_t)(tmp[i*2] | (tmp[i*2 + 1] << 8));
        words_done += full;
        if (got < chunk * 2) break;
    }
    return words_done;
}

static void putc_(int c) { putchar(c); }
static void nl_(void)   { putchar('\n'); }
static void puts_(const char *s) { fputs(s, stdout); fflush(stdout); }

static void putnum(int n) {
    if (n >= 10) putnum(n / 10);
    putc_('0' + n % 10);
}

static int getc_(void) {
    int c = getchar();
    if (c == EOF) std::exit(12);
    return c;
}

static void skipnl(void) { int c; do { c = getc_(); } while (c != '\n'); }

static int gety(void) {
    int c = getc_();
    int y = (c == 'y' || c == 'Y');
    if (c != '\n') skipnl();
    return y;
}

static int hash_board(void) {
    int m = 0;
    for (int p = 0; p < 8; p++) {
        int h = 0;
        for (int i = 0; i < 9; i++)
            h = h * 3 + board[perms[p][i]];
        if (h > m) m = h;
    }
    return m;
}

static int wcheck(int target) {
    for (int l = 0; l < 8; l++) {
        int sum = 0;
        for (int k = 0; k < 3; k++) {
            int c = board[lines[l][k]];
            if (c == 0) c = 10;
            sum += c;
        }
        if (sum == target) return 1;
    }
    return 0;
}

static int fullchk(void) {
    for (int i = 0; i < 9; i++)
        if (board[i] == 0) return 0;
    return 1;
}

static void bye(void) {
    FILE *f = fopen(knowledge_file, "rb");
    if (f) {
        size_t room = (size_t)(kbuf + KBUF_WORDS - kend);
        if (room > 0) {
            kend += read_le16(f, kend, room);
        }
        fclose(f);
    }

    uint16_t *r1 = kbuf, *r2 = kbuf;
    for (;;) {
        if (r2 >= kbuf + KBUF_WORDS) break;
        
        int16_t v = (int16_t)*r2++;
        if (v == 0) break;
        if (v < 0)  continue;
        for (uint16_t *r3 = kbuf; r3 < r2; r3++) {
            if (*r3 == 0) break;
            if (*r3 == (uint16_t)v) { r3[0] = 0xFFFF; }
        }
        *r1++ = (uint16_t)v;
    }
    
    if (r1 < kbuf + KBUF_WORDS) {
        *r1 = 0;
    }

    size_t words = (size_t)(r1 - kbuf);
    nl_();
    putnum((int)words);
    puts_(" 'bits' returned\n");

    f = fopen(knowledge_file, "wb");
    if (f) {
        write_le16(f, kbuf, words);
        fclose(f);
    }
    std::exit(12);
}

static void sigint_handler(int sig) { (void)sig; bye(); }

static void game(void) {
newgame:
    puts_("new game\n");
    stp = stm;
    memset(board, 0, sizeof board);

show:
    for (int r = 0; r < 3; r++) {
        for (int c = 0; c < 3; c++) {
            int i = r * 3 + c, v = board[i];
            if      (v == 0) putc_('1' + i);
            else if (v == 1) putc_('O');
            else             putc_('X');
        }
        nl_();
    }
    nl_();
    puts_("? ");

    int ch = getc_();
    if (ch != '\n') {
        int idx = ch - '1';
        if (idx < 0 || idx > 8) goto illegal;
        if (board[idx] != 0)    goto illegal;
        int ch2 = getc_();
        if (ch2 != '\n')        goto illegal;
        board[idx] = 2;
    }

    if (wcheck(6)) goto youwin;
    if (fullchk()) { puts_("Draw\n"); goto newgame; }

    cnt = 0; best = 0; bestc = NULL;
    for (int i = 0; i < 9; i++) {
        if (board[i] != 0) continue;
        board[i] = 1;
        if (wcheck(3)) { goto iwin; }
        int h = hash_board();
        board[i] = 0;

        int known = 0;
        for (uint16_t *p = kbuf; p < kend; p++)
            if (*p == (uint16_t)h) { known = 1; break; }
        if (known) continue;
        if (h == best) continue;
        if (h > best) { best = h; bestc = &board[i]; }
        cnt++;
    }
    if (cnt == 0) goto concede;
    if (cnt != 1) stp = stm;
    *stp++ = (uint16_t)best;
    if (bestc) *bestc = 1;
    goto show;

illegal:
    puts_("Illegal move\n");
    skipnl();
    goto show;

youwin:
    puts_("You win\n");
    goto learn;

iwin:
    puts_("I win\n");
    goto newgame;

concede:
    puts_("I concede\n");
    goto learn;

learn:
    {
        uint16_t *p = stm;
        while (p < stp && kend < kbuf + KBUF_WORDS)
            *kend++ = *p++;
    }
    goto newgame;
}

int main(int argc, char *argv[]) {
    const char *env_kf = getenv("TTT_KNOWLEDGE_FILE");
    if (argc > 1) {
        knowledge_file = argv[1];
    } else if (env_kf) {
        knowledge_file = env_kf;
    }

    puts_("Tic-Tac-Toe\n");
    puts_("Accumulated knowledge? ");

    if (gety()) {
        FILE *f = fopen(knowledge_file, "rb");
        if (f) {
            read_le16(f, kbuf, KBUF_WORDS);
            fclose(f);
        }
    }

    uint16_t *p = kbuf;
    while (p < kbuf + KBUF_WORDS && *p) p++;
    kend = p;

    putnum((int)(kend - kbuf));
    puts_(" 'bits' of knowledge\n");

    std::signal(SIGINT, sigint_handler);

    game();
    return 0;
}