%{
	#include <cstdio>
	#include <cstdlib>
	#include <cstring>
	#include <cctype>
	#include <string>

	#include "../parser/parser.tab.hh"
	#include "../common/diagnostics.hpp"
	using dana::parser;

	#define EMIT_AUTOEND_IF_NEEDED() \
    do { \
        if (atLineStart && checkIndentation(yylloc->begin.column)) { \
            yyless(0); \
            return parser::token::T_AUTO_END; \
        } \
    } while(0)

	/* This runs at the beginning of each token match */
	#define YY_USER_ACTION (yylloc->begin = yylloc->end);
	#define YY_DECL int yylex(parser::semantic_type* yylval, parser::location_type* yylloc)

	static int commentDepth = 0;

	/* Layout management
	 * Definition: Column of a token refers to the column where its first character is encountered in the code
	 */
	#define MAX_INDENT_LEVELS 500

	static int indentStack[MAX_INDENT_LEVELS] = { -1 };
	static int cur_indent_level = 1;
	static int pendingAutoEnds = 0;
	static bool atLineStart = true;
	
	/* Declaration of lexer helper functions */
	static inline void pushGuide(const char* guide_name, int guide_col, int guide_line);
	static inline void popGuide();
	static inline bool checkIndentation(int curr_col);
	static inline void handleNewLine(parser::location_type* yylloc);
	static inline char processEscape(const char* text, int pos, const parser::location_type* yylloc);
	static inline std::string processStringEscapes(const char* input, const parser::location_type* yylloc);
%}

%option noyywrap nodefault nounput

DIGIT [0-9]
LETTER [a-zA-Z]
ID_CHAR [a-zA-Z0-9_]

%x PENDING
%x COMMENT

%%

<PENDING>(.|\n) 		{
	yyless(0); /* put char back; DO NOT CONSUME */
	if (--pendingAutoEnds == 0) {
		BEGIN(INITIAL); 				/* leave PENDING state */
		return yylex(yylval, yylloc); 	/* rescan from the same position in INITIAL, */
										/* so the *real* next token is parsed and returned */
	}	

	return parser::token::T_AUTO_END;		
}

<PENDING><<EOF>> 		{
    if (pendingAutoEnds > 0) {
        --pendingAutoEnds;
        if (pendingAutoEnds == 0) {
			BEGIN(INITIAL);
			return 0; /* T_EOF */
		}
        return parser::token::T_AUTO_END;
    }
    return 0; /* T_EOF */
}

<INITIAL><<EOF>> 				{
	if (cur_indent_level > 1) {
		pendingAutoEnds = cur_indent_level - 1;
		while (cur_indent_level > 1) popGuide();
		BEGIN(PENDING);
		return parser::token::T_AUTO_END;
	}
	
	return 0;
}

"def"					{ 
	/* Guide token */
	EMIT_AUTOEND_IF_NEEDED();
	pushGuide("def", yylloc->begin.column, yylloc->begin.line);
	atLineStart = false;
	yylloc->columns(yyleng);
	return parser::token::T_DEF; 
}
"if"					{ 
	/* Guide token */
	EMIT_AUTOEND_IF_NEEDED();
	pushGuide("if", yylloc->begin.column, yylloc->begin.line); 
	atLineStart = false;
	yylloc->columns(yyleng);
	return parser::token::T_IF; 
}
"elif"					{ 
	/* Guide token */
	EMIT_AUTOEND_IF_NEEDED();
	pushGuide("elif", yylloc->begin.column, yylloc->begin.line); 
	atLineStart = false;
	yylloc->columns(yyleng);
	return parser::token::T_ELIF; 
}
"else"					{ 
	/* Guide token */
	EMIT_AUTOEND_IF_NEEDED();
	pushGuide("else", yylloc->begin.column, yylloc->begin.line); 
	atLineStart = false;
	yylloc->columns(yyleng);
	return parser::token::T_ELSE; 
}

"loop"					{ 
	/* Guide token */
	EMIT_AUTOEND_IF_NEEDED();
	pushGuide("loop", yylloc->begin.column, yylloc->begin.line); 
	atLineStart = false;
	yylloc->columns(yyleng);
	return parser::token::T_LOOP; 
}

