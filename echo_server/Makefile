CC := clang
CFLAGS := -g -O2

SOCK_PORT := 9453
CLIENT_MSG := "Hello from client"

PROGS := server client

ifeq ($(V),1)
	Q =
else
	Q = @
endif


.PHONY: all install
all: install

install: $(PROGS)

server: server.c
	$(Q)$(CC) $(CFLAGS) -o $@ $<

client: client.c
	$(Q)$(CC) $(CFLAGS) -o $@ $<


.PHONY: test clean
test: install
	$(Q)scripts/test_echo.sh $(SOCK_PORT) $(CLIENT_MSG)

clean:
	$(Q)rm -f $(PROGS)