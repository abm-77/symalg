
// NOTE(bryson): Helpers
internal inline b32 NumEqual (Symbol* numeric, f32 value) {
	return 	numeric->type == SYMBOL_NUMERIC && 
			*((f32*)numeric->value) == value;
}
internal inline b32 IsNum (Symbol symbol) {
	return symbol.type == SYMBOL_NUMERIC;
}
internal inline f32 NumCast (Symbol* numeric) {
	return *((f32*) numeric->value);
}
internal inline char CharCast (Symbol* variable) {
	return *((char*) variable->value);
}
internal void SetVariable (Context* context, char variable, Symbol value) {
	// NOTE(bryson): we have to copy the variable's mapped symbol into permanent memory so we do not lose it between frames.
	Symbol* persisted_symbol = (Symbol*) M_ArenaPushZero(&program.permanent_memory, sizeof(Symbol));

	persisted_symbol->type = value.type;
	persisted_symbol->value = value.value;

	(*context)[variable] = persisted_symbol;
}

// NOTE(bryson): Constructors
internal Symbol Def (Symbol lhs, Symbol rhs, b32 persist_data) {
	Symbol result = {};
	result.type = SYMBOL_BINOP;

	BinOp* op = (BinOp*) M_ArenaPushZero( (persist_data) ? &program.permanent_memory : &program.transient_memory, sizeof(BinOp));
	op->operation = BINOP_DEF;
	op->lhs = lhs;	
	op->rhs = rhs;
	op->associative = false;

	result.value = op;
	return result;
}
internal Symbol Exp (Symbol lhs, Symbol rhs, b32 persist_data) {
	Symbol result = {};
	result.type = SYMBOL_BINOP;

	BinOp* op = (BinOp*) M_ArenaPushZero( (persist_data) ? &program.permanent_memory : &program.transient_memory, sizeof(BinOp));
	op->operation = BINOP_EXP;
	op->lhs = lhs;	
	op->rhs = rhs;
	op->associative = false;

	result.value = op;
	return result;
}
internal Symbol Add (Symbol lhs, Symbol rhs, b32 persist_data) {
	Symbol result = {};
	result.type = SYMBOL_BINOP;

	BinOp* op = (BinOp*) M_ArenaPushZero( (persist_data) ? &program.permanent_memory : &program.transient_memory, sizeof(BinOp));
	op->operation = BINOP_ADD;
	op->lhs = lhs;	
	op->rhs = rhs;
	op->associative = true;

	result.value = op;
	return result;
}
internal Symbol Sub (Symbol lhs, Symbol rhs, b32 persist_data) {
	Symbol result = {};
	result.type = SYMBOL_BINOP;

	BinOp* op = (BinOp*) M_ArenaPushZero( (persist_data) ? &program.permanent_memory : &program.transient_memory, sizeof(BinOp));
	op->operation = BINOP_SUB;
	op->lhs = lhs;	
	op->rhs = rhs;
	op->associative = false;

	result.value = op;

	return result;
}
internal Symbol Mul (Symbol lhs, Symbol rhs, b32 persist_data) {
	Symbol result = {};
	result.type = SYMBOL_BINOP;

	BinOp* op = (BinOp*) M_ArenaPushZero( (persist_data) ? &program.permanent_memory : &program.transient_memory, sizeof(BinOp));
	op->operation = BINOP_MUL;
	op->lhs = lhs;	
	op->rhs = rhs;
	op->associative = true;

	result.value = op;
	return result;
}
internal Symbol Div (Symbol lhs, Symbol rhs, b32 persist_data) {
	Symbol result = {};
	result.type = SYMBOL_BINOP;

	BinOp* op = (BinOp*) M_ArenaPushZero( (persist_data) ? &program.permanent_memory : &program.transient_memory, sizeof(BinOp));
	op->operation = BINOP_DIV;
	op->lhs = lhs;	
	op->rhs = rhs;
	op->associative = false;

	result.value = op;
	return result;
}
internal Symbol Numeric (f32 value, b32 persist_data) {
	Symbol result = {};
	result.type = SYMBOL_NUMERIC;
	result.value = (f32*) M_ArenaPushZero( (persist_data) ? &program.permanent_memory : &program.transient_memory, sizeof(f32));
	*((f32*)result.value) = value;
	return result;
}
internal Symbol Variable (char variable_name, b32 persist_data) {
	Symbol result = {};
	result.type = SYMBOL_VARIABLE;
	result.value = (char*) M_ArenaPushZero( (persist_data) ? &program.permanent_memory : &program.transient_memory, sizeof(char));
	*((char*)result.value) = variable_name;
	return result;
}

