#!/bin/bash
# SUBSYSTEM: XVK-SCHED | Component: cpusets
# File: scripts/xvk_cpusets.sh
# Status: Implemented
# Phase: Scheduler

# SM7635-AC Topology (Assumed 1+3+4)
# Little: 0-3
# Big:    4-6
# Prime:  7

CDIR=/dev/cpuset

if [ ! -d "$CDIR" ]; then
    mount -t cpuset none $CDIR 2>/dev/null
fi

if [ ! -d "$CDIR/top-app" ]; then
    mkdir $CDIR/top-app
    mkdir $CDIR/foreground
    mkdir $CDIR/background
    mkdir $CDIR/restricted
fi

echo "[*] Applying XVK CPUset isolation..."

# Top-App: Gets Prime + Big (4-7)
echo 4-7 > $CDIR/top-app/cpus
echo 0 > $CDIR/top-app/cpuset.cpu_exclusive
echo 1 > $CDIR/top-app/mems

# Foreground: Gets Big only (4-6)
echo 4-6 > $CDIR/foreground/cpus
echo 0 > $CDIR/foreground/cpuset.cpu_exclusive
echo 1 > $CDIR/foreground/mems

# Background: Restricted to Little (0-3)
echo 0-3 > $CDIR/background/cpus
echo 0 > $CDIR/background/cpuset.cpu_exclusive
echo 1 > $CDIR/background/mems

# Restricted (system/bg services): Little only (0-2, reserve 3 for IRQs)
echo 0-2 > $CDIR/restricted/cpus
echo 0 > $CDIR/restricted/cpuset.cpu_exclusive
echo 1 > $CDIR/restricted/mems

echo "[+] XVK CPUsets applied successfully."
