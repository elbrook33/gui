#ifndef PARSE_H
#define PARSE_H

#include "snippet.h"
#include "draw.h"
#include <stdarg.h>

triplet parseFormatting(couplet startSplit, const char* end, snippetFn drawFn)
// startSplit: ...((...
// endSplit: ((...))...
{
	if(!startSplit.tail.start || isSpaceChar(*(startSplit.tail.start))) return tripletFail;
	
	couplet endSplit = splitAt(startSplit.tail, end);
	if(!endSplit.head.end || isSpaceChar(*(endSplit.head.end))) return tripletFail;
	
	triplet result = {
		{startSplit.head.start, startSplit.head.end, NULL},
		{endSplit.head.start, endSplit.head.end, drawFn},
		{endSplit.tail.start, endSplit.tail.end, NULL} };
	return result;
}

#define listSize 16

triplet firstMatch(snippet text, ...)
{
	const char *starts[listSize], *ends[listSize];
	snippetFn functions[listSize];
	int indices[listSize], inOrder[listSize];
	
	// va stuff
	int listLen = 0;
	va_list args;
	va_start(args, text);
	while(starts[listLen] = va_arg(args, const char*)) {
		ends[listLen] = va_arg(args, const char*);
		functions[listLen] = va_arg(args, snippetFn);
		indices[listLen] = indexOf(text, starts[listLen]);
		listLen++;
	}
	
	// Sort
	int lastMin = -1;
	for(int i = 0; i < listLen; i++) {
		int currentMin = sniplen(text);
		inOrder[i] = -1;
		for(int j = 0; j < listLen; j++) {
			if(indices[j] >=0 && indices[j] < currentMin && indices[j] > lastMin) {
				inOrder[i] = j;
				currentMin = indices[j];
			}
		}
		lastMin = currentMin;
	}
	
	// Try each match
	triplet parseResult = tripletFail;
	for(int i = 0; i < listLen; i++) {
		int format = inOrder[i];
		if(format == -1) break;
		parseResult = parseFormatting(
			splitAt(text, starts[format]),
			ends[format],
			functions[format] );
		if(parseResult.head.start && *(parseResult.head.start)) break;
	}
	return parseResult;
}

void parsePar(snippet line)
{
	assert(line.start && *(line.start) && line.start != line.end, );
	
	triplet parsed = firstMatch(line,
		"((", "))", drawFade,
		"[[", "]]", drawAccent,
		"_", "_", drawItalic,
		"**", "**", drawBold,
		"<", ">", drawTag,
		"`", "`", drawMono,
		NULL);
	
	if(!parsed.head.start) {
		drawNormal(line);
	} else {
		// head is normal
		drawNormal(parsed.head);
		
		// middle is formatted
		parsed.middle.fn(parsed.middle);
		
		// parse the rest
		parsePar(parsed.tail);
	}
}

void parse(snippet text)
{
	assert(text.start && *(text.start), );
	
	couplet lineSplit = splitAt(text, "\n");
	snippet line = lineSplit.head;
	
	snippet word1 = word(text, 1);
	if(startsWith(word1, "|")) {
		line = fromWord(line, 2);
		highlightStart();
	}

	parsePar(line);
	drawNewLine();

	if(startsWith(word1, "|")) {
		highlightEnd(sniplen(word1));
	}
	
	parse(lineSplit.tail);
}

#endif