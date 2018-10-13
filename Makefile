all:
	gcc src/*.c -Wall -Wextra -pedantic -std=c99 -o lsgl

debug:
	gcc src/*.c -Wall -Wextra -pedantic -std=c99 -g -o lsgl
	