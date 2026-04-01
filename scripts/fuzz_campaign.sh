#!/bin/bash
# fuzz_campaign_clean.sh

set -u  # safer than -e for multi-process scripts

if [[ $# -ne 3 ]]; then
    echo "Usage: $0 <target_executable> <number_of_nodes> <output_directory>"
    exit 1
fi

BINARY="$1"
NUM_NODES="$2"
OUTDIR="$3"

CPU=$(nproc)
if [[ $NUM_NODES -gt $CPU ]]; then
    echo "[!] Requested $NUM_NODES nodes, but only $CPU cores available."
    echo "[!] Limiting nodes to $CPU."
    NUM_NODES=$CPU
fi

SEEDS="$HOME/qt-http-fuzzer/corpus"
DICT="$HOME/qt-http-fuzzer/wordlist/http.afl.dict"

LOGDIR="$OUTDIR/logs"
mkdir -p "$OUTDIR" "$LOGDIR"

# Launch helper to start a fuzzer node
start_fuzzer() {
    local TYPE="$1"
    local NAME="$2"
    local TARGET="$3"
    local ENVVARS="$4"
    local EXTRA="$5"
    local OPTIONS="$6"

    local LOGFILE="$LOGDIR/${NAME}.log"

    # Send stats to server
    ENVVARS+=" AFL_STATSD=1"
    ENVVARS+=" AFL_STATSD_HOST=10.212.170.69"
    ENVVARS+=" AFL_STATSD_PORT=8125"
    ENVVARS+=" AFL_STATSD_TAGS_FLAVOR=dogstatsd"

    if [[ "$TYPE" == "main" ]]; then
        screen -dmS "$NAME" bash -c "
            exec env AFL_FINAL_SYNC=1 \
                $ENVVARS \
                afl-fuzz -M $NAME \
                    -i $SEEDS \
                    -o $OUTDIR \
                    -x $DICT \
                    $OPTIONS \
                    $TARGET \
                > $LOGFILE 2>&1
        "
    else
        # ASAN handling
        if [[ "$TARGET" == *asan-build* ]]; then
            screen -dmS "$NAME" bash -c "
                exec env  $ENVVARS \
                    sudo ./asan_limit.sh afl-fuzz -S \"$NAME\" \
                        -i $SEEDS \
                        -o $OUTDIR \
                        -x $DICT \
                        $EXTRA \
                        $OPTIONS \
                        $TARGET \
                    > $LOGFILE 2>&1
            "
        else
            screen -dmS "$NAME" bash -c "
                exec env  $ENVVARS \
                    afl-fuzz -S \"$NAME\" \
                        -i $SEEDS\" \
                        -o $OUTDIR\" \
                        -x \"$DICT\" \
                        $EXTRA \
                        $OPTIONS \
                        \"$TARGET\" \
                    > \"$LOGFILE\" 2>&1
            "
        fi
    fi
}

# Start main fuzzer
MAIN_NAME="$BINARY-main"

echo "[*] Starting main: $MAIN_NAME"

start_fuzzer "main" \
    "$MAIN_NAME" \
    "$HOME/qt-http-fuzzer/afl-build/harness/$BINARY" \
    "" "" ""


# Function to start a fuzzer node
for i in $(seq 1 $((NUM_NODES - 1))); do
    NAME="$BINARY-$i"

    ENVVARS=""
    OPTIONS=""
    EXTRA=""
    TARGET=""

    # Randomly apply variants
    case $((i % 4)) in
        0) OPTIONS="-Z" ;;
        1) OPTIONS="-d" ;;
        2) OPTIONS="-P explore" ;;
        3) OPTIONS="-P exploit" ;;
    esac

    # Randomly apply instrumentation
    rand=$(( RANDOM % 4 ))

    if [[ $rand -eq 0 ]]; then
        TARGET="$HOME/qt-http-fuzzer/afl-build/harness/$BINARY"

    elif [[ $rand -eq 1 ]]; then
        ENVVARS+=" AFL_LLVM_CMPLOG=1"
        EXTRA="-l 2AT -c $HOME/qt-http-fuzzer/cmplog-build/harness/$BINARY"
        TARGET="$HOME/qt-http-fuzzer/afl-build/harness/$BINARY"

    elif [[ $rand -eq 2 ]]; then
        ENVVARS+=" AFL_LLVM_LAF_ALL=1"
        TARGET="$HOME/qt-http-fuzzer/laf-build/harness/$BINARY"

    elif [[ $rand -eq 3 ]]; then
        ENVVARS+=" AFL_USE_ASAN=1"
        EXTRA="-m none"
        TARGET="$HOME/qt-http-fuzzer/asan-build/harness/$BINARY"
    fi

    echo "[*] Starting secondary: $NAME"
    start_fuzzer "secondary" "$NAME" "$TARGET" "$ENVVARS" "$EXTRA" "$OPTIONS"
done

echo "[*] All fuzzers started."
echo "[?] Logs:    $LOGDIR"
echo "[?] Example: tail -f $LOGDIR/${MAIN_NAME}.log"
echo "[?] Attach:  screen -r ${MAIN_NAME}"
wait
