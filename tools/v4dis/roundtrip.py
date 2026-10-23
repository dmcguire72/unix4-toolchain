#!/usr/bin/env python3
import argparse, os, shutil, struct, subprocess, sys, tempfile

def image(d):
    magic, ts, ds, bs = struct.unpack('<4H', d[:8])
    if magic not in (0o407, 0o410):
        return None
    dbase = ts if magic == 0o407 else (ts + 8191) & ~8191
    mem = bytearray(dbase + ds)
    mem[0:ts] = d[16:16 + ts]
    mem[dbase:dbase + ds] = d[16 + ts:16 + ts + ds]
    return bytes(mem), bs

def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--v4dis', default='bin/v4dis')
    ap.add_argument('--emu', default='bin/v4linux')
    ap.add_argument('--root', default='v4')
    ap.add_argument('--opts', default='')
    ap.add_argument('--keep', help='directory to keep generated .s files')
    ap.add_argument('--scan', action='store_true', help='treat the arguments as directories and test every 0407/0410 file in them')
    ap.add_argument('files', nargs='+')

    a = ap.parse_args()

    if a.scan:
        found = []
        for d in a.files:
            for dp, _, fs in os.walk(d):
                for fn in sorted(fs):
                    p = os.path.join(dp, fn)

                    try:
                        h = open(p, 'rb').read(2)
                    except OSError:
                        continue

                    if len(h) == 2 and struct.unpack('<H', h)[0] in (0o407, 0o410) \
                            and os.path.getsize(p) > 16:
                        found.append(p)
        a.files = sorted(found)

    scratch = tempfile.mkdtemp()
    root = os.path.join(scratch, 'root')
    shutil.copytree(a.root, root, symlinks=True)
    tmp = os.path.join(root, 'tmp'); os.makedirs(tmp, exist_ok=True)
    emu, dis = os.path.abspath(a.emu), os.path.abspath(a.v4dis)
    bad = 0

    for f in a.files:
        orig = open(f, 'rb').read()
        oi = image(orig)
        if oi is None:
            print('SKIP %s (not 0407/0410)' % f); continue

        r = subprocess.run([dis] + a.opts.split() + [f], capture_output=True, text=True)
        if r.returncode:
            print('FAIL %s: v4dis: %s' % (f, r.stderr.strip())); bad += 1; continue

        src = os.path.join(tmp, 'x.s'); open(src, 'w').write(r.stdout)
        if a.keep:
            os.makedirs(a.keep, exist_ok=True)
            open(os.path.join(a.keep, f.replace('/', '_') + '.s'), 'w').write(r.stdout)

        ao = os.path.join(tmp, 'a.out')
        if os.path.exists(ao): os.unlink(ao)
        try:
            p = subprocess.run([emu, '-r', root, '--chdir', '/tmp', '/bin/as', 'x.s'],
                               capture_output=True, text=True, timeout=120)

        except subprocess.TimeoutExpired:
            print('FAIL %s: as timed out (symbol table full?)' % f); bad += 1; continue
    
        if not os.path.exists(ao):
            print('FAIL %s: no a.out' % f); bad += 1; continue
        
        msg = p.stdout.replace('x.s\n', '').strip()
        new = open(ao, 'rb').read()
        ni = image(new)

        if msg:
            print('FAIL %s: as errors: %s' % (f, ' '.join(msg.split())[:120])); bad += 1; continue
        if ni != oi:
            n0, n1 = (ni[0] if ni else b''), oi[0]
            diff = next((i for i in range(min(len(n0), len(n1))) if n0[i] != n1[i]), None)
            print('FAIL %s: image differs (len %d vs %d, bss %s vs %s, first diff @%s)' %
                  (f, len(n0), len(n1), ni[1] if ni else '?', oi[1],
                   oct(diff) if diff is not None else '-')); bad += 1; continue

        print('ok   %s' % f)
    shutil.rmtree(scratch)
    return 1 if bad else 0

sys.exit(main())