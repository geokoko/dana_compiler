%{
	#include <cstdio>
	#include <cstdlib>
	#include <cstring>
	#include <cctype>

	/* TOKEN definitions */
	#define T_EOF 0				// EOF

	/* KEYWORDS */
	#define T_AND 1001
	#define T_AS 1002
	#define T_BEGIN 1003	
	#define T_BREAK 1004
	#define T_BYTE 1005
	#define T_CONT 1006			// CONTINUE
	#define T_DECL 1007
	#define T_DEF 1008
	#define T_ELIF 1009
	#define T_ELSE 1010
	#define T_END 1011
	#define T_EXIT 1012
	#define T_FALSE 1013
	#define T_IF 1014
	#define T_IS 1015
	#define T_INT 1016
	#define T_LOOP 1017
	#define T_NOT 1018
	#define T_OR 1019
	#define T_REF 1020
	#define T_RET 1021			// RETURN
	#define T_SKIP 1022
	#define T_TRUE 1023
	#define T_VAR 1024

	/* IDENTIFIERS */
	#define T_ID 2001			// general identifier: var names, function names etc.
	#define T_INT_CONST 2002	// number (eg. 42). Treated as a constant integer
	#define T_CHAR_CONST 2003	// character constant
	#define T_STRING_CONST 2004	// String (eg. "abc"). Treated as a constant string

	/* OPERATORS (multi-char) */
	#define T_ASSIGN 3001		// :=
	#define T_NE 3002			// <>
	#define T_LE 3003			// <=
	#define T_GE 3004			// >=

	/* special tokens */
	#define T_AUTO_END 5001

	/* global variables */
	int lineno = 1;				// keeps tracks of the current line we are scanning
	int columno = 1;			// keeps track of the current column (def: the first column where a non whitespace character is met)

	/* Layout management
	 * Definition: Column of a token refers to the column where its first character is encountered in the code
	 */
	#define MAX_INDENT_LEVELS 500

	int indentStack[MAX_INDENT_LEVELS];
	int cur_indent_level = 1;
	int pendingAutoEnds = 0;
	bool atLineStart = true;

	// Initialize the stack
	void initStack() {
		indentStack[0] = -1;
	}

	// Guide token encountered (push)
	void pushGuide (const char* guide_name, int guide_col) {
		if (cur_indent_level >= MAX_INDENT_LEVELS) {
			fprintf(stderr, "Error: Too many nested levels at line %d\n", lineno);
			exit(1);
		}

		indentStack[cur_indent_level++] = guide_col;
		printf("Guide pushed: %s at column %d\n", guide_name, guide_col);
	}

	// Go up one indentation level. 2 possibilities for usage of this:
	// a. Next token is first in a new line and it is located in a column lefter than the column of the guide word
	// b. EOF encountered
	void popGuide () {
		if (cur_indent_level > 1) {
			indentStack[--cur_indent_level] = -1;
		}
	}

	// Check indentation status at line start
	void checkIndentation (int curr_col) {
		if (!atLineStart) return;

		while (cur_indent_level > 1 && curr_col <= indentStack[cur_indent_level - 1]) {
			popGuide();
			pendingAutoEnds++;
		}
	}

	// Handle newline - reset column and set line start flag
	void handleNewLine() {
		lineno++;
		columno = 1;
		atLineStart = true;
	}

	// Escape processing functions
	char processEscape(const char* text, int pos) {
		char escape_char = text[pos + 1];
		
		switch (escape_char) {
			case 'n':  return '\n';
			case 't':  return '\t';
			case 'r':  return '\r';
			case '0':  return '\0';
			case '\\': return '\\';
			case '\'': return '\'';
			case '"':  return '"';
			case 'x':  // Hex escape \xnn
				if (pos + 3 < strlen(text)) {
					int val1 = (text[pos + 2] >= '0' && text[pos + 2] <= '9') ? text[pos + 2] - '0' :
							   (text[pos + 2] >= 'a' && text[pos + 2] <= 'f') ? text[pos + 2] - 'a' + 10 :
							   (text[pos + 2] >= 'A' && text[pos + 2] <= 'F') ? text[pos + 2] - 'A' + 10 : -1;
					int val2 = (text[pos + 3] >= '0' && text[pos + 3] <= '9') ? text[pos + 3] - '0' :
							   (text[pos + 3] >= 'a' && text[pos + 3] <= 'f') ? text[pos + 3] - 'a' + 10 :
							   (text[pos + 3] >= 'A' && text[pos + 3] <= 'F') ? text[pos + 3] - 'A' + 10 : -1;
					if (val1 != -1 && val2 != -1) {
						return (char)(val1 * 16 + val2); // here returning ASCII char
					}
				}
				fprintf(stderr, "Error: Invalid hex escape at line %d\n", lineno);
				return 0;
			default:
				fprintf(stderr, "Warning: Unknown escape sequence \\%c at line %d\n", escape_char, lineno);
				return escape_char;
		}
	}

	void processStringEscapes(char* input) {
		int len = strlen(input);
		char* result = (char*)malloc(len + 1);
		int i = 1; // Skip opening quote
		int j = 0;
		
		while (i < len - 1) { // Until closing quote
			if (input[i] == '\\') {
				result[j++] = processEscape(input, i);
				// Skip escape sequence
				if (input[i+1] == 'x') {
					i += 4; // \xnn
				} else {
					i += 2; // \c
				}
			} else {
				result[j++] = input[i++];
			}
		}
		result[j] = '\0';
		
		// Replace yytext content with processed string
		strcpy(yytext, result);
		free(result);
	}

%}

