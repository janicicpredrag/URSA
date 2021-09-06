%{
#include <stdlib.h>
#include "ursa.h"
#include "ursa.tab.hpp"
void yyerror(char *);
%}

%%

"++"            return PLUSPLUS; 
"--"            return MINUSMINUS; 
"<<="           return LSHIFTEQ; 
">>="           return RSHIFTEQ; 
"+="            return PLUSEQ; 
"-="            return MINUSEQ; 
"*="            return MULTEQ; 
"/="            return DIVEQ; 
"&="            return BITWISEANDEQ;  
"|="            return BITWISEOREQ; 
"^="            return BITWISEXOREQ; 
"&&"            return LOGICALAND;  
"||"            return LOGICALOR; 
"^^"            return LOGICALXOR; 
"&&="           return ANDEQ;  
"||="           return OREQ; 
"^^="           return XOREQ; 
"<<"            return LSHIFT;
">>"            return RSHIFT;
">="            return GE;
"<="            return LE;
"=="            return EQ;
"!="            return NE;
"num2bool"      return NUM2BOOL;
"bool2num"      return BOOL2NUM;
"sgn"           return SGN;
"ite"           return ITE;
"while"         return WHILE;
"for"           return FOR;
"if"            return IF;
"else"          return ELSE;
"print"         return PRINT;
"printb"        return PRINTB;
"printx"        return PRINTX;
"listvars"      return LIST;
"clear"         return CLEAR;
"halt"          return HALT;
"minimize"      return MINIMIZE;
"maximize"      return MAXIMIZE;
"assert"        return ASSERT;
"assert_all"    return ASSERTA;
"procedure"     return PROCEDURE;
"call"          return CALL;


n[A-Za-z0-9_]* { 
                 yylval.sName = strdup(yytext);
                 return INTEGER_VARIABLE;
               }

b[A-Za-z0-9_]* { 
                 yylval.sName = strdup(yytext);
                 return BOOLEAN_VARIABLE;
               }

[0-9]+         {
                 yylval.sValue = strdup(yytext);
                 return INTEGER;
               }

0b[0-1]+       {
                 yylval.sValue = strdup(yytext);
                 return INTEGER;
               }

0x[A-Fa-f0-9]+ {
                 yylval.sValue = strdup(yytext);
                 return INTEGER;
               }

"true"         {
                 yylval.bValue = 1;
                 return BOOLEAN;
               }

"false"        {
                 yylval.bValue = 0;
                 return BOOLEAN;
               }

[-()<>=+*/;{}.,|&^~\[\]!] {
                return *yytext;
             }



"/*"([^*]|\*[^/])*"*/"  { /* printf("comment: %s",yytext); */ }  /* ignore comments */

[A-Za-z][A-Za-z0-9_]* { 
                 yylval.sName = strdup(yytext);
                 return PROCEDURE_ID;
               }


[ \t\n]+        ;       /* ignore whitespace */

.               yyerror((char*)"Unknown character");
%%
int yywrap(void) {
    return 1;
}