internal String BinOpToString (BinOp* binop) {
	M_TempArenaSavepoint temp = M_TempArenaBegin(&program.transient_memory);

	StringBuilder builder = StringBuilderCreate(&program.transient_memory, 64);
	Symbol lhs = binop->lhs;
	Symbol rhs = binop->rhs;

	if (lhs.type == SYMBOL_BINOP && ((BinOp*)lhs.value)->operation < binop->operation) {
			StringBuilderAppend(&builder, MakeString("("));
			StringBuilderAppend(&builder, SymbolToString(&lhs));
			StringBuilderAppend(&builder, MakeString(")"));
	}
	else {
		StringBuilderAppend(&builder, SymbolToString(&lhs));
	}

	switch (binop->operation) {
		case BINOP_SUB: {
			StringBuilderAppend(&builder, MakeString(" - "));
		} break;

		case BINOP_ADD: {
			StringBuilderAppend(&builder, MakeString(" + "));
		} break;

		case BINOP_DIV: {
			StringBuilderAppend(&builder, MakeString(" / "));
		} break;

		case BINOP_MUL: {
			StringBuilderAppend(&builder, MakeString(" * "));
		} break;
		case BINOP_EXP: {
			StringBuilderAppend(&builder, MakeString("^"));
		} break;
		case BINOP_DEF: {
			StringBuilderAppend(&builder, MakeString(" = "));
		} break;
	}

	if 	((rhs.type == SYMBOL_BINOP) &&
		((binop->associative 		&& ((BinOp*)rhs.value)->operation 	< 	binop->operation)  ||
		(!binop->associative 		&& ((BinOp*)rhs.value)->operation 	<= 	binop->operation))) {
		StringBuilderAppend(&builder, MakeString("("));
		StringBuilderAppend(&builder, SymbolToString(&rhs));
		StringBuilderAppend(&builder, MakeString(")"));
	}
	else {
		StringBuilderAppend(&builder, SymbolToString(&rhs));
	}

	String result = StringBuilderToStringEx(builder, &program.transient_memory);

	M_TempArenaEnd(temp);

	return result;
}

internal String SymbolToString (Symbol* symbol) {
	String str;

	switch (symbol->type) {
		case SYMBOL_NUMERIC: {
			// NOTE(bryson): No one would ever input a number more than 1024 digits, right?
			char buffer [1024];
			sprintf(buffer, "%.2f",  NumCast(symbol));
			str = MakeString(buffer);
		} break;

		case SYMBOL_VARIABLE: {
			// TODO(bryson): Find a better way?
			char buffer [1];
			sprintf(buffer, "%c", CharCast(symbol));
			str = MakeString(buffer);
		} break;

		case SYMBOL_BINOP: {
			str = BinOpToString((BinOp*)symbol->value);
		} break;

		default: {
			str = MakeString("Invalid expression.");
		} break;
	}

	return str;
}

internal void PrintSymbol (Symbol* symbol) {
	const char* str = StringToCString(SymbolToString(symbol), &program.transient_memory);
	printf(">> %s\n", str);
}

internal Symbol EvaluateBinOp (BinOp* binop, Context* context, b32 persist_data) {
	Symbol result = {};
	Symbol elhs = {};
	Symbol erhs = {};

	if (binop->operation == BINOP_DEF) {
		// Assuming lhs is simply a variable
		elhs = binop->lhs;
		persist_data = true; 
	}
	else {
		elhs = EvaluateSymbol(&binop->lhs, context, persist_data);
	}
	erhs = EvaluateSymbol(&binop->rhs, context, persist_data);

	switch (binop->operation) {
		case BINOP_SUB: {
			result = Sub(elhs, erhs, persist_data);
		} break;
		case BINOP_ADD: {
			result = Add(elhs, erhs, persist_data);
		} break;
		case BINOP_DIV: {
			result = Div(elhs, erhs, persist_data); 
		} break;
		case BINOP_MUL: {
			result = Mul(elhs, erhs, persist_data);
		} break;
		case BINOP_EXP: {
			result = Exp(elhs, erhs, persist_data);
		} break;
		case BINOP_DEF: {
			if (elhs.type == SYMBOL_VARIABLE) {
				result = Def(elhs, erhs, persist_data); 
				SetVariable(context, CharCast(&elhs), erhs);
			}
		} break;
	}

	return result;
}

