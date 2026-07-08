#!/bin/bash
# SUBSYSTEM: XVK-MEM | Component: psi_monitor
# File: scripts/xvk_psi_monitor.sh
# Status: Implemented
# Phase: Memory

if [ ! -f /proc/pressure/memory ]; then
    echo "[-] PSI not available on this kernel"
    exit 1
fi

echo "[*] Monitoring memory pressure (10s window)..."
awk '/memory/ { print "Memory Pressure - avg10: "$3" avg60: "$4" avg300: "$5 }' /proc/pressure/memory

echo "[*] Watching for memory stall events (threshold: 50ms)..."
timeout 10 grep -E "some|full" /proc/pressure/memory
