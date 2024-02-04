CC := clang
CFLAGS := -g -O2 -Wall -Wextra -I /usr/include/aarch64-linux-gnu

SOCK_PORT=9453
CLIENT_MSG="Hello, World!"

PROGS := echo_server echo_client ebpf_tcp_redirect.o ebpf_sockmap_ops.o
.PHONY: all clean test

all: $(PROGS)

echo_server: echo_server.c
	$(CC) $(CFLAGS) -o $@ $<

echo_client: echo_client.c
	$(CC) $(CFLAGS) -o $@ $<

ebpf_tcp_redirect.o: ebpf_tcp_redirect.c
	$(CC) $(CFLAGS) -target bpf -c -o $@ $<

ebpf_sockmap_ops.o: ebpf_sockmap_ops.c
	$(CC) $(CFLAGS) -target bpf -c -o $@ $<

test: $(PROGS)
	@scripts/test_echo.sh '$(SOCK_PORT)' '$(CLIENT_MSG)'

clean:
	rm -f $(PROGS)