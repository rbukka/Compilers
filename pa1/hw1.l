%option noyywrap
%{
#include <stdio.h>
%}

%%
"/*"([^*]|[\r\n]|(\*+([^*/]|[\r\n])))*[*]+"/" 
[0123456789]+ printf("%s\n",yytext);
[a-zA-Z][a-zA-Z_0-9]* printf("%s\n",yytext);
"&&"|"||"|"=="|"!="|"<="|">="|"<"|">"|"+"|"-"|"*"|"/" printf("%s\n",yytext);
[ \t\n] 
"=" 
";" 
. 
%%
int main(int argc,char **argv)
{

	++argv;--argc;
	if(argc>0)
		yyin = fopen(argv[0],"r");
	else
		yyin = stdin;
	yylex(); /* start the analysis*/
}
/* use command "flex <filename>" then gcc lex.yy.c */
