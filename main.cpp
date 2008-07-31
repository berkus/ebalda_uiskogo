#include "ebalda.h"
#include <sys/time.h>
#include <stdlib.h>

// Run: ebalda new "Name" filename.dmp
// or
// ebalda filename.dmp

int main(int argc, char **argv)
{
	// Seed RNG.
	timeval tt;
	gettimeofday(&tt, NULL);
	srand(tt.tv_usec);

	Ebalda ebalda;
	ebalda.Hatch("Jfgzoza");
	ebalda.Grow();
	ebalda.SaveState("jfgzoza.state");
}
