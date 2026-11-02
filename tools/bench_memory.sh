#!/usr/bin/env bash
# Peak resident memory of the main modes, for the Vita/PSP memory budget.
# Usage: tools/bench_memory.sh [path/to/rr4]
# Needs GNU time (/usr/bin/time). Run from the repo root.
set -u

BIN=${1:-./build/rr4}
TIME=/usr/bin/time
if [[ ! -x $TIME ]]; then
    echo "bench_memory: $TIME not found (install GNU time)" >&2
    exit 1
fi
if [[ ! -x $BIN ]]; then
    echo "bench_memory: $BIN not found (build first)" >&2
    exit 1
fi

# Runs one command and prints its peak RSS in MB. /usr/bin/time writes the
# peak to stderr; the game writes its normal log there too, so keep both.
run() {
    local name=$1
    shift
    local out err rss
    out=$(mktemp) err=$(mktemp)
    "$TIME" -v "$@" >"$out" 2>"$err"
    rss=$(sed -n 's/.*Maximum resident set size (kbytes): *//p' "$err")
    rm -f "$out" "$err"
    if [[ -z $rss ]]; then
        printf '%-24s %s\n' "$name" "failed"
        return
    fi
    awk -v n="$name" -v k="$rss" 'BEGIN { printf "%-24s %7.1f MB\n", n, k / 1024 }'
}

echo "rr4 memory benchmark ($BIN)"
echo
run "assets only (--trace)" "$BIN" --trace 0
run "probe (--probe)" "$BIN" --probe 32053 30797 45
run "audio (--audio-test)" "$BIN" --audio-test
run "software bench" "$BIN" --bench 60
run "OpenGL bench" "$BIN" --gl --bench 60
# Interactive runs open a real window (set SDL_VIDEODRIVER=dummy for a
# driver-memory-free number on a headless machine).
run "interactive software" timeout 5 "$BIN" --soft
run "interactive OpenGL" timeout 5 "$BIN"
