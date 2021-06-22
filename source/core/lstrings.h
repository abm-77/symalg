#pragma once

#define CharAt(str, idx) str.data[idx]
#define MakeString(str) String { (char*) str, strlen(str) }

// NOTE(bryson): Strings
typedef struct String {
    char* data;
    size_t size;
} String;


typedef struct StringBuilder {
    char* data;
    size_t size;
    size_t capacity;
    b32 valid;

	// 	NOTE(bryson): 
	// 	All "intra"-builder functions (create, append, prepend, etc.) utilize this arena. 
	// 	"extra"-builder functions (i.e. tostring, clone, etc.) can take an alternative arena
	// 	In most cases this should be used in conjuction with a temporary arena.	
	M_Arena* arena;
} StringBuilder;