internal Symbol EvaluateSymbol (Symbol* symbol, Context* context, b32 persist_data) {
	Symbol result = {};

	switch (symbol->type) {
		case SYMBOL_NUMERIC: {
			result = *symbol;
		} break;

		case SYMBOL_VARIABLE: {
			char var_name = CharCast(symbol);
			if (context->find(var_name) != context->end()) {
				result = *((*context)[var_name]);
			}
			else {
				result = *symbol;
			}
		} break;

		case SYMBOL_BINOP: {
			result = EvaluateBinOp((BinOp*) symbol->value, context, persist_data);
		} break;
	}

	return result;
}

internal Symbol SimplifyBinOp (BinOp* binop) {
	Symbol result = {};
	Symbol slhs = SimplifySymbol(&binop->lhs); 
	Symbol srhs = SimplifySymbol(&binop->rhs); 

	b32 lhsn = IsNum(slhs);
	b32 rhsn = IsNum(srhs);

	switch (binop->operation) {
		case BINOP_SUB: {
			if (lhsn && rhsn) {
				// Ex: 1 - 1
				result = Numeric (NumCast(&slhs) - NumCast(&srhs), false);
			}
			else if (NumEqual(&binop->rhs, 0)) {
				// Ex: x - 0
				result = slhs;
			}
			else {
				result = Sub(SimplifySymbol(&slhs), SimplifySymbol(&srhs), false);
			}
		} break;
		case BINOP_ADD: {
			if (lhsn && rhsn) {
				// Ex: 1 + 1
				result = Numeric (NumCast(&slhs) + NumCast(&srhs), false);
			}
			else if (NumEqual(&binop->lhs, 0)) {
				// Ex: 0 + x 
				result = srhs;
			}
			else if (NumEqual(&binop->rhs, 0)) {
				// Ex: x + 0
				result = slhs; 
			}
			else {
				result = Add(SimplifySymbol(&slhs), SimplifySymbol(&srhs), false);
			}
		} break;
		case BINOP_DIV: {
			if (lhsn && rhsn) {
				// Ex: 1 / 1
				result = Numeric (NumCast(&slhs) / NumCast(&srhs), false);
			}
			else if (NumEqual(&binop->rhs, 1)) {
				// Ex: x / 1 
				result = slhs;
			}
			else if (NumEqual(&binop->lhs, 0)) {
				// Ex: 0 / x 
				result =  Numeric(0, false);
			}
			else {
				result = Div(SimplifySymbol(&slhs), SimplifySymbol(&srhs), false);
			}
		} break;
		case BINOP_MUL: {
			if (lhsn && rhsn) {
				// Ex: 1 * 1
				result = Numeric (NumCast(&slhs) * NumCast(&srhs), false);
			}
			else if (NumEqual(&binop->lhs, 0) || NumEqual(&binop->rhs, 0)) {
				// Ex: 0 * x  or x * 0
				result = Numeric (0, false); 
			}
			else if (NumEqual(&binop->rhs, 1)) {
				// Ex: x * 1
				result = slhs;
			}
			else if (NumEqual(&binop->lhs, 1)) {
				// Ex: 1 * x
				result = srhs;
			}
			else {
				result = Mul(SimplifySymbol(&slhs), SimplifySymbol(&srhs), false);
			}
		} break;
		case BINOP_EXP: {
			if (lhsn && rhsn) {
				// Ex: 2^3
				result = Numeric (powf(NumCast(&slhs), NumCast(&srhs)), false);
			}
			else if (NumEqual(&binop->lhs, 0)) {
				// Ex: 0 ^ x
				result = Numeric (0, false); 
			}
			else if (NumEqual(&binop->lhs, 1) || NumEqual(&binop->rhs, 0)) {
				// Ex: 1 ^ x, or x ^ 0
				result = Numeric(1, false); 
			}
			else if (NumEqual(&binop->rhs, 1)) {
				// Ex: x ^ 1
				result = slhs;
			}
			else {
				result = Exp(SimplifySymbol(&slhs), SimplifySymbol(&srhs), false);
			}
		} break;
		case BINOP_DEF: {
			result = Def(slhs, srhs, false);
		} break;
	}

	return result;
}