DIGIT [0-9]
LETTER [a-zA-Z]
ID_CHAR [a-zA-Z0-9_]

%%

"def"					{ 
	/* Guide token */
	if (atLineStart) checkIndentation(columno);
	pushGuide("def", columno); 
	atLineStart = false;
	columno += yyleng;
	return T_DEF; 
}
"if"					{ 
	/* Guide token */
	if (atLineStart) checkIndentation(columno);
	pushGuide("if", columno); 
	atLineStart = false;
	columno += yyleng;
	return T_IF; 
}
"elif"					{ 
	/* Guide token */
	if (atLineStart) checkIndentation(columno);
	pushGuide("elif", columno); 
	atLineStart = false;
	columno += yyleng;
	return T_ELIF; 
}
"else"					{ 
	/* Guide token */
	if (atLineStart) checkIndentation(columno);
	pushGuide("else", columno); 
	atLineStart = false;
	columno += yyleng;
	return T_ELSE; 
}
"loop"					{ 
	/* Guide token */
	if (atLineStart) checkIndentation(columno);
	pushGuide("loop", columno); 
	atLineStart = false;
	columno += yyleng;
	return T_LOOP; 
}

"and"					{ 
	/* Keyword */
	if (atLineStart) checkIndentation(columno);
	atLineStart = false;
	columno += yyleng;
	return T_AND; 
}
"is"					{ 
	if (atLineStart) checkIndentation(columno);
	atLineStart = false;
	columno += yyleng;
	return T_IS; 
}
"skip"					{ 
	if (atLineStart) checkIndentation(columno);
	atLineStart = false;
	columno += yyleng;
	return T_SKIP; 
}
"as"					{ 
	if (atLineStart) checkIndentation(columno);
	atLineStart = false;
	columno += yyleng;
	return T_AS; 
}
"int"					{ 
	if (atLineStart) checkIndentation(columno);
	atLineStart = false;
	columno += yyleng;
	return T_INT; 
}
"true"					{ 
	if (atLineStart) checkIndentation(columno);
	atLineStart = false;
	columno += yyleng;
	return T_TRUE; 
}
"begin"					{ 
	if (atLineStart) checkIndentation(columno);
	atLineStart = false;
	columno += yyleng;
	return T_BEGIN; 
}
"var"					{ 
	if (atLineStart) checkIndentation(columno);
	atLineStart = false;
	columno += yyleng;
	return T_VAR; 
}
"break"					{ 
	if (atLineStart) checkIndentation(columno);
	atLineStart = false;
	columno += yyleng;
	return T_BREAK; 
}
"end"					{ 
	if (atLineStart) checkIndentation(columno);
	atLineStart = false;
	columno += yyleng;
	return T_END; 
}
"not"					{ 
	if (atLineStart) checkIndentation(columno);
	atLineStart = false;
	columno += yyleng;
	return T_NOT; 
}
"byte"					{ 
	if (atLineStart) checkIndentation(columno);
	atLineStart = false;
	columno += yyleng;
	return T_BYTE; 
}
"exit"					{ 
	if (atLineStart) checkIndentation(columno);
	atLineStart = false;
	columno += yyleng;
	return T_EXIT; 
}
"or"					{ 
	if (atLineStart) checkIndentation(columno);
	atLineStart = false;
	columno += yyleng;
	return T_OR; 
}
"continue"				{ 
	if (atLineStart) checkIndentation(columno);
	atLineStart = false;
	columno += yyleng;
	return T_CONT; 
}
"false"					{ 
	if (atLineStart) checkIndentation(columno);
	atLineStart = false;
	columno += yyleng;
	return T_FALSE; 
}
"ref"					{ 
	if (atLineStart) checkIndentation(columno);
	atLineStart = false;
	columno += yyleng;
	return T_REF; 
}
"decl"					{ 
	if (atLineStart) checkIndentation(columno);
	atLineStart = false;
	columno += yyleng;
	return T_DECL; 
}
"return"				{ 
	if (atLineStart) checkIndentation(columno);
	atLineStart = false;
	columno += yyleng;
	return T_RET; 
}

