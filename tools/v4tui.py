from __future__ import annotations

import collections
import curses
import locale
import os
import shutil
import sys
import textwrap
import threading
import time

PENDING, RUNNING, DONE, WARN, FAILED, SKIPPED = (
    "pending", "running", "done", "warn", "failed", "skipped")
FINISHED = (DONE, WARN, SKIPPED)

def terminal_is_utf8() -> bool:
    if os.environ.get("TERM") == "linux":
        return False
    try:
        codeset = locale.nl_langinfo(locale.CODESET)
    except (AttributeError, ValueError):
        return False
    return codeset.upper().replace("-", "") == "UTF8"

def human_bytes(n: float) -> str:
    n = float(n)
    for unit in ("B", "KB", "MB", "GB"):
        if n < 1024 or unit == "GB":
            return f"{n:.0f} {unit}" if unit == "B" else f"{n:.1f} {unit}"
        n /= 1024
    return f"{n:.1f} GB"

def human_time(seconds: float) -> str:
    if seconds < 0:
        seconds = 0.0

    if seconds < 1.0:
        return f"{seconds * 1000:.2f}ms"

    if seconds < 2.0:
        return f"{seconds:.2f}s"

    secs = int(seconds)
    m, s = divmod(secs, 60)
    h, m = divmod(m, 60)
    return f"{h}:{m:02d}:{s:02d}" if h else f"{m}:{s:02d}"

def ascii_bar(frac: float | None, width: int, full: str, empty: str) -> str:
    frac = 0.0 if frac is None else min(1.0, max(0.0, frac))
    filled = int(round(frac * width))
    return full * filled + empty * (width - filled)

class Step:
    def __init__(self, key: str, title: str, weight: float = 1.0):
        self.key = key
        self.title = title
        self.weight = weight
        self.status = PENDING
        self.frac: float | None = None
        self.detail = ""
        self.note = ""
        self.t0 = 0.0
        self.t1 = 0.0

    def elapsed(self) -> float:
        if not self.t0:
            return 0.0
        return (self.t1 or time.monotonic()) - self.t0

class Model:
    def __init__(self, title: str, subtitle: str, steps):
        self.title = title
        self.subtitle = subtitle
        self.lock = threading.RLock()
        self.steps = [Step(*s) for s in steps]
        self._by_key = {s.key: s for s in self.steps}
        self.current: Step | None = None
        self.logbuf = collections.deque(maxlen=500)
        self.logfile = None
        self.t0 = time.monotonic()
        self.t1 = 0.0
        self.cancelled = False
        self.result: str | None = None
        self.error = ""
        self.failed_step: Step | None = None
        self.summary: list[tuple[str, str]] = []
        self.footer: list[str] = []
        self.warnings: list[str] = []
        self.listeners = []

    def begin(self, key: str):
        with self.lock:
            s = self._by_key[key]
            s.status, s.t0, s.frac, s.detail = RUNNING, time.monotonic(), None, ""
            self.current = s
            self.add_log(f"== {s.title}")
        self._emit("on_begin", s)

    def update(self, frac: float | None = None, detail: str = ""):
        with self.lock:
            s = self.current
            if s is None:
                return
            s.frac, s.detail = frac, detail
        self._emit("on_update", s)

    def end(self, status: str = DONE, note: str = ""):
        with self.lock:
            s = self.current
            if s is None:
                return
            s.status, s.note, s.t1 = status, note, time.monotonic()
            if status == FAILED:
                self.failed_step = s
            s.frac = 1.0 if status in FINISHED else s.frac
            self.current = None
            if note:
                self.add_log(f"   {note}")
        self._emit("on_end", s)

    def skip(self, key: str, note: str = ""):
        with self.lock:
            s = self._by_key[key]
            s.status, s.note = SKIPPED, note
            self.add_log(f"== {s.title} (skipped{': ' + note if note else ''})")
        self._emit("on_skip", s)

    def warn(self, text: str):
        with self.lock:
            self.warnings.append(text)
            self.add_log(f"warning: {text}")

    def add_log(self, text: str):
        with self.lock:
            self.logbuf.append(text)
            if self.logfile:
                try:
                    stamp = time.strftime("%H:%M:%S")
                    self.logfile.write(f"{stamp} {text}\n")
                    self.logfile.flush()
                except OSError:
                    self.logfile = None
        self._emit("on_log", text)

    def finish(self, result: str, error: str = ""):
        with self.lock:
            self.result, self.error, self.t1 = result, error, time.monotonic()
        self._emit("on_finish", result)

    def cancel(self):
        self.cancelled = True

    def overall(self) -> float:
        with self.lock:
            total = sum(s.weight for s in self.steps) or 1.0
            done = sum(s.weight for s in self.steps if s.status in FINISHED)
            cur = self.current
            if cur is not None and cur.frac is not None:
                done += cur.weight * min(1.0, max(0.0, cur.frac))
            return min(1.0, done / total)

    def elapsed(self) -> float:
        return (self.t1 or time.monotonic()) - self.t0

    def index_of(self, step: Step) -> int:
        return self.steps.index(step) + 1

    def _emit(self, name: str, *args):
        for listener in self.listeners:
            fn = getattr(listener, name, None)
            if fn:
                fn(self, *args)