"and"					{ 
	/* Keyword */
	atLineStart = false;
	yylloc->columns(yyleng);
	return parser::token::T_AND; 
}
"is"					{ 
	atLineStart = false;
	yylloc->columns(yyleng);
	return parser::token::T_IS; 
}
"skip"					{ 
	EMIT_AUTOEND_IF_NEEDED();
	atLineStart = false;
	yylloc->columns(yyleng);
	return parser::token::T_SKIP; 
}
"as"					{ 
	atLineStart = false;
	yylloc->columns(yyleng);
	return parser::token::T_AS; 
}
"int"					{
	EMIT_AUTOEND_IF_NEEDED();
	atLineStart = false;
	yylloc->columns(yyleng);
	return parser::token::T_INT; 
}
"true"					{ 
	atLineStart = false;
	yylloc->columns(yyleng);
	return parser::token::T_TRUE; 
}
"begin"					{ 
	// do not apply offside rule after begin
	// instead, pop the last guide token if any
	if (cur_indent_level > 1) {
		popGuide();
	}

	atLineStart = false;
	yylloc->columns(yyleng);
	return parser::token::T_BEGIN; 
}
"var"					{
	EMIT_AUTOEND_IF_NEEDED();
	atLineStart = false;
	yylloc->columns(yyleng);
	return parser::token::T_VAR; 
}
"break"					{
	EMIT_AUTOEND_IF_NEEDED();
	atLineStart = false;
	yylloc->columns(yyleng);
	return parser::token::T_BREAK; 
}
"end"					{ 
	/* Explicit end of an explicit block: no off-side handling. */
	atLineStart = false;
	yylloc->columns(yyleng);
	return parser::token::T_END; 
}
"not"					{ 
	atLineStart = false;
	yylloc->columns(yyleng);
	return parser::token::T_NOT; 
}
"byte"					{
	EMIT_AUTOEND_IF_NEEDED();
	atLineStart = false;
	yylloc->columns(yyleng);
	return parser::token::T_BYTE; 
}
"exit"					{
	EMIT_AUTOEND_IF_NEEDED();
	atLineStart = false;
	yylloc->columns(yyleng);
	return parser::token::T_EXIT; 
}
"or"					{ 
	atLineStart = false;
	yylloc->columns(yyleng);
	return parser::token::T_OR; 
}
"continue"				{
	EMIT_AUTOEND_IF_NEEDED();
	atLineStart = false;
	yylloc->columns(yyleng);
	return parser::token::T_CONT; 
}

"false"					{ 
	atLineStart = false;
	yylloc->columns(yyleng);
	return parser::token::T_FALSE; 
}
"ref"					{ 

	atLineStart = false;
	yylloc->columns(yyleng);
	return parser::token::T_REF; 
}
"decl"					{
	EMIT_AUTOEND_IF_NEEDED();
	atLineStart = false;
	yylloc->columns(yyleng);
	return parser::token::T_DECL; 
}
"return"				{
	EMIT_AUTOEND_IF_NEEDED();
	atLineStart = false;
	yylloc->columns(yyleng);
	return parser::token::T_RET; 
}

"(*" 			 { commentDepth++;
				   BEGIN(COMMENT); }
<COMMENT>"*)" 	 { commentDepth--;
				   if(commentDepth == 0) {
				    yylloc->columns(yyleng);
					atLineStart = false;
				    BEGIN(INITIAL); }
				 }
<COMMENT>\n 	 { handleNewLine(yylloc); }
<COMMENT>"*" 	 { yylloc->columns(1); }			//Track column if only '*' encountered
<COMMENT>[^*\n]+ { yylloc->columns(yyleng); }		//Track column if code follows
<COMMENT><<EOF>> {
    if (auto* diag = dana::getGlobalDiagnostics()) {
        diag->report(Diagnostics::Severity::Error,
            static_cast<int>(yylloc->begin.line), 0,
            "EOF reached inside a comment");
    } else {
        fprintf(stderr, "Error: EOF reached inside a comment at line %d\n",
                static_cast<int>(yylloc->begin.line));
    }
    exit(1);
}


{DIGIT}+				{ 
	/* Integer constant */

	atLineStart = false;
	yylloc->columns(yyleng);
	yylval->emplace<int>(std::stoi(yytext));
	return parser::token::T_INT_CONST;
}

'([^\'\\]|\\[nrt0'"\\]|\\x[0-9A-Fa-f]{2})'		{
	/* Character constant with escape handling */

	atLineStart = false;
	yylloc->columns(yyleng);
	char c;
	if (yytext[1] == '\\')
		c = processEscape(yytext, 1, yylloc);
	else
		c = yytext[1];

	yylval->emplace<char>(c);
	return parser::token::T_CHAR_CONST;
}

{LETTER}{ID_CHAR}*		{ 
	/* Identifier */
	EMIT_AUTOEND_IF_NEEDED();
	atLineStart = false;
	yylloc->columns(yyleng);
	yylval->emplace<std::string>(yytext);
	return parser::token::T_ID;
}


\"([^\"\n\r\\]|\\.)*\"		{
	/* String constant with escape handling */
	EMIT_AUTOEND_IF_NEEDED();
	atLineStart = false;
	yylloc->columns(yyleng);
	std::string s = processStringEscapes(yytext, yylloc);
	
	yylval->emplace<std::string>(std::move(s));
	return parser::token::T_STRING_CONST;
}


":="                    { 
	/* Assignment operator */

	atLineStart = false;
	yylloc->columns(yyleng);
	return parser::token::T_ASSIGN; 
}
"<>"                    { 
	/* Not equal operator */

	atLineStart = false;
	yylloc->columns(yyleng);
	return parser::token::T_NE; 
}
"<="                    { 
	/* Less than or equal operator */

	atLineStart = false;
	yylloc->columns(yyleng);
	return parser::token::T_LE; 
}
">="                    { 
	/* Greater than or equal operator */

	atLineStart = false;
	yylloc->columns(yyleng);
	return parser::token::T_GE; 
}


