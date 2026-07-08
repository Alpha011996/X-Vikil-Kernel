#!/bin/bash
# SUBSYSTEM: XVK-THERM | Component: skin_thermal_model
# File: scripts/xvk_skin_thermal_model.sh
# Status: Implemented
# Phase: Thermal

THERM_SYSFS=/sys/kernel/xvk_therm
SKIN_ZONE=""
TARGET_SKIN_TEMP=38000 # 38.0 C

# Find skin thermal zone (shell_max is usually the highest priority skin sensor)
for zone in /sys/class/thermal/thermal_zone*; do
    type=$(cat $zone/type 2>/dev/null)
    if [[ "$type" == *"shell"* || "$type" == *"case"* ]]; then
        SKIN_ZONE=$zone
        # Prefer shell_max if available
        if [[ "$type" == "shell_max" ]]; then
            break
        fi
    fi
done

if [ -z "$SKIN_ZONE" ]; then
    echo "[-] Skin thermal zone not found. Exiting."
    exit 1
fi

echo "[*] Monitoring Skin zone: $SKIN_ZONE ($(cat $SKIN_ZONE/type))"
echo "[*] Target Skin Temp: $TARGET_SKIN_TEMP millicelsius"

while true; do
    temp=$(cat $SKIN_ZONE/temp 2>/dev/null)
    
    if [ "$temp" -gt $TARGET_SKIN_TEMP ]; then
        # Skin is too hot, enable predictive throttle
        echo 1 > $THERM_SYSFS/predictive_throttle
    else
        # Skin is cool, disable predictive throttle
        echo 0 > $THERM_SYSFS/predictive_throttle
    fi
    
    sleep 2
done
