#ifndef DRAW_H
#define DRAW_H

#include <GL/glew.h>
#define GLFW_INCLUDE_GLU
#include <GLFW/glfw3.h>
#define NANOVG_GLEW
#include "nanovg.h"
#define NANOVG_GL2_IMPLEMENTATION
#include "nanovg_gl.h"

#include "snippet.h"
void parse(snippet);
void parseLine(snippet);
#include <stdio.h>

//
// Types
//
typedef struct panel {
	float x, y, w, h, xRatio, yRatio, wRatio, hRatio, margin;
	float scrollY;
	float fontSize, lineRatio, lineHeight, descender, spaceSize, curX, curY;
	float* bounds; NVGtextRow* rows; int numRows;
	const char* text;
} panel;

//
// Globals
//
GLFWwindow* glfwWindow;
NVGcontext* nano;
int winW = 800, winH = 600, pxRatio;
panel p;
panel defaultPanel = {
	0, 0, 0, 0, 0, 0, 0.4, 1.0, 25,
	0,
	13.8, 1.25, 0, 0, 0, 0, 0,
	NULL, NULL, 0,
	NULL};

#define maxRows 4096

//
// Settings
//
#define glWhite	255, 255, 255, 255
#define bgWhite	nvgHSLA(0.5, 0.1, 1.0, 255)
#define bgLight	nvgHSLA(0.5, 0.1, 0.95, 255)
#define fgBlack	nvgHSLA(0.0, 0.0, 0.1, 255)
#define fgFade	nvgHSLA(0.5, 0.1, 0.6, 255)
#define fgAccent	nvgHSLA(0.5, 0.1, 0.4 ,255)
#define hlFade	nvgHSLA(0.5, 0.1, 0.9, 255)
#define hlFull	nvgHSLA(0.5, 0.1, 0.6, 255)