[\=\<\>\+\-\*\/\%\(\)\[\]\,\:]    { 
	/* Single-character operators */

	atLineStart = false;
	yylloc->columns(yyleng);
	return yytext[0]; 
}


"!"						{ 
	/* Bitwise NOT operator */

	atLineStart = false;
	yylloc->columns(yyleng);
	return '!'; 
}
"|"						{ 
	/* Bitwise OR operator */

	atLineStart = false;
	yylloc->columns(yyleng);
	return '|'; 
}
"&" 					{ 
	/* Bitwise AND operator */

	atLineStart = false;
	yylloc->columns(yyleng);
	return '&'; 
}

[ \t]+					{ yylloc->columns(yyleng); /* ignore spaces but track column */ }
\r						{ /* ignore carriage return */ }
\n+						{ handleNewLine(yylloc); }

#.*\n					{ handleNewLine(yylloc); }

.						{ 
	/* Error handling for illegal characters */
	if (auto* diag = dana::getGlobalDiagnostics()) {
		diag->report(Diagnostics::Severity::Error,
			static_cast<int>(yylloc->begin.line),
			static_cast<int>(yylloc->begin.column),
			"Illegal character '%c'", yytext[0]);
	} else {
		fprintf(stderr, "Error: Illegal character '%c' at line %d, column %d\n",
				yytext[0],
				static_cast<int>(yylloc->begin.line),
				static_cast<int>(yylloc->begin.column));
	}
	yylloc->columns(1);
}

%%

/* Implementation of lexer helper functions */

// Guide token encountered (push)
static inline void pushGuide (const char* guide_name, int guide_col, int guide_line) {
	(void)guide_name;
	if (cur_indent_level >= MAX_INDENT_LEVELS) {
		if (auto* diag = dana::getGlobalDiagnostics()) {
			diag->report(Diagnostics::Severity::Error, guide_line, 0, "Too many nested levels");
		} else {
			fprintf(stderr, "Error: Too many nested levels at line %d\n", guide_line);
		}
		exit(1);
	}

	indentStack[cur_indent_level++] = guide_col;
	//printf("Guide pushed: %s at column %d\n", guide_name, guide_col);
}

// Go up one indentation level. 2 possibilities for usage of this:
// a. Next token is first in a new line and it is located in a column lefter than the column of the guide word
// b. EOF encountered
static inline void popGuide () {
	if (cur_indent_level > 1) {
		indentStack[--cur_indent_level] = -1;
	}
}

// Check indentation status at next line start
static inline bool checkIndentation (int cur_col) {
	if (!atLineStart) return false;

	// condition: there are indentation layers and the current column we are scanning is not to the right of the guide token
	while (cur_indent_level > 1 && cur_col <= indentStack[cur_indent_level - 1]) {
		// then we pop guide (we move up one indentation layer)
		popGuide();
		// and mark that we need to emit an <auto-end> token
		pendingAutoEnds++;
	}

	if (pendingAutoEnds > 0) {
		BEGIN(PENDING);
		return true;
	}

	return false;
}

// Handle newline - reset column and set line start flag - also check if auto-end must be emitted - if yes emit
static inline void handleNewLine(parser::location_type* yylloc) {
	atLineStart = true;
	yylloc->lines(yyleng);
	yylloc->step();
}

// Escape processing functions
static inline char processEscape(const char* text, int pos, const parser::location_type* yylloc) {
	const int line = yylloc ? static_cast<int>(yylloc->begin.line) : 0;
	char escape_char = text[pos + 1];
	size_t text_len = std::strlen(text);
	
	switch (escape_char) {
		case 'n':  return '\n';
		case 't':  return '\t';
		case 'r':  return '\r';
		case '0':  return '\0';
		case '\\': return '\\';
		case '\'': return '\'';
		case '"':  return '"';
		case 'x':  // Hex escape \xnn
				if ((size_t)pos + 3 < text_len) {
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
			if (auto* diag = dana::getGlobalDiagnostics()) {
				diag->report(Diagnostics::Severity::Error, line, 0, "Invalid hex escape sequence");
			} else {
				fprintf(stderr, "Error: Invalid hex escape at line %d\n", line);
			}
			return 0;
		default:
			if (auto* diag = dana::getGlobalDiagnostics()) {
				diag->report(Diagnostics::Severity::Warning, line, 0, "Unknown escape sequence \\%c", escape_char);
			} else {
				fprintf(stderr, "Warning: Unknown escape sequence \\%c at line %d\n", escape_char, line);
			}
			return escape_char;
	}
}

static inline std::string processStringEscapes(const char* input, const parser::location_type* yylloc) {
	int len = std::strlen(input);
	std::string result;
	result.reserve(len); // avoids reallocations

	int i = 1; // Skip opening quote
	while (i < len - 1) { // Until before closing quote
		if (input[i] == '\\') {
			// Handle escape sequence
			char c = processEscape(input, i, yylloc);
			result.push_back(c);

			// Skip over the escape sequence in the source text
			if (input[i + 1] == 'x')
				i += 4;   // \xNN
			else
				i += 2;   // \n, \t, etc.
		} else {
			result.push_back(input[i]);
			i++;
		}
	}

	return result;
}
