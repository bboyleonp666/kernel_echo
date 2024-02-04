#ifndef __EBPF_TCP_REDIRECT_H
#define __EBPF_TCP_REDIRECT_H

#include <sys/socket.h>
#include <linux/types.h>

#include <linux/bpf.h>
#include <bpf/bpf_endian.h>
#include <bpf/bpf_helpers.h>

#define MAX_SOCK_OPS_MAP_ENTRIES 65535
#define MAX_BPF_MAP_ENTRIES 1024

#define SOCKET_PORT 9453

struct sock_key {
    __u32 family;
    __u32 remote_ip4;
    __u32 local_ip4;
    __u32 remote_port;
    __u32 local_port;
};

struct {
	__uint(type, BPF_MAP_TYPE_SOCKMAP);
	__type(key_size, sizeof(struct sock_key));
	__type(value_size, sizeof(__u32));
	__uint(max_entries, MAX_SOCK_OPS_MAP_ENTRIES);
} sockops_map SEC(".maps");

#endif