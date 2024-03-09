#ifndef __SOCKRDR_H
#define __SOCKRDR_H

#define AF_INET 2
#define MAX_MAP_ENTRIES 4096

struct sock_key {
    __u32 family;
    __u32 remote_ip4;
    __u32 local_ip4;
    __u16 remote_port;
    __u16 local_port;
};

#endif /* __SOCKRDR_H */