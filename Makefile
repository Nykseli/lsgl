all:
	gcc src/*.c -Wall -Wextra -pedantic -std=c99 -o nyan

debug:
	gcc src/*.c -Wall -Wextra -pedantic -std=c99 -g -o nyan
	