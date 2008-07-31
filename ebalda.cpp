#include "ebalda.h"
#include "config.h"
#include <gd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

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
		if (!ebalda_himself)
			abort(); // Хуякс!!
		memset(ebalda_himself, 'A', (width + 2) * (height + 2));
	}
}

#ifdef DEBUG
void Ebalda::dump()
{
	fprintf(stderr, "Wild ebalda: %s @ step %d (grown: %d)\n"
					" Height %d x Width %d\n"
					" Head @ %d x %d\n"
					" Start @ %d x %d\n"
					" End @ %d x %d\n", name, step, grown,
					height, width,
					headrow, headcol,
					startrow, startcol,
					endrow, endcol);
	for(int row = 0; row < height+2; row++)
//	for(int row = startrow; row <= endrow; row++)
	{
	    fprintf(stderr, "Row %02d: ", row);
	  for(int col = 0; col < width+2; col++)
//	  for(int col = startcol; col <= endcol; col++)
	  {
		char c = ebalda_himself[ (width + 2) * row + col ];
		fprintf(stderr, "%c", (row == headrow && col == headcol) ? toupper(c) : tolower(c));
	  }
		fprintf(stderr, "\n");
	}
	fprintf(stderr, "\n");
}
#endif

// Hatch a fresh new ebalda with a name.
void Ebalda::Hatch(const char *n)
{
	name = strdup(n);
	width = height = 1;
	allocate_himself();
	step = 0;
	headrow = headcol = 1; // At first, it's only the head.
	startrow = startcol = 1;
	endrow = endcol = 1;
	ebalda_himself[ (width + 2) * headrow + headcol ] = 'B';
	grown = false;
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
bool Ebalda::LoadState(const char *fname)
{
	short temp = 0;
	char buf[6];

	FILE *file = fopen(fname, "rb");
	if (!file)
		return false;

	fread(buf, 6, 1, file);
	if (strncmp(buf, "EBAlDA", 6))
		return false;

	fread(&temp, 1, 1, file); // NOT PORTABLE
	if (temp == 0)
		return false;
	name = new char [temp];
	if (!name)
		return false;

	fread(name, temp, 1, file);
	fread(&step, 4, 1, file);
	fread(&headrow, 2, 1, file);
	fread(&headcol, 2, 1, file);
	fread(&width, 2, 1, file);
	fread(&height, 2, 1, file);
	allocate_himself();

	startrow = startcol = 1;
	endrow = height;
	endcol = width;
	grown = false;
	if (headrow == 0) headrow = 1;
	if (headcol == 0) headcol = 1;

#ifdef DEBUG
	fprintf(stderr, "**BEFORE LOAD**\n");
	dump();
#endif

	// Now the funky read loop.
	for(int row = startrow; row <= endrow; row++)
	{
#ifdef DEBUG
	    fprintf(stderr, "reading at %d for %d\n",(width + 2) * row + startcol, endcol - startcol + 1);
#endif
	  fread(&ebalda_himself[ (width + 2) * row + startcol ], endcol - startcol + 1, 1, file);
	}

	fclose(file);

#ifdef DEBUG
	fprintf(stderr, "**LOADED**\n");
	dump();
#endif

	return true;
}

// Save ebalda to file.
void Ebalda::SaveState(const char *fname)
{
	FILE *file = fopen(fname, "wb");
	if (!file)
		return;

	short temp;
	fwrite("EBAlDA", 6, 1, file);
	temp = strlen(name) + 1; // ughmm, this relies on a) strlen(name) being < 255, b) x86 arch to take address of LSB of temp
	fwrite(&temp, 1, 1, file); // NOT PORTABLE
	fwrite(name, strlen(name)+1, 1, file);
	fwrite(&step, 4, 1, file);
	fwrite(&headrow, 2, 1, file);
	fwrite(&headcol, 2, 1, file);
	temp = endcol - startcol + 1; // new width
	fwrite(&temp, 2, 1, file);
	temp = endrow - startrow + 1; // new height
	fwrite(&temp, 2, 1, file);

	// Now the funky write loop.
	for(int row = startrow; row <= endrow; row++)
	  fwrite(&ebalda_himself[ (width + 2) * row + startcol ], endcol - startcol + 1, 1, file);

	fclose(file);

#ifdef DEBUG
	fprintf(stderr, "**TRANSFORMED**\n");
	dump();
#endif
}

void Ebalda::Grow()
{
	if (grown)
		return; // Save us from biiig memory trouble.

	int which = rand() % 2;
	int dir = rand() % 2;

#ifdef DEBUG
	fprintf(stderr, "Prev row: %d prev col: %d\n", headrow, headcol);
#endif
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
#ifdef DEBUG
	fprintf(stderr, "Next row: %d next col: %d\n\n", headrow, headcol);
#endif

	char *p = &ebalda_himself[ (width + 2) * headrow + headcol ];
	(*p)++;
	if (*p > 'J') *p = 'J';

	step++;
	grown = true;
}

void Ebalda::Draw(const char *outfile)
{
    gdImagePtr im;
    FILE *pngout;
    int black, white;
    char buf[512];
    
    snprintf(buf, 512, "%s Age: %d Size %d x %d", name, step, endcol - startcol + 1, endrow - startrow + 1);
    
    im = gdImageCreateTrueColor((width + 1) * DOT_SIZE, (height + 1) * DOT_SIZE + 20);
    white = gdImageColorAllocate(im, 255, 255, 255); // first allocated color is background
    black = gdImageColorAllocate(im, 0, 0, 0);
    
    int brect[8];
    gdImageStringFT(im, brect, white, (char *)FONT_FILE, 11.0, 0.0, /*x*/5, /*y*/(height + 1) * DOT_SIZE + 10, buf);

    for(int row = startrow; row <= endrow; row++)
    {
	for(int col = startcol; col <= endcol; col++)
	{
	    int color;
	    int c = ebalda_himself[ (width + 2) * row + col ] - 'A';
	    if (row == headrow && col == headcol) color = gdImageColorAllocate(im, 255, 0, 0);
	    else                                  color = gdImageColorAllocate(im, 250 - c * 25, 250 - c * 25, 250 - c * 25);
	    gdImageFilledRectangle(im, (col - startcol) * DOT_SIZE, (row - startrow) * DOT_SIZE,
	                               (col - startcol) * DOT_SIZE + DOT_SIZE - 1, (row - startrow) * DOT_SIZE + DOT_SIZE - 1, color);
	}
    }

    pngout = fopen(outfile, "wb");
    gdImagePng(im, pngout);
    
    fclose(pngout);
    gdImageDestroy(im);
}
