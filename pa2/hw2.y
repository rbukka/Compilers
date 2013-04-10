%{
    #include <stdio.h>
    #include <stdlib.h>
    #define COUNT 20
    #define DEBUG 0
    int yylex(void);
    void yyerror(char *);
    int a[COUNT];    
    int count = 0;
%}

%token INT GEQ LEQ GT LT EQ NEQ ADD SUB MULT DIV OR AND LEFTP RIGHTP LINE NOT

%left	   OR
%left	   AND
%left      EQ NEQ
%left      ADD SUB
%left      MULT DIV
%right 	   NOT
%%

program:
        program bexpr '\n'         { if($2==1) printf("%s\n","YES"); else
	  printf("%s\n","NO"); }
	|
	;

bexpr:
	bexpr OR bterm		  { $$ = ($1 || $3);}
	| bterm			  { $$ = $1;}

bterm:
	bterm AND bfactor	 { $$ = ($1 && $3);if (DEBUG) printf("\nValues are%d%d\n",$1,$3);}
	| bfactor		 { $$ = $1;}

bfactor:
	NOT bfactor 		{ $$ = ($2==0)?1:0;}
	| LEFTP bexpr RIGHTP 	{ $$ = $2;}
	|  expr relop expr	{
				 if($2==LT)
				 {
				 	$$ = ($1<$3)?1:0;
					if(DEBUG)
						printf("LT %d\n",$$);
				 }
				 	 if($2==GT) 
					 {
						$$ = ($1>$3)?1:0;
						if(DEBUG)
							printf("GT %d\n",$$);
					 }
					 if($2==EQ) $$ =
					 ($1==$3)?1:0; if($2==GEQ)
					 $$=($1>=$3)?1:0; if($2==LEQ)
					 $$=($1<=$3)?1:0; if($2==NEQ)
					 $$=($1!=$3)?1:0;
				}

relop:
	LT {$$ = LT;} | GT {$$=GT;} | EQ {$$=EQ;} | GEQ {$$=GEQ;} | LEQ
	{$$=LEQ;} | NEQ {$$=NEQ;} 
	

expr:
        INT    {$$ = $1;}
	| LEFTP expr RIGHTP {$$=$2;}
        | expr ADD expr           { $$ = $1 + $3; }
        | expr SUB expr           { $$ = $1 - $3;if(DEBUG) printf("Sub %d\n",$$);}
	| expr MULT expr		  { $$ = $1 * $3; }
	| expr DIV expr		  { $$ = $1 / $3; }
	| LINE INT		  { if($2<=count) {$$=a[$2-1];if(DEBUG)
	printf("Line %d\n",$$);}	else{printf("%s\n","Line number exceeds	length of database file");}}	
        ;

%%

void yyerror(char *s) {
	fprintf(stderr, "%s\n", s);
}

int main(int argc,char** argv) {
   
    ++argv;--argc;
    if(argc>0)
    {	

	    FILE *fp = fopen(argv[0],"r");
    
	    char line[128];
	    size_t len;

	    if(fp!=NULL)
    		{
			while(fgets(line,sizeof(line),fp) != NULL) {
				a[count] = atoi(line);
				count++;
			}
    		}
    yyparse();
    }
    else
    {
	    printf("%s\n","Please provide database file");
    }
    return 0;
}
