CC = clang

CFLAGS = -g -Wall -lm -lncurses -lpthread -lrt#-fsanitize=address #--enable-widec --enable-ext-colors #-Werror

TARGETS = display game_play
#---------------------

all: $(TARGETS)

%: %.c
	@echo Compiling $<
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -f list *o *~ core* $(TARGETS)