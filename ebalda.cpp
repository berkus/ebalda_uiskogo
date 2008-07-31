#include "ebalda.h"

Ebalda::Ebalda()
  : name(0)
  , step(0)
  , grown(false)
  , headrow(0), headcol(0)
  , width(0), height(0)
  , ebalda_himself(0)
  , startrow(0), startcol(0)
  , endrow(0), endcol(0)
{
}

Ebalda::~Ebalda()
{
	delete ebalda_himself;
}

void Ebalda::allocate_himself()
{
	delete ebalda_himself;
	ebalda_himself = 0;

	if (width > 0 && height > 0)
	{
		ebalda_himself = new char [ (width + 2) * (height + 2) ];
	}
}

// Hatch a fresh new ebalda with a name.
void Ebalda::Hatch(const char *name)
{
	name = strdup(name);
	width = height = 1;
	allocate_himself();
	step = 0;
	headrow = headcol = 1; // At first, it's only the head.
	startrow = startcol = 1;
	endrow = endcol = 1;
	ebalda_himself[ (width + 2) * headrow + headcol ] = 'A';
	// Now you can save your newly hatched ebalda and let it live!
}

//   Ebalda on-disk file format is as follows:
//   NB: Not portable between architectures.
//
//   'EBAlDA' id
//   1 bytes length
//   length bytes \0 terminated ebalda name
//   4 bytes step
//   2 bytes head row
//   2 bytes head column
//   2 bytes width
//   2 bytes height
//   width*height bytes field

// Load ebalda from file.
void Ebalda::LoadState(const char *fname)
{
	startrow = startcol = 1;
	endrow = width;
	endcol = height;
	grown = false;
}

// Save ebalda to file.
void Ebalda::SaveState(const char *fname)
{
	file = fopen(fname, "wb");
	if (!file)
		return;

	fwrite("EBAlDA", 6, 1, file);
	fwrite(strlen(name)+1, 1, 1, file);
	fwrite(name, strlen(name)+1, 1, file);
	fwrite(&step, 4, 1, file);
	fwrite(&headrow, 2, 1, file);
	fwrite(&headcol, 2, 1, file);
	short temp;
	temp = endcol - startcol + 1;
	fwrite(&temp, 2, 1, file);
	temp = endrow - startrow + 1;
	fwrite(&temp, 2, 1, file);

	// Now the funky write loop.
	for(int row = startrow; row <= endrow; row++)
	  fwrite(&ebalda_himself[ (width + 2) * row + startcol ], endcol - startcol + 1, 1, file);
}

void Ebalda::Grow()
{
	if (grown)
		return; // Save us from biiig memory trouble.

	int which = rand() % 2;
	int dir = rand() % 2;

	if (which == 0)
	{
		// change row
		headrow += dir ? +1 : -1;
		if (headrow < startrow)
			startrow--;
		else if (headrow > endrow)
			endrow++;
	}
	else
	{
		// change column
		headcol += dir ? +1 : -1;
		if (headcol < startcol)
			startcol--;
		else if (headcol > endcol)
			endcol++;
	}
}

void Ebalda::Draw(const char *outfile)
{
}
