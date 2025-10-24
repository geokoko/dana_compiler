%{
	#include <cstdio>
	#include <cstdlib>
	#include <cstring>
	#include <cctype>
	#include <string>

	#include "../parser/parser.tab.hh"
	using dana::parser;

	#define EMIT_AUTOEND_IF_NEEDED() \
    do { \
        if (atLineStart && checkIndentation(columno)) { \
            yyless(0); \
            return parser::token::T_AUTO_END; \
        } \
    } while(0)

	#define YY_DECL int yylex(parser::semantic_type* yylval, parser::location_type* yylloc)
	#define YY_USER_ACTION yylloc->columns(yyleng);

	static int lineno = 1;
	static int columno = 1;
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
	static inline void pushGuide(const char* guide_name, int guide_col);
	static inline void popGuide();
	static inline bool checkIndentation(int curr_col);
	static inline void handleNewLine(parser::location_type& yylloc);
	static inline char processEscape(const char* text, int pos);
	static inline std::string processStringEscapes(const char* input);
%}

%option noyywrap nodefault

DIGIT [0-9]
LETTER [a-zA-Z]
ID_CHAR [a-zA-Z0-9_]

%s PENDING
%s COMMENT

%%

<PENDING>(.|\n) 		{
  	printf("I AM IN PENDING STATE\n");
	yyless(0); /* put char back; DO NOT CONSUME */
	if (--pendingAutoEnds == 0) {
		BEGIN(INITIAL); /* leave PENDING state */
		return yylex(yylval, yylloc); /* rescan from the same position in INITIAL, */
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
	pushGuide("def", columno);
	atLineStart = false;
	columno += yyleng;
	return parser::token::T_DEF; 
}
"if"					{ 
	/* Guide token */
	EMIT_AUTOEND_IF_NEEDED();
	pushGuide("if", columno); 
	atLineStart = false;
	columno += yyleng;
	return parser::token::T_IF; 
}
"elif"					{ 
	/* Guide token */
	EMIT_AUTOEND_IF_NEEDED();
	pushGuide("elif", columno); 
	atLineStart = false;
	columno += yyleng;
	return parser::token::T_ELIF; 
}
"else"					{ 
	/* Guide token */
	EMIT_AUTOEND_IF_NEEDED();
	pushGuide("else", columno); 
	atLineStart = false;
	columno += yyleng;
	return parser::token::T_ELSE; 
}

"loop"					{ 
	/* Guide token */
	EMIT_AUTOEND_IF_NEEDED();
	pushGuide("loop", columno); 
	atLineStart = false;
	columno += yyleng;
	return parser::token::T_LOOP; 
}

"and"					{ 
	/* Keyword */
	EMIT_AUTOEND_IF_NEEDED();
	atLineStart = false;
	columno += yyleng;
	return parser::token::T_AND; 
}
"is"					{
	EMIT_AUTOEND_IF_NEEDED();
	atLineStart = false;
	columno += yyleng;
	return parser::token::T_IS; 
}
"skip"					{ 
	EMIT_AUTOEND_IF_NEEDED();
	atLineStart = false;
	columno += yyleng;
	return parser::token::T_SKIP; 
}
"as"					{
	EMIT_AUTOEND_IF_NEEDED();
	atLineStart = false;
	columno += yyleng;
	return parser::token::T_AS; 
}
"int"					{
	EMIT_AUTOEND_IF_NEEDED();
	atLineStart = false;
	columno += yyleng;
	return parser::token::T_INT; 
}
"true"					{
	EMIT_AUTOEND_IF_NEEDED();
	atLineStart = false;
	columno += yyleng;
	return parser::token::T_TRUE; 
}
"begin"					{
	EMIT_AUTOEND_IF_NEEDED();
	atLineStart = false;
	columno += yyleng;
	return parser::token::T_BEGIN; 
}
"var"					{
	EMIT_AUTOEND_IF_NEEDED();
	atLineStart = false;
	columno += yyleng;
	return parser::token::T_VAR; 
}
"break"					{
	EMIT_AUTOEND_IF_NEEDED();
	atLineStart = false;
	columno += yyleng;
	return parser::token::T_BREAK; 
}
"end"					{
	EMIT_AUTOEND_IF_NEEDED();
	atLineStart = false;
	columno += yyleng;
	return parser::token::T_END; 
}
"not"					{
	EMIT_AUTOEND_IF_NEEDED();
	atLineStart = false;
	columno += yyleng;
	return parser::token::T_NOT; 
}
"byte"					{
	EMIT_AUTOEND_IF_NEEDED();
	atLineStart = false;
	columno += yyleng;
	return parser::token::T_BYTE; 
}
"exit"					{
	EMIT_AUTOEND_IF_NEEDED();
	atLineStart = false;
	columno += yyleng;
	return parser::token::T_EXIT; 
}
"or"					{
	EMIT_AUTOEND_IF_NEEDED();
	atLineStart = false;
	columno += yyleng;
	return parser::token::T_OR; 
}
"continue"				{
	EMIT_AUTOEND_IF_NEEDED();
	atLineStart = false;
	columno += yyleng;
	return parser::token::T_CONT; 
}
"false"					{
	EMIT_AUTOEND_IF_NEEDED();
	atLineStart = false;
	columno += yyleng;
	return parser::token::T_FALSE; 
}
"ref"					{ 
	EMIT_AUTOEND_IF_NEEDED();
	atLineStart = false;
	columno += yyleng;
	return parser::token::T_REF; 
}
"decl"					{
	EMIT_AUTOEND_IF_NEEDED();
	atLineStart = false;
	columno += yyleng;
	return parser::token::T_DECL; 
}
"return"				{
	EMIT_AUTOEND_IF_NEEDED();
	atLineStart = false;
	columno += yyleng;
	return parser::token::T_RET; 
}

"(*" 			 { commentDepth++;
				   BEGIN(COMMENT); }
<COMMENT>"*)" 	 { commentDepth--;
				   if(commentDepth == 0) {
					columno += yyleng;		//For the case that there is code after the comment in the same line	
				    atLineStart = false;
				    BEGIN(INITIAL); }
				 }
<COMMENT>\n 	 { handleNewLine(*yylloc); }
<COMMENT>"*" 	 { columno += 1; }
<COMMENT>[^*\n]+ { columno += yyleng; }		//Track column if code follows
<COMMENT><<EOF>> {
    fprintf(stderr, "Error: EOF reached inside a comment at line %d\n", lineno);
    exit(1);
}


{DIGIT}+				{ 
	/* Integer constant */
	EMIT_AUTOEND_IF_NEEDED();
	atLineStart = false;
	columno += yyleng;
	yylval->emplace<int>(std::stoi(yytext));
	return parser::token::T_INT_CONST;
}

'([^\'\\]|\\[nrt0'"\\]|\\x[0-9A-Fa-f]{2})'		{
	/* Character constant with escape handling */
	EMIT_AUTOEND_IF_NEEDED();
	atLineStart = false;
	columno += yyleng;
	char c;
	if (yytext[1] == '\\')
		c = processEscape(yytext, 1);
	else
		c = yytext[1];

	yylval->emplace<char>(c);
	return parser::token::T_CHAR_CONST;
}

