#!/bin/bash

MEM_MB=128
CGROUP=/sys/fs/cgroup/asan-$RANDOM

# create cgroup
sudo mkdir -p "$CGROUP"

# set memory limit
echo $((MEM_MB * 1024 * 1024)) | sudo tee "$CGROUP/memory.max" > /dev/null

# move THIS process into the cgroup
echo $$ | sudo tee "$CGROUP/cgroup.procs" > /dev/null

# run whatever command you passed
exec "$@"
