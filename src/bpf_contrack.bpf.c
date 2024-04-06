#include "bpf_sockmap.h"

char LICENSE[] SEC("license") = "Dual BSD/GPL";

SEC("sockops")
int bpf_sockops_handler(struct bpf_sock_ops *skops){
    /* ignore packets that meets the following conditions
     *   1. not IPv4
     *   2. IPv4 address not the same (not localhost)
     *   3. IPv4 port not the same (not the server port)
     */
    if (skops->family != AF_INET)
        return BPF_OK;

    if(skops->remote_ip4 != LOCALHOST_IPV4 || skops->local_ip4 != LOCALHOST_IPV4)
        return BPF_OK;

    if (skops->local_port != 9453)
        return BPF_OK;

	struct sock_key key = {
        .dip = skops->remote_ip4,
        .sip = skops->local_ip4,
        .sport = bpf_htonl(skops->local_port),  /* convert to network byte order */
        .dport = skops->remote_port,
        .family = skops->family,
    };

    switch(skops->op) {
        case BPF_SOCK_OPS_PASSIVE_ESTABLISHED_CB:
        case BPF_SOCK_OPS_ACTIVE_ESTABLISHED_CB:
            bpf_sock_hash_update(skops, &sock_ops_map, &key, BPF_NOEXIST);
            break;
        default:
            break;
    }

	bpf_printk(">>> connection: OP:%d, PORT:%d --> %d\n", \
        skops->op, bpf_ntohl(key.sport), bpf_ntohl(key.dport));

    return BPF_OK;
}