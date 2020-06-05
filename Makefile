CC=gcc
CFLAGS=-Wall -Werror -g -lrt -lpthread

PROGS=echo_server echo_server_thread echo_server_thread_both echo_server_udp_thread udp_client

.PHONY: all
all: $(PROGS)

%: %.c
	$(CC) -o $@ $< $(CFLAGS)

.PHONY: clean
clean:
	rm -f $(PROGS)

