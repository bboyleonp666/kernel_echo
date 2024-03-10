#include "vmlinux.h"
#include <bpf/bpf_endian.h>
#include <bpf/bpf_helpers.h>
#include <bpf/bpf_core_read.h>
#include "sockrdr.h"

char LICENSE[] SEC("license") = "Dual BSD/GPL";

struct sock_key {
    __u32 family;
    __u32 remote_ip4;
    __u32 local_ip4;
    __u32 remote_port;
    __u32 local_port;
};

struct {
    __uint(type, BPF_MAP_TYPE_SOCKHASH);
    __uint(max_entries, MAX_SOCKMAP_ENTRIES);
    __type(key, struct sock_key);
    __type(value, __u32);
} sock_ops SEC(".maps");

static __always_inline void update_sock_ops(struct bpf_sock_ops *skops) {
    struct sock_key skk = {
        .family = skops->family,
        .remote_ip4 = skops->remote_ip4,
        .local_ip4  = skops->local_ip4,
        .remote_port = bpf_ntohl(skops->remote_port),
        .local_port = skops->local_port,
    };

    /* force writing the key value pair even if already exist */
    int ret;
    if ((ret = bpf_sock_hash_update(skops, &sock_ops, &skk, BPF_ANY)) < 0) {
        bpf_printk("update bpf_sock_ops failed. Code: %d\n", ret);
    }
}

SEC("sockops")
int bpf_sockhash(struct bpf_sock_ops *skops) {
    /* ignore packets that are not IPv4 */
    bpf_printk("Now in bpf_sockmap\n");
    if (skops->family != AF_INET)
        return 0;

    switch (skops->op) {
        /* refer to linux/tools/include/uapi/linux/bpf.h
         * BPF_SOCK_OPS_PASSIVE_ESTABLISHED_CB:
         * callback for ACK in 3-way handshake.
         *
         * BPF_SOCK_OPS_ACTIVE_ESTABLISHED_CB:
         * callback for SYN-ACK in 3-way handshake.
         */
        case BPF_SOCK_OPS_PASSIVE_ESTABLISHED_CB:
        case BPF_SOCK_OPS_ACTIVE_ESTABLISHED_CB:
            update_sock_ops(skops);
            break;
        default:
            break;
    }

    return 0;
}

SEC("sk_skb/stream_verdict")
int bpf_redir(struct __sk_buff *skb) {
    /* ignore packets that meets the following conditions
     *   1. not IPv4
     *   2. IPv4 address not the same (not localhost)
     *   3. IPv4 port not the same (not the server port)
     */
    bpf_printk("Now in bpf_redir\n");
    if (skb->family != AF_INET)
        return SK_PASS;

    if (skb->remote_ip4 != skb->local_ip4)
        return SK_PASS;
    
    if (skb->local_port != 9453)
        return SK_PASS;

    long rdr;
    
    /* exchange the src port and dst port */
    struct sock_key skk = {
        .family = skb->family,
        .remote_ip4 = skb->remote_ip4,
        .local_ip4  = skb->local_ip4,
        .remote_port = skb->local_port,
        .local_port = bpf_ntohl(skb->remote_port),
    };

    /* redirect the packet */
    bpf_printk("try redirect port %d --> %d\n", skb->local_port,
        bpf_ntohl(skb->remote_port));
    rdr = bpf_sk_redirect_hash(skb, &sock_ops, &skk, BPF_F_INGRESS);
    if (rdr != SK_PASS) {
        bpf_printk("redirect port %d --> %d failed\n", skb->local_port, 
            bpf_ntohl(skb->remote_port));
    }
    return rdr;
}