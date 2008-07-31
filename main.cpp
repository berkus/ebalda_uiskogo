#include "ebalda.h"
#include <sys/time.h>
#include <stdlib.h>

int main(int, char **)
{
	// Seed RNG.
	timeval tt;
	gettimeofday(&tt, NULL);
	srand(tt.tv_usec);

	Ebalda ebalda;

	ebalda.Hatch("Jfgzoza"); // uncomment these two lines to make a new one
	ebalda.SaveState("jfgzoza.state");

	if (ebalda.LoadState("jfgzoza.state"))
	{
		ebalda.Grow();
		ebalda.SaveState("jfgzoza.state");
	}
}
