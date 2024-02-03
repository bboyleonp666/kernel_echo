CC := clang
CFLAGS := -g -O2 -Wall -Wextra

SOCK_PORT=9453
CLIENT_MSG="Hello, World!"

PROGS := echo_server echo_client
.PHONY: all clean test

all: $(PROGS)

echo_server: echo_server.c
	$(CC) $(CFLAGS) -o $@ $^

echo_client: echo_client.c
	$(CC) $(CFLAGS) -o $@ $^

test: $(PROGS)
	@scripts/test_echo.sh '$(SOCK_PORT)' '$(CLIENT_MSG)'

clean:
	rm -f $(PROGS)