def format_result(model: Model, uni: bool) -> list[str]:
    ok, bad, wrn = ("✓", "✗", "!") if uni else ("OK", "FAILED", "!")
    out: list[str] = []
    if model.result == "ok":
        out.append(f"{ok} Installation complete in {human_time(model.elapsed())}")
        if model.summary:
            out.append("")
            width = max(len(k) for k, _ in model.summary)
            out += [f"  {k.ljust(width)}  {v}" for k, v in model.summary]
    elif model.result == "cancelled":
        out.append("Installation cancelled.")
    else:
        step = model.failed_step
        where = f" while running '{step.title}'" if step else ""
        out.append(f"{bad} Installation failed{where}")
        if model.error:
            out.append("")
            out += ["  " + line for line in textwrap.wrap(model.error, 76)]
    if model.warnings:
        out.append("")
        out += [f"  {wrn} {w}" for w in model.warnings]
    if model.footer:
        out.append("")
        out += model.footer
    return out

class PlainUI:
    uni = False

    def __init__(self, verbose: bool = False):
        self.verbose = verbose
        self.tty = sys.stdout.isatty() and os.environ.get("TERM", "dumb") != "dumb"
        self._last_draw = 0.0
        self._line_open = False
        self.uni = terminal_is_utf8()

    def __enter__(self):
        return self

    def __exit__(self, *exc):
        return False

    def menu(self, title, subtitle, options, default=0):
        print(f"\n{title}")
        if subtitle:
            print(f"  {subtitle}")
        for i, (label, desc) in enumerate(options, 1):
            print(f"  {i} - {label}" + (f"  ({desc})" if desc else ""))
        for _ in range(3):
            try:
                raw = input(f"Selection [1-{len(options)}, default {default + 1}]: ").strip()
            except (EOFError, KeyboardInterrupt):
                print()
                return None
            if not raw:
                return default
            if raw.isdigit() and 1 <= int(raw) <= len(options):
                return int(raw) - 1
            print("  Not a valid choice.")
        return None

    def run(self, model: Model, work):
        model.listeners.append(self)
        try:
            work()
        finally:
            self._close_line()

    def _width(self):
        return max(40, shutil.get_terminal_size((80, 24)).columns - 1)

    def _close_line(self):
        if self._line_open:
            sys.stdout.write("\n")
            self._line_open = False
        sys.stdout.flush()

    def _prefix(self, model, step):
        return f"[{model.index_of(step)}/{len(model.steps)}] {step.title}"

    def on_begin(self, model, step):
        self._close_line()
        if self.tty and not self.verbose:
            self._draw(model, step, force=True)
        elif self.verbose:
            print(self._prefix(model, step) + " ...", flush=True)
        else:
            sys.stdout.write(self._prefix(model, step) + " ... ")
            sys.stdout.flush()
            self._line_open = True

    def on_update(self, model, step):
        if self.tty and not self.verbose:
            self._draw(model, step)

    def on_end(self, model, step):
        tag = {DONE: "done", WARN: "done with warnings", FAILED: "FAILED"}.get(step.status, step.status)
        note = f" ({step.note})" if step.note else ""
        timing = f" [{human_time(step.elapsed())}]"
        if self.tty and not self.verbose:
            line = f"{self._prefix(model, step)} ... {tag}{note}{timing}"
            sys.stdout.write("\r\033[K" + line[: self._width()] + "\n")
        elif self.verbose:
            print(f"{self._prefix(model, step)} ... {tag}{note}{timing}", flush=True)
        else:
            sys.stdout.write(f"{tag}{note}{timing}\n")
        sys.stdout.flush()
        self._line_open = False

    def on_skip(self, model, step):
        self._close_line()
        note = f" ({step.note})" if step.note else ""
        print(f"[{model.index_of(step)}/{len(model.steps)}] {step.title} ... skipped{note}", flush=True)

    def on_log(self, model, text):
        if not self.verbose or text.startswith("=="):
            return
        self._close_line()
        print("    " + text, flush=True)

    def on_finish(self, model, result):
        self._close_line()

    def _draw(self, model, step, force=False):
        now = time.monotonic()
        if not force and now - self._last_draw < 0.1:
            return
        self._last_draw = now
        width = self._width()
        head = self._prefix(model, step)
        if step.frac is None:
            bar = ""
            pct = ""
        else:
            bar = "[" + ascii_bar(step.frac, 20, "#", "-") + "] "
            pct = f"{int(step.frac * 100):3d}%  "
        line = f"{head}  {bar}{pct}{step.detail}"
        sys.stdout.write("\r\033[K" + line[:width])
        sys.stdout.flush()
        self._line_open = True

