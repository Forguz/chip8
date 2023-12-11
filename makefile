CFLAGS=-std=c17 -v -Wall -Wextra -Werror -I ./include -D_THREAD_SAFE  -L ./lib -l SDL2 
dev:
	clang chip8.c -o chip8 $(CFLAGS)
debug:
	clang chip8.c -g -o chip8 $(CFLAGS)