internal Symbol SimplifySymbol (Symbol* symbol) {
	Symbol result = {};

	switch (symbol->type) {
		case SYMBOL_NUMERIC: {
		case SYMBOL_VARIABLE: {
			result = *symbol;
		}} break;

		case SYMBOL_BINOP: {
			result = SimplifyBinOp((BinOp*) symbol->value);
		} break;
	}

	return result;
}

internal TokenArray TokenizeExpression (String expression) {
	M_TempArenaSavePoint temp = M_TempArenaBegin(&program.transient_memory);

	TokenArray result = {0};
	StringBuilder curr_numeric_token = StringBuilderCreate(&program.transient_memory, 64);

	auto CharIsOperator = [](char c) {
		return c == '+' || c == '-' || c == '*' || c == '/';
	};

	auto Append = [&](String str, b32 is_numeric) {
		result.tokens[result.token_count].is_numeric = is_numeric; 
		result.tokens[result.token_count].value = StringCopy(str, &program.transient_memory);
		result.token_count++;
	};

	auto Flush = [&]() {
		if (curr_numeric_token.size != 0) {
			Append(StringBuilderToString(curr_numeric_token), true);
			StringBuilderClear(&curr_numeric_token);
		}
	}; 

	for (u32 i = 0; i < expression.size; ++i) {
		char curr = CharAt(expression, i);
		String curr_str =  StringSubstring(expression, i, i+1);

		if (CharIsSpace(curr)) {
			Flush();	
		}
		else if (curr == '(' ||  curr == ')') {
			Flush();
			Append(curr_str, false);
		}
		else if (curr == '.') {
			StringBuilderAppend(&curr_numeric_token, curr_str);
		}
		else if (CharIsOperator(curr)) {
			if (curr == '-') {
				StringBuilderAppend(&curr_numeric_token, curr_str);
			}
			else {
				Append(curr_str, false);
			}
		}
		else if (CharIsDigit(curr)) {
			StringBuilderAppend(&curr_numeric_token, curr_str);
		}
		else {
			Append(curr_str, false);
		}
	}

	Flush();

	M_TempArenaEnd(temp);

	return result;
}

internal DescentParseData Parse (TokenArray* arr, u32 index, b32 persist_data) {
	DescentParseData curr_parse;

	Token token = arr->tokens[index];

	if (token.is_numeric) {
		curr_parse.symbol = Numeric(atof(StringToCString(token.value, &program.transient_memory)), persist_data);
	}
	else if (!StringMatch(token.value, MakeString("("))) {
		// NOTE(bryson): Assumes variables are length-1 strings.
		if(StringIsAlpha(token.value))
			curr_parse.symbol = Variable(token.value.data[0], persist_data); 	
	}
	else {
		DescentParseData left = Parse(arr, index + 1, persist_data); index = left.next_index;

		// NOTE(bryson): If the operation is a set, any subsequent allocations for symbolic data must be put in permanent memory.
		// TODO(bryson): see about simplifying expressions before saving them, 
		// this would cut down on how much memory is used per saved variable.
		char op = arr->tokens[index].value.data[0]; 
		if (op == '=') persist_data = true;

		DescentParseData right = Parse(arr, index + 1,  persist_data); index = right.next_index;

		if(left.symbol.type == SYMBOL_INVALID || right.symbol.type == SYMBOL_INVALID) {
			op = 0;
		}

		switch (op) {
			case '+': {
				curr_parse.symbol = Add(left.symbol, right.symbol, persist_data);
			} break;
			case '-': {
				curr_parse.symbol = Sub(left.symbol, right.symbol, persist_data);
			} break;
			case '*': {
				curr_parse.symbol = Mul(left.symbol, right.symbol, persist_data);
			} break;
			case '/': {
				curr_parse.symbol = Div(left.symbol, right.symbol, persist_data);
			} break;
			case '^': {
				curr_parse.symbol = Exp(left.symbol, right.symbol, persist_data);
			} break;
			case '=': {
				curr_parse.symbol = Def(left.symbol, right.symbol, persist_data);
			} break;
		}
	}

	curr_parse.next_index = index + 1;
	return curr_parse;
};

internal Symbol ParseTokens (TokenArray* arr) {
	DescentParseData parse = Parse(arr, 0, false);
	return parse.symbol;
}