class CursesUI:
    MIN_ROWS, MIN_COLS = 12, 50

    def __init__(self):
        self.scr = None
        self.uni = False
        self.colors = {}

    def __enter__(self):
        locale.setlocale(locale.LC_ALL, "")
        os.environ.setdefault("ESCDELAY", "25")
        self.scr = curses.initscr()
        try:
            curses.noecho()
            curses.cbreak()
            self.scr.keypad(True)
            try:
                curses.curs_set(0)
            except curses.error:
                pass
            self.uni = terminal_is_utf8()
            self._init_colors()
        except Exception:
            curses.endwin()
            raise
        return self

    def __exit__(self, *exc):
        try:
            self.scr.keypad(False)
            curses.nocbreak()
            curses.echo()
        finally:
            curses.endwin()
        return False

    def _init_colors(self):
        names = ("ok", "bad", "warn", "accent")
        self.colors = {n: 0 for n in names}
        if not curses.has_colors():
            return
        curses.start_color()
        try:
            curses.use_default_colors()
            bg = -1
        except curses.error:
            bg = curses.COLOR_BLACK
        for i, (name, fg) in enumerate(zip(names, (
                curses.COLOR_GREEN, curses.COLOR_RED,
                curses.COLOR_YELLOW, curses.COLOR_CYAN)), 1):
            curses.init_pair(i, fg, bg)
            self.colors[name] = curses.color_pair(i)

    def _put(self, y, x, text, attr=0):
        h, w = self.scr.getmaxyx()
        if y < 0 or y >= h or x >= w - 1 or not text:
            return
        text = text[: w - x - 1]
        try:
            self.scr.addstr(y, x, text, attr)
        except curses.error:
            pass

    def _glyphs(self):
        if self.uni:
            return dict(full="█", empty="░", rule="─", ok="✔", bad="✘", warn="!",
                        run="▶", pend="○", skip="–", sel="›",
                        spin="⠋⠙⠹⠸⠼⠴⠦⠧⠇⠏")
        return dict(full="#", empty="-", rule="-", ok="+", bad="x", warn="!",
                    run=">", pend=".", skip="-", sel=">", spin="|/-\\")

    def _clip(self, text, width):
        if len(text) <= width:
            return text
        return text[: max(0, width - 1)] + ("…" if self.uni else "~")

    def menu(self, title, subtitle, options, default=0):
        g = self._glyphs()
        sel = default
        self.scr.timeout(-1)
        self.scr.clear()
        while True:
            self.scr.erase()
            h, w = self.scr.getmaxyx()
            y = 1
            self._put(y, 2, title, curses.A_BOLD | self.colors["accent"])
            y += 2
            if subtitle:
                for line in textwrap.wrap(subtitle, max(20, w - 6)):
                    self._put(y, 2, line)
                    y += 1
                y += 1
            for i, (label, desc) in enumerate(options):
                attr = curses.A_REVERSE | curses.A_BOLD if i == sel else 0
                marker = g["sel"] if i == sel else " "
                self._put(y, 2, self._clip(f" {marker} {i + 1}. {label}".ljust(w - 6), w - 5), attr)
                y += 1
                if desc:
                    for line in textwrap.wrap(desc, max(20, w - 12)):
                        self._put(y, 8, line, curses.A_DIM)
                        y += 1
                y += 0
            self._put(h - 2, 2, "↑/↓ select   Enter confirm   q cancel" if self.uni
                      else "Up/Down select   Enter confirm   q cancel", curses.A_DIM)
            self.scr.refresh()
            try:
                ch = self.scr.getch()
            except KeyboardInterrupt:
                return None
            if ch in (curses.KEY_UP, ord("k")):
                sel = (sel - 1) % len(options)
            elif ch in (curses.KEY_DOWN, ord("j")):
                sel = (sel + 1) % len(options)
            elif ord("1") <= ch <= ord("9") and ch - ord("1") < len(options):
                sel = ch - ord("1")
            elif ch in (curses.KEY_ENTER, 10, 13):
                return sel
            elif ch in (ord("q"), ord("Q"), 27, 3):
                return None

    def run(self, model: Model, work):
        crash: list[BaseException] = []

        def target():
            try:
                work()
            except BaseException as exc:
                crash.append(exc)

        worker = threading.Thread(target=target, daemon=True)
        self.scr.clear()
        worker.start()
        self.scr.timeout(100)
        while worker.is_alive():
            self._draw(model)
            try:
                self.scr.getch()
            except KeyboardInterrupt:
                model.cancel()
        worker.join()
        if model.result is None:
            model.finish("failed", f"internal error: {crash[0]!r}" if crash else "installer stopped unexpectedly")
        self.scr.timeout(-1)
        while True:
            self._draw(model, final=True)
            try:
                ch = self.scr.getch()
            except KeyboardInterrupt:
                break
            if ch != curses.KEY_RESIZE:
                break
            self.scr.clear()

    def _status_attr(self, status):
        return {DONE: self.colors["ok"], WARN: self.colors["warn"],
                FAILED: self.colors["bad"], RUNNING: self.colors["accent"],
                SKIPPED: curses.A_DIM, PENDING: curses.A_DIM}.get(status, 0)

    def _draw(self, m: Model, final=False):
        g = self._glyphs()
        scr = self.scr
        scr.erase()
        h, w = scr.getmaxyx()
        if h < self.MIN_ROWS or w < self.MIN_COLS:
            self._put(0, 0, "Terminal too small, enlarge it to see progress.")
            self._put(1, 0, f"Total: {int(m.overall() * 100)}%")
            scr.refresh()
            return

        with m.lock:
            result = m.result
            if result == "ok":
                status, attr = "installation complete", self.colors["ok"] | curses.A_BOLD
            elif result == "failed":
                status, attr = "installation failed", self.colors["bad"] | curses.A_BOLD
            elif result == "cancelled":
                status, attr = "cancelled", self.colors["warn"] | curses.A_BOLD
            elif m.cancelled:
                status, attr = "cancelling...", self.colors["warn"] | curses.A_BOLD
            else:
                status, attr = "installing...", self.colors["accent"] | curses.A_BOLD
            self._put(0, 2, m.title, curses.A_BOLD)
            self._put(0, w - 2 - len(status), status, attr)
            y = 1
            if m.subtitle and h >= 16:
                self._put(1, 2, self._clip(m.subtitle, w - 6), curses.A_DIM)
                y = 2
            y += 1

            frac = m.overall()
            tail = f" {int(frac * 100):3d}%  {human_time(m.elapsed())}"
            bar_w = max(10, w - 6 - len(tail) - 10)
            self._put(y, 2, "Total ", curses.A_DIM)
            bar_attr = self.colors["bad"] if result == "failed" else self.colors["ok"] if result == "ok" else self.colors["accent"]
            self._put(y, 10, ascii_bar(frac, bar_w, g["full"], g["empty"]), bar_attr)
            self._put(y, 10 + bar_w, tail)
            y += 2

            if result != "ok":
                reserve = 0
                if result in ("failed", "cancelled"):
                    msg = m.error or "Cancelled."
                    reserve = min(len(textwrap.wrap(msg, max(20, w - 8))), 3) + 2
                limit = max(y + 2, h - 3 - reserve)
                room = max(1, limit - y)
                focus = next((i for i, st in enumerate(m.steps) if st.status in (RUNNING, FAILED)), None)
                if focus is None:
                    focus = next((i for i, st in enumerate(m.steps) if st.status == PENDING), len(m.steps) - 1)
                first = 0
                while first < focus:
                    used = (focus - first + 1) + (1 if m.steps[focus].status == RUNNING else 0)
                    if used <= room:
                        break
                    first += 1
                for step in m.steps[first:]:
                    if y >= limit:
                        break
                    glyph = {DONE: g["ok"], WARN: g["warn"], FAILED: g["bad"],
                             SKIPPED: g["skip"], PENDING: g["pend"]}.get(step.status, g["run"])
                    if step.status == RUNNING and result is None:
                        glyph = g["spin"][int(time.monotonic() * 10) % len(g["spin"])]
                    sa = self._status_attr(step.status)
                    right = ""
                    if step.status in FINISHED + (FAILED,) and step.t0:
                        right = human_time(step.elapsed())
                    note = f"  {step.note}" if step.note and step.status != RUNNING else ""
                    title = self._clip(step.title + note, w - 14 - len(right))
                    self._put(y, 2, f"{glyph} ", sa)
                    self._put(y, 4, title, curses.A_BOLD if step.status == RUNNING else (curses.A_DIM if step.status in (PENDING, SKIPPED) else 0))
                    self._put(y, w - 2 - len(right), right, curses.A_DIM)
                    y += 1
                    if step.status == RUNNING and y < limit:
                        inner = max(8, min(24, w - 56))
                        if step.frac is None:
                            pos = int(time.monotonic() * 12) % max(1, inner)
                            cells = [g["empty"]] * inner
                            for k in range(4):
                                cells[(pos + k) % inner] = g["full"]
                            line_bar = "".join(cells)
                            pct = ""
                        else:
                            line_bar = ascii_bar(step.frac, inner, g["full"], g["empty"])
                            pct = f"{int(step.frac * 100):3d}%  "
                        self._put(y, 4, line_bar, self.colors["accent"])
                        x = 4 + inner + 2
                        self._put(y, x, self._clip(pct + step.detail, w - x - 2))
                        y += 1
                y += 1

            rows = h - 2 - y
            avail = rows - 1
            head: list[tuple[str, int]] = []
            body: list[tuple[str, int]] = []
            title = "Log"
            if result == "ok":
                title = "Summary"
                width = max((len(k) for k, _ in m.summary), default=0)
                head = [(f"{k.ljust(width)}  {v}", 0) for k, v in m.summary]
                head += [(f"{g['warn']} {wn}", self.colors["warn"]) for wn in m.warnings]
                if m.footer:
                    head += [("", 0)] + [(line, curses.A_DIM) for line in m.footer]
            elif result in ("failed", "cancelled"):
                title = "Error" if result == "failed" else "Cancelled"
                text = m.error or ("Nothing was left mounted. Files already copied are harmless; "
                                   "run the installer again to finish." if result == "cancelled" else "")
                color = self.colors["bad"] if result == "failed" else self.colors["warn"]
                head = [(line, color) for line in textwrap.wrap(text, max(20, w - 8))]
                head.append(("", 0))
                body = [(self._clip(l, w - 8), curses.A_DIM) for l in m.logbuf]
            else:
                body = [(self._clip(l, w - 8), curses.A_DIM) for l in m.logbuf]
            if rows >= 3:
                self._put(y, 2, g["rule"] * 2 + f" {title} " + g["rule"] * max(0, w - 10 - len(title)), curses.A_DIM)
                y += 1
                head = head[:avail]
                body = body[-(avail - len(head)):] if avail > len(head) else []
                for text, attr in head + body:
                    self._put(y, 4, text, attr)
                    y += 1

            hint = "Press any key to exit" if final else "Ctrl-C to cancel"
            self._put(h - 1, 2, hint, curses.A_DIM)
        scr.refresh()

def pick_ui(plain: bool, verbose: bool):
    if not plain and sys.stdin.isatty() and sys.stdout.isatty():
        term = os.environ.get("TERM", "")
        size = shutil.get_terminal_size((80, 24))
        if term and term != "dumb" and size.lines >= CursesUI.MIN_ROWS and size.columns >= CursesUI.MIN_COLS:
            return CursesUI()
    return PlainUI(verbose=verbose)