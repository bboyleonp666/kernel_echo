#include "ebpf_tcp_redirect.h"

static __always_inline void update_sockmap_ops(struct bpf_sock_ops *skops) {
    struct sock_key skk = {
        .family = skops->family,
        .remote_ip4 = skops->remote_ip4,
        .local_ip4  = skops->local_ip4,
        .remote_port = bpf_ntohl(skops->remote_port),
        .local_port = skops->local_port,
    };

    /* force writing the key value pair even if already exist */
    int ret;
    if ((ret = bpf_map_update_elem(&skops, &skk, &skk, BPF_ANY)) < 0) {
        bpf_trace_printk("update bpf_sock_ops failed. Code: %d\n", ret);
    }
}

SEC("sockops")
int bpf_sockhash(struct bpf_sock_ops *skops) {
    /* ignore packets that are not IPv4 */
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
            update_sockmap_ops(skops);
            break;
        default:
            break;
    }

    return 0;
}

char _license[] SEC("license") = "GPL";