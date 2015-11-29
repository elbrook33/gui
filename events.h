#ifndef EVENTS_H
#define #EVENTS_H

typedef struct charPos {
	int n, par; // i.e. nth-char in whole text, corresponding to par-th paragraph.
} charPos;

charPos charAtXY(panel p, float x, float y)
{
	// Go straight through every char from the top. This only happens once a click so it's no big deal.
	charPos hit = {0, 0};
	
	for(int row = 0; row < p.numRows; row++) {
		while(p.text + hit.n != row.start) {
			if(p.text[hit.n] == '\n') hit.par++;
			hit.n++;
		}
	}
}

#endif