%{
    #define T_eof 0

    #define T_assign 650
    #define T_colon 651

    #define T_and 770
    #define T_def 771
    #define T_is 772
    #define T_skip 773
    #define T_as 774
    #define T_elif 775
    #define T_int 776
    #define T_true 777
    #define T_begin 778
    #define T_else 779
    #define T_loop 780
    #define T_var 781
    #define T_break 782
    #define T_end 783
    #define T_not 784
    #define T_byte 785
    #define T_exit 786
    #define T_or 787
    #define T_continue 788
    #define T_false 789
    #define T_ref 790
    #define T_decl 791
    #define T_if 792
    #define T_return 793

    #define T_integer 800
    #define T_id 801
    #define T_string 802

    #define T_notEqual 903
    #define T_bigEqual 904
    #define T_smallEqual 905

    void ERROR (const char msg[]);
    int lineno = 1;
%}


D [0-9]
L [A-Za-z]
W [ \t\r]


%x COMMENT


%%

"and" { return T_and; }
"def" { return T_def; }
"is"  { return T_is; }
"skip" { return T_skip; }
"as" { return T_as; }
"elif" { return T_elif; }
"int" { return T_int; }
"true" { return T_true; }
"begin" { return T_begin; }
"else" { return T_else; }
"loop" { return T_loop; }
"var" { return T_var; }
"break" { return T_break; }
"end" { return T_end; }
"not" { return T_not; }
"byte" { return T_byte; }
"exit" { return T_exit; }
"or" { return T_or; }
"continue" { return T_continue; }
"false" { return T_false; }
"ref" { return T_ref; }
"if" { return T_if; }
"return" { return T_return; }

[\=\<\>\+\-\*\/\%\(\)\[\]\,] { return yytext[0]; } //Return ASCII character
"!" { return T_not; }
"|" { return T_or; }
"&" { return T_and; } 
"<>" { return T_notEqual; }
">=" { return T_bigEqual; }
"<=" { return T_smallEqual; }

":=" { return T_assign; }
":" { return T_colon; }

\".*\" { return T_string; }
\-*{D}+ {return T_integer; }
{L}({L}|{D}|_)* {return T_id;}
{W}+ { /* nothing */ }
\n { lineno++; }
\#.*\n { lineno++; }

"(*" { BEGIN(COMMENT); }
<COMMENT>"*)" { BEGIN(INITIAL); }
<COMMENT>\n { lineno++; }
<COMMENT>"*" { /* nothing */ }
<COMMENT>[^*\n]+ { /* nothing */ }


.       { ERROR("Illegal token"); }

%%

void ERROR(const char msg[]){
    fprintf(stderr, "ERROR, line %d: %s\n", lineno, msg);
    exit(1);
}


int main() { 
    int token;
    do{
        token = yylex();
        printf("token=%d, lexeme=\"%s\" @line: %d\n", token, yytext, lineno);
    } while (token != T_eof);

    printf("Line count: %d\n", lineno);

    return 0;
}
