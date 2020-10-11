%{
    #include "globals.h"
    #include "util.h"
    #include "scan.h"

    /* lexema de identificador ou palavra reservada */
    char tokenString[MAXTOKENLEN+1];
    static int yylex(void);
%}

digit       [0-9]
number      {digit}+
letter      [a-zA-Z]
identifier  {letter}+
newline     \n
whitespace  [ \t]+

%option noyywrap
%%

"/*"            {
                    char c;
                    int a = 2;
                    while(a) {
                        c = input();
                        if(c == EOF) {
                            return ERRO; 
                        }
                        else if(c == '\n') lineno++;
                        else {
                            if(c == '*') a = 1;
                            else if(a == 1 && c == '/') a = 0;
                            else a = 2;
                        }
                    }
                }

"else"          { return ELSE; }
"if"            { return IF; }
"int"           { return INT; }
"return"        { return RETURN; }
"void"          { return VOID; }
"while"         { return WHILE; }
"+"             { return MAIS; }
"-"             { return MENOS; }
"*"             { return VEZES; }
"/"             { return DIVIDIDO; }
"<"             { return MENORQUE; }
"<="            { return MENORIGUALQUE; }
">"             { return MAIORQUE; }
">="            { return MAIORIGUALQUE; }
"=="            { return IGUAL; }
"!="            { return DIFERENTEDE; }
"="             { return RECEBE; }
";"             { return PONTOVIRGULA; }
","             { return VIRGULA; }
"("             { return EPARENTESES; }
")"             { return DPARENTESES; }
"["             { return ECOLCHETE; }
"]"             { return DCOLCHETE; }
"{"             { return ECHAVE; }
"}"             { return DCHAVE; }
{number}     	{ return NUMERO; }
{identifier}    { return ID; }
{newline}       { lineno++; }
{whitespace}    { }
.               { return ERRO; }

%%

TokenType getToken(void) {
    static int firstTime = TRUE;
    TokenType currentToken;
    if (firstTime) {
        firstTime = FALSE;
        lineno++;
        yyin = source;
        yyout = listing;
    }
    currentToken = yylex();
    strncpy(tokenString,yytext,MAXTOKENLEN);
    if (TraceScan) {
        fprintf(listing,"\t%d: ",lineno);
        printToken(currentToken,tokenString);
    }
    return currentToken;
}
