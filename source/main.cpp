#include <windows.h>
#include "lunar_inc.inl"

#include "symalg.h"

typedef struct Program {
	b32 running;
	Context context;
	M_Arena permanent_memory;
	M_Arena transient_memory;
} Program;

global Program program;

#include "symalg.cpp"

int main (void) {
	// NOTE(bryson): Init program and memory.
	program.running = true;
	program.permanent_memory = M_ArenaMake();
	program.transient_memory = M_ArenaMake();

	printf("Welcome to the SymAlg Calculator! This solver with evaluate and simplify basic algebraic expressions (+, -, /, *, ^).\n");
	printf("Any expression entered must be contained in parentheses (i.e (10 + 5), ((1 + 2) / (3 - 1)), etc.). \n");
	NEW_LINE;

	printf("The solver has basic variable support.\n");
	printf("Variables are considered length-1 strings and are case sensitive.\n");
	printf("The syntax to set the value of a variable is: (VAR = VALUE). Where VAR is the name of the variable, and VALUE is any valid symbolic expression.\n");
	NEW_LINE;

	printf("Type 'quit' to exit.\n");
	printf("Type 'clear' to clear all stored variables.\n");
	NEW_LINE;

	// NOTE(bryson): Main loop.
	enum { MAX_LINE_LENGTH = 1024 };
	while (program.running) {
		M_TempArenaSavepoint frame = M_TempArenaBegin(&program.transient_memory);

		printf("Please Enter An Expression: ");

		char buffer[MAX_LINE_LENGTH];
		fgets(buffer, MAX_LINE_LENGTH, stdin);
		String expression = MakeString(buffer);

		// NOTE(bryson): Stop the program.
		if (StringMatch(StringToUpper(expression, &program.transient_memory), MakeString("QUIT\n"))) {
			program.running = false;
		}

		// NOTE(bryson): Clear all stored varaibles. Effectively clears permanent program memory.
		else if (StringMatch(StringToUpper(expression, &program.transient_memory), MakeString("CLEAR\n"))) {
			program.context.clear();
			M_ArenaPopTo(&program.permanent_memory, 0);
		}

		// NOTE(bryson): Parse user input.
		else {
			TokenArray tokens = TokenizeExpression(expression);
			Symbol out = ParseTokens(&tokens);
			PrintSymbol(&SimplifySymbol(&EvaluateSymbol(&out, &program.context, false)));
			NEW_LINE;
		}

		M_TempArenaEnd(frame);
	}

	// NOTE(bryson): Release arenas.
	M_ArenaRelease(&program.transient_memory);
	M_ArenaRelease(&program.permanent_memory);
	return 0;
}