{DIGIT}+				{ 
	/* Integer constant */
	if (atLineStart) checkIndentation(columno);
	atLineStart = false;
	columno += yyleng;
	return T_INT_CONST; 
}

'([^\'\\]|\\.)'			{
	/* Character constant with escape handling */
	if (atLineStart) checkIndentation(columno);
	atLineStart = false;
	columno += yyleng;
	if (yytext[1] == '\\')
		return processEscape(yytext, 1);
	else
		return yytext[1];
}

{LETTER}{ID_CHAR}*		{ 
	/* Identifier */
	if (atLineStart) checkIndentation(columno);
	atLineStart = false;
	columno += yyleng;
	return T_ID; 
}


\"([^\"\\]|\\.)*\"		{
	/* String constant with escape handling */
	if (atLineStart) checkIndentation(columno);
	atLineStart = false;
	columno += yyleng;
	processStringEscapes(yytext);
	return T_STRING_CONST;
}


":="                    { 
	/* Assignment operator */
	if (atLineStart) checkIndentation(columno);
	atLineStart = false;
	columno += yyleng;
	return T_ASSIGN; 
}
"<>"                    { 
	/* Not equal operator */
	if (atLineStart) checkIndentation(columno);
	atLineStart = false;
	columno += yyleng;
	return T_NE; 
}
"<="                    { 
	/* Less than or equal operator */
	if (atLineStart) checkIndentation(columno);
	atLineStart = false;
	columno += yyleng;
	return T_LE; 
}
">="                    { 
	/* Greater than or equal operator */
	if (atLineStart) checkIndentation(columno);
	atLineStart = false;
	columno += yyleng;
	return T_GE; 
}


[\=\<\>\+\-\*\/\%\(\)\[\]\,\:]    { 
	/* Single-character operators */
	if (atLineStart) checkIndentation(columno);
	atLineStart = false;
	columno += yyleng;
	return yytext[0]; 
}


"!"						{ 
	/* Bitwise NOT operator */
	if (atLineStart) checkIndentation(columno);
	atLineStart = false;
	columno += yyleng;
	return '!'; 
}
"|"						{ 
	/* Bitwise OR operator */
	if (atLineStart) checkIndentation(columno);
	atLineStart = false;
	columno += yyleng;
	return '|'; 
}
"&" 					{ 
	/* Bitwise AND operator */
	if (atLineStart) checkIndentation(columno);
	atLineStart = false;
	columno += yyleng;
	return '&'; 
}

[ ]+					{ columno += yyleng; /* ignore spaces but track column */ }
\r						{ /* ignore carriage return */ }
\t						{ columno += 8; /* assuming tab width of 8 */ }
\n						{ handleNewLine(); }

#.*						{ /* ignore line comments */ }

.						{ 
	/* Error handling for illegal characters */
	fprintf(stderr, "Error: Illegal character '%c' at line %d, column %d\n", yytext[0], lineno, columno);
	columno++; 
}

%%

int yywrap() { return 1; }

/* Wrapper for yylex to handle <auto-end>s correctly */
int yylex_wrapper () {
	if (pendingAutoEnds > 0) {
		pendingAutoEnds--;
		return T_AUTO_END;
	}

	int token = yylex();

	// At end of file, emit any remaining AUTO_END tokens
	if (token == 0 && cur_indent_level > 1) {
		pendingAutoEnds = cur_indent_level - 1;
		cur_indent_level = 1;
		if (pendingAutoEnds > 0) {
			pendingAutoEnds--;
			return T_AUTO_END;
		}
	}

	return token;
}

int main() {
	initStack();  // Initialize the indentation stack
	
	int token;
	printf("Dana Lexer Test\n");
	printf("===============\n");
	
	do {
		token = yylex_wrapper();
		
		switch(token) {
			case T_EOF:
				printf("EOF\n");
				break;
			case T_AUTO_END:
				printf("AUTO_END\n");
				break;
			case T_INT_CONST:
				printf("INT_CONST: %s\n", yytext);
				break;
			case T_CHAR_CONST:
				printf("CHAR_CONST: %s\n", yytext);
				break;
			case T_STRING_CONST:
				printf("STRING_CONST: %s\n", yytext);
				break;
			case T_ID:
				printf("IDENTIFIER: %s\n", yytext);
				break;
			default:
				if (token >= 1000) {
					printf("KEYWORD/TOKEN: %d (%s)\n", token, yytext);
				} else {
					printf("OPERATOR: '%c' (ASCII %d)\n", token, token);
				}
				break;
		}
	} while (token != T_EOF);
	
	return 0;
}
