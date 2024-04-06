CC := clang
CFLAGS := -g -O2 -Wall -Wextra -I /usr/include/aarch64-linux-gnu
MAKE := make

PROGS := ebpf_tcp_redirect.o ebpf_sockmap_ops.o
.PHONY: all clean test

all: $(PROGS)

echo_server:
	$(MAKE) -C echo_server

ebpf_tcp_redirect.o: ebpf_tcp_redirect.c
	$(CC) $(CFLAGS) -target bpf -c -o $@ $<

ebpf_sockmap_ops.o: ebpf_sockmap_ops.c
	$(CC) $(CFLAGS) -target bpf -c -o $@ $<

clean:
	$(MAKE) -C echo_server clean
	rm -f $(PROGS)