//
// Setup - boilerplate to create a new window, initalise NanoVG, and create fonts.
//
int setup()
{
	// GLFW
	if (!glfwInit()) {
		printf("Failed to init GLFW.");
		return -1;
	}
	glfwWindow = glfwCreateWindow(winW, winH, "Untitled app", NULL, NULL);
	if (!glfwWindow) {
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(glfwWindow);

	// Glew
	if(glewInit() != GLEW_OK) {
		printf("Could not initialize glew.\n");
		return -1;
	}

	// NanoVG
	nano = nvgCreateGL2(NVG_ANTIALIAS | NVG_STENCIL_STROKES);
	if (nano == NULL) {
		printf("Could not init nanovg.\n");
		return -1;
	}

	// Fonts
	if (nvgCreateFont(nano, "normal", "fonts/LiberationSans-Regular.ttf") == -1) {
		printf("Could not add font normal.\n");
		return -1;
	}
	if (nvgCreateFont(nano, "bold", "fonts/LiberationSans-Bold.ttf") == -1) {
		printf("Could not add font bold.\n");
		return -1;
	}
	if (nvgCreateFont(nano, "italic", "fonts/LiberationSans-Italic.ttf") == -1) {
		printf("Could not add font italic.\n");
		return -1;
	}
	if (nvgCreateFont(nano, "italic", "fonts/LiberationSans-Italic.ttf") == -1) {
		printf("Could not add font italic.\n");
		return -1;
	}
	if (nvgCreateFont(nano, "mono", "fonts/LiberationMono-Regular.ttf") == -1) {
		printf("Could not add font italic.\n");
		return -1;
	}
}

//
// Draw start and end - boilerplate to set up a frame and blit it.
//
void drawStart()
{
	// Check window size and calculate pixel ratio. (This could change while running.)
	int fbW, fbH;
	glfwGetWindowSize(glfwWindow, &winW, &winH);
	glfwGetFramebufferSize(glfwWindow, &fbW, &fbH);
	pxRatio = (float)fbW / (float)winW;

	// Set up OpenGL
	glViewport(0, 0, fbW, fbH);
	glClearColor(glWhite);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	// Set up NanoVG
	nvgBeginFrame(nano, winW, winH, pxRatio);
}
void drawEnd()
{
	nvgEndFrame(nano);
	glfwSwapBuffers(glfwWindow);
}

//
// Draw panel
//
void panelSetup()
{
	p.numRows = 0;

	// Update window size (if ratios aren't set, keep the absolute values.)
	if(p.xRatio > 0) p.x = winW * p.xRatio + 1;
	if(p.yRatio > 0) p.y = winH * p.yRatio;
	if(p.wRatio > 0) p.w = winW * p.wRatio;
	if(p.hRatio > 0) p.h = winH * p.hRatio;

	// Set fonts
	nvgFontSize(nano, p.fontSize);
	nvgTextLineHeight(nano, p.lineRatio);
	nvgFontFace(nano, "normal");
	
	// Update text positions
	nvgTextMetrics(nano, NULL, &p.descender, &p.lineHeight);
	float M_M[4];
	float MM[4];
	nvgTextBounds(nano, 0, 0, "M M", NULL, M_M);
	nvgTextBounds(nano, 0, 0, "MM", NULL, MM);
	p.spaceSize = M_M[3] - MM[3];
	
	// Set initial positions
	p.curX = p.x + p.margin;
	p.curY = p.y + p.margin + p.lineHeight*0.5 - p.scrollY;
}
void drawPanel(panel setPanel, const char* text)
{
	p = setPanel;
	
	panelSetup();
	p.text = text;

	parse(snip(text));
	
	// Draw line to right if not full
	if(p.x + p.w < winW) {
		nvgBeginPath(nano);
		nvgFillColor(nano, hlFull);
		nvgRect(nano, p.x + p.w, p.y, 1, p.h);
		nvgFill(nano);
	}
}
void drawBar(panel bar, const char* text)
{
	p = bar;
	
	panelSetup();
	p.text = text;

	// Set bar size
	p.h = p.lineHeight * 2;
	p.y = winH - p.h;
	p.curY = p.y + p.lineHeight * 1.5 + p.descender;
	
	// Redraw background
	nvgBeginPath(nano);
	nvgFillColor(nano, bgLight);
	nvgRect(nano, p.x, p.y, p.w, p.h);
	nvgFill(nano);
	
	parse(snip(text));
}

//
// Drawing text
//
void drawNewLine()
{
	p.curY += p.lineHeight * p.lineRatio;
	p.curX = p.x + p.margin;
}
bool drawTextLimitLines(snippet text, int limit)
{
	int rowLimit = limit < maxRows? limit : maxRows - p.numRows;
	int numLines = nvgTextBreakLines(nano, text.start, text.end, (p.x + p.w - p.margin) - p.curX, p.rows + p.numRows, rowLimit + 1);
	
	NVGtextRow *rows = p.rows + p.numRows;
	
	for(int i = 0; i < numLines && i < rowLimit; i++) {
		nvgText(nano, p.curX, p.curY, rows[i].start, rows[i].end);
		
		p.numRows++;
		nvgTextBounds(nano, p.curX, p.curY, rows[i].start, rows[i].end, p.bounds+(p.numRows-1)*4);
		
		// If there are more lines after this one, move down and left.
		if(i < numLines - 1 && i < rowLimit - 1) {
			// If this was a short (partial) line, recalculate subsequent lines.
			if(p.curX > p.x + p.margin) {
				drawNewLine();
				snippet next = {rows[i].next, text.end, NULL};
				return drawTextLimitLines(next, limit-1);
			} else {
				drawNewLine();
			}
		} else {
			p.curX = p.bounds[(p.numRows-1)*4 + 2] + p.spaceSize;
		}
	}
	return numLines > rowLimit;
}
void drawText(snippet text)
{
	drawTextLimitLines(text, maxRows);
}

//
// Draw procedures for different types of elements (text).
//
void drawNormal(snippet text)
{
	nvgFillColor(nano, fgBlack);
	drawText(text);
}
void drawFade(snippet text)
{
	nvgFillColor(nano, fgFade);
	drawText(text);
}
void drawAccent(snippet text)
{
	nvgFillColor(nano, fgAccent);
	drawText(text);
}
void drawItalic(snippet text)
{
	nvgFontFace(nano, "italic");
	nvgFillColor(nano, fgBlack);
	drawText(text);
	nvgFontFace(nano, "normal");
}
void drawBold(snippet text)
{
	nvgFontFace(nano, "bold");
	nvgFillColor(nano, fgBlack);
	drawText(text);
	nvgFontFace(nano, "normal");
}
void drawMono(snippet text)
{
	nvgFontFace(nano, "mono");
	nvgFillColor(nano, fgFade);
	drawText(text);
	nvgFontFace(nano, "normal");
}
void drawTag(snippet text)
{
	snippet type = word(text, 1);
	if(startsWith(type, "block")) {
		// grab text for lines=N
		snippet linesWord = word(text, 2);
		snippet linesNumSnip = splitAt(linesWord, "=").tail;
		
		// parse number
		char linesText[8];
		int maxLines = atoi(snipcpy(linesNumSnip, linesText));
		
		// draw lines (if chopped, draw ellipsis)
		if(drawTextLimitLines(fromWord(text, 3), maxLines)) {
			nvgText(nano, p.curX, p.curY, "…", NULL);
		}
	} else
	if(startsWith(type, "image")) {
		// One day…
	} else
	if(startsWith(type, "video")) {
		// One day…
		if(p.curX > p.x + p.margin) drawNewLine();

		nvgBeginPath(nano);
		nvgFillColor(nano, hlFull);

		float w = p.w - p.margin*2, h = w*10.0/16.0;
		nvgRect(nano, p.curX, p.curY, w, h);

		nvgFill(nano);
		p.curY += h;
	}
}

//
// Drawing highlights (boxes in the margins).
// (Particularly un-thread-safe.)
//
float highlightStartY;

void highlightStart()
{
	highlightStartY = p.curY;
}
void highlightEnd(int n)
{
	nvgBeginPath(nano);
	
	if(n<=2)	nvgFillColor(nano, hlFade);
	else	nvgFillColor(nano, hlFull);
	
	// Left
	nvgRect(nano, p.x, highlightStartY-p.lineHeight, p.margin/2, p.curY-highlightStartY+1);
	
	// Right
	if(n==3) nvgRect(nano, p.w-p.margin/2, highlightStartY-p.lineHeight, p.margin/2, p.curY-highlightStartY+1);
	
	nvgFill(nano);
}

#endif
