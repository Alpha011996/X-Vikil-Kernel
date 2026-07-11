// SUBSYSTEM: XVK-LAB | Component: ebpf_sched_probe
// File: tools/xvikil/lab/xvk_sched_probe.c
// Status: Experimental
// Phase: Lab

#include <linux/bpf.h>
#include <bpf/bpf_helpers.h>
#include <bpf/bpf_tracing.h>

char LICENSE[] SEC("license") = "GPL";

// Map to track migration counts per PID
struct {
    __uint(type, BPF_MAP_TYPE_HASH);
    __uint(max_entries, 4096);
    __type(key, u32); // PID
    __type(value, u64); // Migration Count
} migrate_count SEC(".maps");

SEC("tracepoint/sched/sched_migrate_task")
int trace_sched_migrate_task(struct trace_event_raw_sched_migrate_task *ctx) {
    u32 pid = ctx->pid;
    u64 *count = bpf_map_lookup_elem(&migrate_count, &pid);
    if (count) {
        (*count)++;
    } else {
        u64 init_val = 1;
        bpf_map_update_elem(&migrate_count, &pid, &init_val, BPF_ANY);
    }
    return 0;
}
