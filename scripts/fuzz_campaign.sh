#!/bin/bash
# fuzz_campaign.sh
# Usage: ./fuzz_campaign.sh <target_executable> <number_of_nodes> <output_directory>

set -e

if [[ $# -ne 3 ]]; then
    echo "Usage: $0 <target_executable> <number_of_nodes> <output_directory>"
    exit 1
fi

BINARY="$1"
NUM_NODES="$2"
OUTDIR="$3"
SEEDS="$HOME/qt-http-fuzzer/corpus"
DICTIONARY="$HOME/qt-http-fuzzer/wordlist/http.afl.dict"

# Clean up old output if exists
mkdir -p "$OUTDIR"

# Send stats to server
export AFL_STATSD=1
export AFL_STATSD_HOST=10.212.170.69
export AFL_STATSD_PORT=8125
export AFL_STATSD_TAGS_FLAVOR=dogstatsd

# Function to start a fuzzer node
start_fuzzer() {
    local TYPE="$1"
    local NAME="$2"
    local TARGET="$3"
    local EXTRA="${EXTRA:-}"

    if [[ "$TYPE" == "main" ]]; then
        export AFL_FINAL_SYNC=1
        screen -dmS "$NAME" -- afl-fuzz -M "$NAME" \
          -i "$SEEDS" \
          -o "$OUTDIR" \
          -x "$DICTIONARY" \
          "$TARGET"
    else
        unset AFL_FINAL_SYNC
        if [[ "$TARGET" == "$HOME/qt-http-fuzzer/asan-build/harness/$BINARY" ]]; then
            screen -dmS "$NAME" -- sudo ./asan_limit.sh afl-fuzz -S "$NAME" \
                -i "$SEEDS" \
                -o "$OUTDIR" \
                -x "$DICTIONARY" \
                $EXTRA \
                $OPTIONS \
                "$TARGET"
        else
            screen -dmS "$NAME" -- afl-fuzz -S "$NAME" \
                -i "$SEEDS" \
                -o "$OUTDIR" \
                -x "$DICTIONARY" \
                $EXTRA \
                $OPTIONS \
                "$TARGET"
        fi
    fi
}

# Start main fuzzer
MAIN_NAME="$BINARY-main"
echo "[*] Starting main fuzzer: $MAIN_NAME"
start_fuzzer "main" "$MAIN_NAME" "$HOME/qt-http-fuzzer/afl-build/harness/$BINARY"

THRESHOLD=$(( NUM_NODES * 30 / 100 ))

# Start secondary fuzzers
for i in $(seq 1 $(($NUM_NODES - 1))); do
    NAME="$BINARY-$i"
    OPTIONS=""

    if (( i >= THRESHOLD )); then
        export AFL_CUSTOM_MUTATOR_LIBRARY="$HOME/Grammar-Mutator/libgrammarmutator-http.so"
        export AFL_CUSTOM_MUTATOR_ONLY=1
    else
        unset AFL_CUSTOM_MUTATOR_LIBRARY
        unset AFL_CUSTOM_MUTATOR_ONLY
    fi

    # Randomly apply variants
    case $((i % 5)) in
        0) OPTIONS="-L 0" ;;                          # MOpt
        1) OPTIONS="-Z" ;;                            # old queue
        2) OPTIONS="-d" ;;                            # disable trimming (AFL_DISABLE_TRIM)
        3) OPTIONS="-P explore" ;;                    # power schedule explore
        4) OPTIONS="-P exploit" ;;                    # power schedule exploit
    esac

    # Randomly apply instrumentation
    rand=$(( RANDOM % 4 ))

    if [[ $rand -eq 0 ]]; then
        # Plain AFL (no instrumentation)
        unset AFL_USE_ASAN
        unset AFL_LLVM_LAF_ALL
        unset AFL_LLVM_CMPLOG
        TARGET="$HOME/qt-http-fuzzer/afl-build/harness/$BINARY"
    elif [[ $rand -eq 1 ]]; then
        unset AFL_USE_ASAN
        unset AFL_LLVM_LAF_ALL
        export AFL_LLVM_CMPLOG=1
        EXTRA="-l 2AT -c $HOME/qt-http-fuzzer/cmplog-build/harness/$BINARY"
        TARGET="$HOME/qt-http-fuzzer/afl-build/harness/$BINARY"
    elif [[ $rand -eq 2 ]]; then
        # LAF-INTEL
        unset AFL_USE_ASAN
        unset AFL_LLVM_CMPLOG
        export AFL_LLVM_LAF_ALL
        TARGET="$HOME/qt-http-fuzzer/laf-build/harness/$BINARY"
        EXTRA=""
    elif [[ $rand -eq 3 ]]; then
        # ASAN
        unset AFL_LLVM_CMPLOG
        unset AFL_LLVM_LAF_ALL
        export AFL_USE_ASAN=1
        TARGET="$HOME/qt-http-fuzzer/asan-build/harness/$BINARY"
        EXTRA="-m none"
    fi

    echo "[*] Starting secondary fuzzer: $NAME with options: $OPTIONS"
    start_fuzzer "secondary" "$NAME" "$TARGET"
done

echo "[*] All fuzzers started. Monitoring..."
wait
