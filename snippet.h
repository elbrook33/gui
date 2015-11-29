/*
 * Snippet - a simple library for substrings.
 * Has an extra "fn" member in the structs, used to pass specific drawing functions.
 */

#ifndef SNIPPET_H
#define SNIPPET_H

#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include "assert.h"

//
// Types
//
typedef struct snippet {
	const char *start, *end;
	void (*fn)(struct snippet);
} snippet;

typedef void (*snippetFn)(snippet);

typedef struct couplet {
	snippet head, tail;
} couplet;

typedef struct triplet {
	snippet head, middle, tail;
} triplet;

//
// Consts
//
const couplet coupletFail = {{NULL, NULL, NULL}, {NULL, NULL, NULL}};
const triplet tripletFail = {{NULL, NULL, NULL}, {NULL, NULL, NULL}, {NULL, NULL, NULL}};

//
// Constructor
//
snippet snip(const char* text)
{
	snippet newSnippet = {text, NULL, NULL};
	return newSnippet;
}

//
// Functions
//
bool startsWith(snippet A, const char* B)
{
	assert(A.start && B, false);
	while(*(A.start) && A.start != A.end && *B) {
		if(*(A.start) != *B) return false;
		A.start++;
		B++;
	}
	return true;
}

couplet splitAt(snippet text, const char* delimiter)
{
	snippet head = text, tail = text;
	while(*(tail.start) && tail.start != tail.end) {
		if(startsWith(tail, delimiter)) {
			head.end = tail.start;
			tail.start += strlen(delimiter);
			couplet result = {head, tail};
			return result;
		}
		tail.start++;
	}
	return coupletFail;
}

snippet word(snippet text, int n)
{
	couplet split = {text, text};
	while(n > 0) {
		split = splitAt(split.tail, " ");
		n--;
	}
	return split.head;
}

snippet fromWord(snippet text, int n)
{
	couplet split = {text, text};
	while(n > 1) {
		split = splitAt(split.tail, " ");
		n--;
	}
	return split.tail;
}

int sniplen(snippet text)
{
	assert(text.start, 0);
	int len = 0;
	while(*(text.start) && text.start != text.end) {
		len++;
		text.start++;
	}
	return len;
}

const char* snipcpy(snippet text, char* str)
{
	int len = sniplen(text);
	for(int i = 0; i < len; i++) {
		str[i] = text.start[i];
	}
	str[len] = '\0';
	return str;
}

bool isSpaceChar(char c)
{
	return c==' ' || c=='\t' || c=='\n' || c=='\0';
}

int indexOf(snippet text, const char* query)
{
	int i = 0;
	while(*(text.start) && text.start != text.end) {
		if(startsWith(text, query)) return i;
		i++;
		text.start++;
	}
	return -1;
}

#endif