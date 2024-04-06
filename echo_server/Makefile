CC := clang
CFLAGS := -g -O2

SOCK_PORT := 9453
CLIENT_MSG := "Hello from client"

PROGS := server client

.PHONY: all clean test
all: $(PROGS)

server: server.c
	$(CC) $(CFLAGS) -o $@ $<

client: client.c
	$(CC) $(CFLAGS) -o $@ $<

test: $(PROGS)
	scripts/test_echo.sh $(SOCK_PORT) $(CLIENT_MSG)

clean:
	rm -f $(PROGS)