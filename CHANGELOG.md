# X-Vikil Kernel Changelog

## v1.0 - 2024-05-16 (Initial Release)
### Target: Nothing Phone (4a) / SM7635-AC
- **XVK-INSTR**: Added /proc/xvikil/version, ftrace wrapper, eBPF skeleton, and benchmark harness.
- **XVK-SCHED**: Implemented dynamic UCLAMP sysfs, cpuset isolation for 1+3+4 topology, schedutil rate_limit set to 0.
- **XVK-MEM**: Migrated zRAM to ZSTD, enabled MGLRU, enabled PSI, exposed watermark_scale_factor sysfs.
- **XVK-THERM**: Added predictive throttle sysfs, BCL monitor script, skin temp model script.
- **XVK-STORE**: Added BFQ low latency sysfs.
- **XVK-GFX**: Added Adreno 810 busy threshold sysfs.
- **XVK-SEC**: Integrated SukiSU-Ultra (manual su), added xvikil.te SELinux policy, enforced CFI, added namespace isolation sysfs.
- **XVK-LAB**: Added eBPF sched_migrate probe, PGO defconfig, experimental idle governor.
