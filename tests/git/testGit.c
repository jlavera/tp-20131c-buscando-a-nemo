#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

int main(int argc, char **argv) {
	chdir( "../../../src/koopa/" );
	system( "export TERM=xterm");
	system( "./koopa feedKoopa" );
	return 0;
}