{LETTER}{ID_CHAR}*		{ 
	/* Identifier */
	EMIT_AUTOEND_IF_NEEDED();
	atLineStart = false;
	columno += yyleng;
	yylval->emplace<std::string>(yytext);
	return parser::token::T_ID;
}


\"([^\"\n\r\\]|\\.)*\"		{
	/* String constant with escape handling */
	EMIT_AUTOEND_IF_NEEDED();
	atLineStart = false;
	columno += yyleng;
	std::string s = processStringEscapes(yytext);
	
	yylval->emplace<std::string>(std::move(s));
	return parser::token::T_STRING_CONST;
}


":="                    { 
	/* Assignment operator */
	EMIT_AUTOEND_IF_NEEDED();
	atLineStart = false;
	columno += yyleng;
	return parser::token::T_ASSIGN; 
}
"<>"                    { 
	/* Not equal operator */
	EMIT_AUTOEND_IF_NEEDED();
	atLineStart = false;
	columno += yyleng;
	return parser::token::T_NE; 
}
"<="                    { 
	/* Less than or equal operator */
	EMIT_AUTOEND_IF_NEEDED();
	atLineStart = false;
	columno += yyleng;
	return parser::token::T_LE; 
}
">="                    { 
	/* Greater than or equal operator */
	EMIT_AUTOEND_IF_NEEDED();
	atLineStart = false;
	columno += yyleng;
	return parser::token::T_GE; 
}


[\=\<\>\+\-\*\/\%\(\)\[\]\,\:]    { 
	/* Single-character operators */
	EMIT_AUTOEND_IF_NEEDED();
	atLineStart = false;
	columno += yyleng;
	return yytext[0]; 
}


"!"						{ 
	/* Bitwise NOT operator */
	EMIT_AUTOEND_IF_NEEDED();
	atLineStart = false;
	columno += yyleng;
	return '!'; 
}
"|"						{ 
	/* Bitwise OR operator */
	EMIT_AUTOEND_IF_NEEDED();
	atLineStart = false;
	columno += yyleng;
	return '|'; 
}
"&" 					{ 
	/* Bitwise AND operator */
	EMIT_AUTOEND_IF_NEEDED();
	atLineStart = false;
	columno += yyleng;
	return '&'; 
}

[ ]+					{ columno += yyleng; /* ignore spaces but track column */ }
\r						{ /* ignore carriage return */ }
\t						{ columno += 8; /* assuming tab width of 8 */ }
\n+						{ handleNewLine(*yylloc); }

#.*\n					{ handleNewLine(*yylloc); }

.						{ 
	/* Error handling for illegal characters */
	fprintf(stderr, "Error: Illegal character '%c' at line %d, column %d\n", yytext[0], lineno, columno);
	columno++; 
}

%%

/* Implementation of lexer helper functions */

// Guide token encountered (push)
static inline void pushGuide (const char* guide_name, int guide_col) {
	if (cur_indent_level >= MAX_INDENT_LEVELS) {
		fprintf(stderr, "Error: Too many nested levels at line %d\n", lineno);
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

// Check indentation status at line start
static inline bool checkIndentation (int curr_col) {
	if (!atLineStart) return false;

	// condition: there are indentation layers and the current column we are scanning is not to the right of the guide token
	while (cur_indent_level > 1 && curr_col <= indentStack[cur_indent_level - 1]) {
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

// Handle newline - reset column and set line start flag
static inline void handleNewLine(parser::location_type& yylloc) {
	lineno++;
	columno = 1;
	atLineStart = true;
	yylloc.lines(yyleng);
	yylloc.step();
}

// Escape processing functions
static inline char processEscape(const char* text, int pos) {
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

static inline std::string processStringEscapes(const char* input) {
	int len = std::strlen(input);
	std::string result;
	result.reserve(len); // avoids reallocations

	int i = 1; // Skip opening quote
	while (i < len - 1) { // Until before closing quote
		if (input[i] == '\\') {
			// Handle escape sequence
			char c = processEscape(input, i);
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
