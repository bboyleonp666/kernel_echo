#include "vmlinux.h"
#include <bpf/bpf_helpers.h>
#include <bpf/bpf_core_read.h>
#include "sockrdr.h"

char LICENSE[] SEC("license") = "Dual BSD/GPL";

struct sock_key {
    __u32 family;
    __u32 remote_ip4;
    __u32 local_ip4;
    __u16 remote_port;
    __u16 local_port;
};

struct {
    __uint(type, BPF_MAP_TYPE_HASH);
    __uint(max_entries, MAX_MAP_ENTRIES);
    __type(key, struct sock_key);
    __type(value, __u32);
} sock_ops SEC(".maps");

// SEC("sk_skb/stream_verdict")
// int bpf_redir(struct __sk_buff *skb) {
//     /* ignore packets that meets the following conditions
//      *   1. not IPv4
//      *   2. IPv4 address not the same (not localhost)
//      *   3. IPv4 port not the same (not the server port)
//      */
//     if (skb->family != AF_INET)
//         return SK_PASS;

//     if (skb->remote_ip4 != skb->local_ip4)
//         return SK_PASS;
    
//     if (skb->local_port != 9453)
//         return SK_PASS;
    
//     /* exchange the src port and dst port */
//     struct sock_key skk = {
//         .family = skb->family,
//         .remote_ip4 = skb->remote_ip4,
//         .local_ip4  = skb->local_ip4,
//         .remote_port = skb->local_port,
//         .local_port = bpf_ntohl(skb->remote_port),
//     };

//     /* redirect the packet */
//     bpf_printk("try redirect port %d --> %d\n", skb->local_port,
//         bpf_ntohl(skb->remote_port));
//     long ebpf_redir = bpf_sk_redirect_hash(skb, &sockops_map, &skk, BPF_F_INGRESS);
//     if (ebpf_redir != SK_PASS) {
//         bpf_printk("redirect port %d --> %d failed\n", skb->local_port, 
//             bpf_ntohl(skb->remote_port));
//     }

//     return ebpf_redir;
// }