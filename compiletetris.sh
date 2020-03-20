#!/bin/bash
#compile script for tetris
#compiles tetris and creates executable named 'tetris'
#defaults to using gcc, but uses clang if user supplies '-c' or '--clang' as argument to script. 
#use argumnet --help to bring up help info

#prints the help message to terminal.
printHelp() {
	printf "compile script for tetris\n"
	printf "compiles tetris.c and creates executable names 'tetris'\n"
	printf "defaults to using gcc, but uses clang if user supplies '-c' to script.\n\n"
	printf "options:\n"
	printf " -c   --clang 	use clang instead of gcc\n"
	printf "-help  --help 	prints this page then exits\n"
}

#checks if GCC is installed, and if so it compiles the game with given flags.  if not installed then gives error message and exits.
useGCC() {
	(gcc -v >/dev/null 2>&1 && 
	printf "using GCC\n" &&
	gcc tetris.c -orogueclone -Wall -Wextra -O2 -finline-functions -Wswitch-enum -lncurses -std=gnu11 -otetris ) ||
	printf "error, gcc seems to not be installed.\n"
}

#compiles game using Clang
useClang() {
	printf "using Clang\n"
	clang tetris.c -Wall -Wextra -O2 -lncurses -finline-functions -Wswitch-enum -std=gnu11 -otetris   
}

main() {
	case "$1" in
		-help | --help)
			printHelp && exit
			;;
		-c | --clang)
			(clang -v >/dev/null 2>&1 && useClang) || (printf "falling back to GCC.\n" && useGCC)
			;;
		*)
			useGCC
			;;
	esac
}


main "$1"
