#pragma once

// TODO(bryson): implement map from scratch?
#include <map>

#define NEW_LINE printf("\n")
#define INVALID_SYMBOL {0}

typedef enum SymbolType {
	SYMBOL_INVALID,
	SYMBOL_NUMERIC,
	SYMBOL_VARIABLE,
	SYMBOL_BINOP,
} SymbolType;

typedef struct Symbol {
	SymbolType type;
	void* value;
} Symbol;

typedef enum BinOpType {
	BINOP_SET,
	BINOP_SUB,
	BINOP_ADD,
	BINOP_DIV,
	BINOP_MUL,
	BINOP_EXP,
} BinOpType;

typedef struct BinOp {
	BinOpType operation;
	b32 associative;
	Symbol lhs;
	Symbol rhs;
} BinOp;

typedef struct Token {
	b32 is_numeric;
	String value;
} Token;

enum { MAX_NUM_TOKENS = 1024 };
typedef struct TokenArray {
	Token tokens [MAX_NUM_TOKENS];
	u64 token_count;
} TokenArray;

typedef struct DescentParseData {
	Symbol symbol;
	u64 next_index;
} DescentParseData;

// TODO(bryson): possibly make variables multi character
typedef std::map<char, Symbol*> Context;


// NOTE(bryson): Forward Declare (bc these call between each other)
internal Symbol EvaluateBinop(BinOp* binop, Context* context, b32 persist_data);
internal Symbol EvaluateSymbol(Symbol* symbol, Context* context, b32 persist_data);
internal Symbol SimplifyBinop(BinOp* binop);
internal Symbol SimplifySymbol(Symbol* symbol);
internal String BinOpToString(BinOp* binop);
internal String SymbolToString(Symbol* symbol);
internal void PrintSymbol(Symbol* symbol);
