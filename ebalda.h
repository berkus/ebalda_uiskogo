#pragma once
#ifndef EBALDA_H
#define EBALDA_H

class Ebalda
{
	public:
		Ebalda();
		~Ebalda();
		
		void Hatch(const char *name);       ///< Hatch a fresh new ebalda with a \c name.
		bool LoadState(const char *fname);  ///< Load ebalda from file.
		void SaveState(const char *fname);  ///< Save ebalda to file.

		void Grow();
		void Draw(const char *outfile);

	private:
		char *name;
		unsigned step;
		bool grown;
		short headrow, headcol;
		short width, height;
		char *ebalda_himself; // this array is allocated with +2 rows and +2 columns to accomodate for ebalda sudden growth.
		short startrow, startcol;
		short endrow, endcol;

		void allocate_himself();
		void dump();
};

#endif
