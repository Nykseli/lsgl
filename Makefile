all:
	gcc *.c -Wall -Wextra -pedantic -std=c99 -o nyan

debug:
	gcc *.c -Wall -Wextra -pedantic -std=c99 -g -o nyan
	