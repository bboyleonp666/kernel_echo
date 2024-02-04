#include "ebpf_tcp_redirect.h"

SEC("sk_skb/stream_verdict")
int bpf_redir(struct __sk_buff *skb) {
    /* ignore packets that meets the following conditions
     *   1. not IPv4
     *   2. IPv4 address not the same (not localhost)
     *   3. IPv4 port not the same (not the server port)
     */
    if (skb->family != AF_INET)
        return SK_PASS;

    if (skb->remote_ip4 != skb->local_ip4)
        return SK_PASS;
    
    if (skb->local_port != SOCKET_PORT)
        return SK_PASS;
    
    /* exchange the src port and dst port */
    struct sock_key skk = {
        .family = skb->family,
        .remote_ip4 = skb->remote_ip4,
        .local_ip4  = skb->local_ip4,
        .remote_port = skb->local_port,
        .local_port = bpf_ntohl(skb->remote_port),
    };

    /* redirect the packet */
    bpf_trace_printk("try redirect port %d --> %d\n", skb->local_port,
        bpf_ntohl(skb->remote_port));
    long ebpf_redir = bpf_sk_redirect_hash(skb, &sockops_map, &skk, BPF_F_INGRESS);
    if (ebpf_redir != SK_PASS) {
        bpf_trace_printk("redirect port %d --> %d failed\n", skb->local_port, 
            bpf_ntohl(skb->remote_port));
    }

    return ebpf_redir;
}

char _license[] SEC("license") = "GPL";