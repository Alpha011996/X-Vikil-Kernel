#!/bin/bash
# SUBSYSTEM: XVK-THERM | Component: bcl_monitor
# File: scripts/xvk_bcl_monitor.sh
# Status: Implemented
# Phase: Thermal

THERM_SYSFS=/sys/kernel/xvk_therm
BCL_ZONES=()

# Exact BCL zones for SM7635-AC (volcano)
for zone in /sys/class/thermal/thermal_zone*; do
    type=$(cat $zone/type 2>/dev/null)
    if [[ "$type" == *"bcl"* || "$type" == *"ibat"* || "$type" == "vbat" ]]; then
        BCL_ZONES+=("$zone")
    fi
done

if [ ${#BCL_ZONES[@]} -eq 0 ]; then
    echo "[-] No BCL thermal zones found. Exiting."
    exit 1
fi

echo "[*] Monitoring BCL zones: ${BCL_ZONES[@]}"
echo "[*] Target: $THERM_SYSFS/thermal_headroom"

while true; do
    max_temp=0
    for zone in "${BCL_ZONES[@]}"; do
        temp=$(cat $zone/temp 2>/dev/null)
        if [ "$temp" -gt "$max_temp" ]; then
            max_temp=$temp
        fi
    done
    
    # BCL levels trigger at specific milli-degree thresholds (e.g., 45000)
    if [ "$max_temp" -gt 45000 ]; then
        # Hard BCL trip imminent, drop to 50% headroom
        echo 50 > $THERM_SYSFS/thermal_headroom
    elif [ "$max_temp" -gt 40000 ]; then
        # Proactively increase headroom to 30% to drop CPU power
        echo 30 > $THERM_SYSFS/thermal_headroom
    else
        # Normal operation, allow full performance
        echo 10 > $THERM_SYSFS/thermal_headroom
    fi
    
    sleep 1
done
