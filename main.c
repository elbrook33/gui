/*
 * SimpleApp
 * Parses a very simple markdown (from stdin) and draws to an OpenGL context.
 *
 * Files
 * control (link to stdin)
 * events (output - link to stdout?)
 * left, middle, right (markup input)
 *
 * API (to stdin)
 * redraw
 * scroll:left/middle/right=Y
 * set-commands:key-key=Name;key…
 * quit
 * (resize:WxH)
 * (set-title:Title)
 *
 * Output
 * click:char=N;par=N;x=N;y=N
 * command:Name
 * drag:char=N;par=N;x=N;y=N
 * key:key (only if not captured as a command)
 *
 * Markup
 * | highlight (a light-coloured box in the left margin)
 * || active (light highlights in both margins)
 * ||| selected (dark on both sides)
 * **bold**
 * _italic_
 * `monospace`
 * ((fade)) (light-coloured text)
 * [[accent]] (coloured text)
 * <block lines=n text goes here…> (limit block to n lines, ending with an ellipsis)
 * <image file>
 * <video file>
 */

#include "draw.h"
#include "snippet.h"
#include "parse.h"
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>

panel left, middle, right, bar;

void drawFile(panel p, const char* fileName, int size)
{
	assert(size > 0, );

	char text[size+1];
	FILE* file = fopen(fileName, "r");
	fread(text, sizeof(char), size, file);
	fclose(file);
	text[size] = '\0';

	drawPanel(p, text);
}

void redraw()
{
	struct stat fileStats;
	stat("left", &fileStats);
	int l = fileStats.st_size;
	stat("middle", &fileStats);
	int m = fileStats.st_size;
	stat("right", &fileStats);
	int r = fileStats.st_size;
	
	if(l <= 1 && r <= 1) { middle.xRatio = middle.x = 0; middle.wRatio = 1; }
	else if(l > 1 && r > 1) { middle.xRatio = left.wRatio = right.wRatio = 0.2; middle.wRatio = 0.6; right.xRatio = 0.8; }
	else if(l <= 1) { middle.xRatio = middle.x = 0; middle.wRatio = right.xRatio = 0.6; right.wRatio = 0.4; }
	else if(r <= 1) { left.wRatio = middle.xRatio = 0.4; middle.wRatio = 0.6; }
	
	drawStart();
	drawFile(right, "right", r);
	drawFile(middle, "middle", m);
	drawFile(left, "left", l);
	drawBar(bar, "`‹`[[Reply]] `(cmd-R) ‹`[[Forward]] `(cmd-F) ‹`[[Delete]] `(cmd-X) ›` [[New]] `(cmd-N) ›`[[Quit]] `(cmd-Q)`\n");
	drawEnd();
}

int main()
{	
	setup();
	
	// Panel defaults
	left = middle = right = bar = defaultPanel;
	bar.wRatio = 1; bar.yRatio = bar.hRatio = 0; bar.margin = 5;

	// Initalise row data memory allocations
	float _lb[maxRows*4];
	float _mb[maxRows*4];
	float _rb[maxRows*4];
	float _bb[maxRows*4];
	NVGtextRow _lr[maxRows];
	NVGtextRow _mr[maxRows];
	NVGtextRow _rr[maxRows];
	NVGtextRow _br[maxRows];
	left.bounds = _lb; middle.bounds = _mb; right.bounds = _rb; bar.bounds = _bb;
	left.rows = _lr; middle.rows = _mr; right.rows = _rr; bar.rows = _br;
	
	// Go!
	redraw();
	
	char cmd[1024];
	
	// "Multiplex" UI events and stdin using "poll"
	while(scanf("%s", cmd)) {
		snippet command = snip(cmd);
		if(startsWith(command, "redraw")) { redraw(); }
		if(startsWith(command, "scroll")) {
			snippet params = splitAt(command, ":").tail;
			couplet paramSplit = splitAt(params, "=");
			int scroll = atoi(paramSplit.tail.start);
			if(startsWith(paramSplit.head, "left")) { left.scrollY = scroll; }
			if(startsWith(paramSplit.head, "middle")) { middle.scrollY = scroll; }
			if(startsWith(paramSplit.head, "right")) { right.scrollY = scroll; }
			redraw();
		}
	}
	
	return 0;
}
