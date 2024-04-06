CC := clang
CFLAGS := -g -O2 -Wall -Wextra -I /usr/include/aarch64-linux-gnu

MAKE := make
MAKE_FLAG :=

ifeq ($(V),1)
	Q =
	MAKE_FLAG +=
else
	Q = @
	MAKE_FLAG += -s
endif

.PHONY: all install
all: install

install:
	$(Q)$(MAKE) $(MAKE_FLAG) -C src install
	$(Q)$(MAKE) $(MAKE_FLAG) -C echo_server install


.PHONY: clean test
test: install
	$(Q)$(MAKE) $(MAKE_FLAG) -C src test
	$(Q)$(MAKE) $(MAKE_FLAG) -C echo_server test

clean:
	$(Q)$(MAKE) $(MAKE_FLAG) -C echo_server clean
	$(Q)$(MAKE) $(MAKE_FLAG) -C src clean