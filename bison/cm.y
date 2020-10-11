%{
    #define YYPARSER    /* Distingue arquivo de saida dos outros arquivos */

    #include "globals.h"
    #include "util.h"
    #include "scan.h"
    #include "parse.h"

    #define YYSTYPE TreeNode *
    static TreeNode * arvore;    /* guarda arvore sintativa para retorno */
    static int yylex(void);
	int yyerror(char *msg);

%}

%token ELSE IF INT RETURN VOID WHILE
%token MAIS MENOS VEZES DIVIDIDO MENORQUE MENORIGUALQUE MAIORQUE MAIORIGUALQUE IGUAL DIFERENTEDE RECEBE PONTOVIRGULA VIRGULA EPARENTESES DPARENTESES ECOLCHETE DCOLCHETE ECHAVE DCHAVE
%token NUMERO ID
%token ERRO ENDFILE

%% /* Gramatica para C Menos */

programa            :   declaracao_lista
                            { 
                                arvore = $1; 
                            }
                    ;

declaracao_lista    :   declaracao_lista declaracao
                            { 
                                YYSTYPE t = $1;
                                if(t != NULL) {     
                                    while(t->sibling != NULL)
                                        t = t->sibling;
                                    t->sibling = $2;
                                    $$ = $1;
                                }
                                else
                                    $$ = $2;
                            }
                    |   declaracao
                            { 
                                $$ = $1;
                            }
                    ;

declaracao          :   var_declaracao
                            { 
                                $$ = $1;
                            }
                    |   fun_declaracao
                            { 
                                $$ = $1;
                            }
                    ;

var_declaracao      :   INT ident PONTOVIRGULA
                            {
								$$ = newExpNode(TypeK);
                                $$->type = Integer;
								$$->attr.name = "integer";
                                $$->child[0] = $2;
                                $2->nodekind = StmtK;
                                $2->kind.stmt = VarK;
                                $2->type = Integer;
                            }
                    |   INT ident ECOLCHETE num DCOLCHETE PONTOVIRGULA
                            {     
                                $$ = newExpNode(TypeK);
                                $$->type = Integer;
								$$->attr.name = "integer";
                                $$->child[0] = $2;
                                $2->nodekind = StmtK;
                                $2->kind.stmt = VecK;
                                $2->attr.val = $4->attr.val;
                                $2->type = Integer;
                            }
                    ;

fun_declaracao      :   INT ident EPARENTESES params DPARENTESES composto_decl
                            {   
                                $$ = newExpNode(TypeK);
                                $$->type = Integer;
                               	$$->attr.name = "integer";
                                $$->child[0] = $2;
                                $2->nodekind = StmtK;
                                $2->kind.stmt = FuncK;
                                $2->child[0] = $4;
                                $2->child[1] = $6;
    			                $2->type= Integer;
    				            aggScope($2->child[0], $2->attr.name);
    				            aggScope($2->child[1], $2->attr.name);
                            }
		                | VOID ident EPARENTESES params DPARENTESES composto_decl
                            {   
                                $$ = newExpNode(TypeK);
                                $$->type = Void;
                               	$$->attr.name = "VOID";
                                $$->child[0] = $2;
                                $2->nodekind = StmtK;
                                $2->kind.stmt = FuncK;
                                $2->child[0] = $4;
                                $2->child[1] = $6;
    				            $2->type= Void;
    				            aggScope($2->child[0], $2->attr.name);
    				            aggScope($2->child[1], $2->attr.name);
                            }
                    ;

params              :   param_lista
                            { 
                                $$ = $1;
                            }
                    |   VOID
                            {              
                            }
                    ;

param_lista         :   param_lista VIRGULA param
                            { 
                                YYSTYPE t = $1;
                                if(t != NULL){     
                                    while(t->sibling != NULL)
                                        t = t->sibling;
                                    t->sibling = $3;
                                    $$ = $1;
                                }
                                else
                                    $$ = $3;
                            }
                    |   param
                            { 
                                $$ = $1;
                            }
                    ;

param               :   INT ident
                            {
                                $$ = newExpNode(TypeK);
                                $$->type = Integer;
                                $$->attr.name = "integer";
                                $$->child[0] = $2;
                                $2->nodekind = StmtK;
                                $2->kind.stmt = VarK;
                                $2->type = Integer;
                            }
                    |  INT ident ECOLCHETE DCOLCHETE
                            {     
                                $$ = newExpNode(TypeK);
                                $$->type = Integer;
                                $$->attr.name = "integer";
                                $$->child[0] = $2;
                                $2->nodekind = StmtK;
                                $2->kind.stmt = VecK;
                                $2->type = Integer;
                            }
                    ;

composto_decl       :   ECHAVE local_declaracoes statement_lista DCHAVE
                            { 
                                YYSTYPE t = $2;
                                if(t != NULL){     
                                    while(t->sibling != NULL)
                                        t = t->sibling;
                                    t->sibling = $3;
                                    $$ = $2;
                                }
                                else
                                    $$ = $3;
                            }
                    |   ECHAVE local_declaracoes DCHAVE
                            { 
                                $$ = $2;
                            }
                    |   ECHAVE statement_lista DCHAVE
                            { 
                                $$ = $2;
                            }
                    |   ECHAVE DCHAVE
                            {
                            }
                    ;

local_declaracoes   :   local_declaracoes var_declaracao
                            { 
                                YYSTYPE t = $1;
                                if(t != NULL){     
                                    while(t->sibling != NULL)
                                        t = t->sibling;
                                    t->sibling = $2;
                                    $$ = $1;
                                }
                                else
                                    $$ = $2;
                            }
                    |   var_declaracao
                            {
                                $$ = $1;
                            }
                    ;

statement_lista     :   statement_lista statement
                            { 
                                YYSTYPE t = $1;
                                if(t != NULL) {     
                                    while(t->sibling != NULL)
                                        t = t->sibling;
                                    t->sibling = $2;
                                    $$ = $1;
                                }
                                else
                                    $$ = $2;
                            }
                    |   statement
                            {
                                $$ = $1;
                            }
                    ;

statement           :   expressao_decl
                            {
                                $$ = $1;
                            }
                    |   composto_decl
                            {
                                $$ = $1;
                            }
                    |   selecao_decl
                            {
                                $$ = $1;
                            }
                    |   iteracao_decl
                            {
                                $$ = $1;
                            }
                    |   retorno_decl
                            {
                                $$ = $1;
                            }
                    ;

expressao_decl      :   expressao PONTOVIRGULA
                            {
                                $$ = $1;
                            }
                    |   PONTOVIRGULA
                            {
                            }
                    ;

selecao_decl        :   IF EPARENTESES expressao DPARENTESES statement 
                            { 
                                $$ = newStmtNode(IfK);
                                $$->child[0] = $3;
                                $$->child[1] = $5;
                            }
                    |   IF EPARENTESES expressao DPARENTESES statement ELSE statement
                            { 
                                $$ = newStmtNode(IfK);
                                $$->child[0] = $3;
                                $$->child[1] = $5;
                                $$->child[2] = $7;
                            }
                    ;

iteracao_decl       :   WHILE EPARENTESES expressao DPARENTESES statement
                            { 
                                $$ = newStmtNode(WhileK);
                                $$->child[0] = $3;
                                $$->child[1] = $5;
                            }
                    ;

retorno_decl        :   RETURN PONTOVIRGULA
                            { 
                                $$ = newStmtNode(ReturnK);
                            }
                    |   RETURN expressao PONTOVIRGULA
                            { 
                                $$ = newStmtNode(ReturnK);
                                $$->child[0] = $2;
                            }
                    ;

expressao           :   var RECEBE expressao
                            { 
                                $$ = newStmtNode(AssignK);
                                $$->child[0] = $1;
                                $$->child[1] = $3;
                            }
                    |   simples_expressao
                            { 
                                $$ = $1;
                            }
                    ;

var                 :   ident
                            { 
                                $$ = $1;
		                        $$->type=Integer;
                            }
                    |   ident ECOLCHETE expressao DCOLCHETE
                            { 
                                $$ = $1;
                                $$->kind.exp = VecIndexK;
                                $$->child[0] = $3;
				                $$->type=Integer;
                            }
                    ;

simples_expressao   :   soma_expressao relacional soma_expressao
                            { 
                                $$ = $2;
                                $$->child[0] = $1;
                                $$->child[1] = $3;
                            }
                    |   soma_expressao
                            { 
                                $$ = $1;
                            }
                    ;

relacional          :   IGUAL
                            { 
                                $$ = newExpNode(OpK);
                                $$->attr.op = IGUAL;
                    			$$->type=Boolean;
                    			$$->attr.name = "==";                           
                            }
                    |   DIFERENTEDE
                            { 
                                $$ = newExpNode(OpK);
                                $$->attr.op = DIFERENTEDE;  
                				$$->type=Boolean;
                				$$->attr.name = "!=";                           
                            }
                    |   MENORQUE
                            { 
                                $$ = newExpNode(OpK);
                                $$->attr.op = MENORQUE; 
                				$$->type=Boolean;
                				$$->attr.name = "<";                            
                            }
                    |   MENORIGUALQUE
                            { 
                                $$ = newExpNode(OpK);
                                $$->attr.op = MENORIGUALQUE; 
                				$$->type=Boolean; 
                				$$->attr.name = "<=";                           
                            }
                    |   MAIORQUE
                            { 
                                $$ = newExpNode(OpK);
                                $$->attr.op = MAIORQUE;  
                				$$->type=Boolean; 
                				$$->attr.name = ">";                          
                            }
                    |   MAIORIGUALQUE
                            { 
                                $$ = newExpNode(OpK);
                                $$->attr.op = MAIORIGUALQUE;  
                				$$->type=Boolean;
                				$$->attr.name = ">=";                           
                            }
                    ;

soma_expressao      :   soma_expressao soma termo
                            { 
                                $$ = $2;
                                $$->child[0] = $1;
                                $$->child[1] = $3;
                            }
                    |   termo
                            {
                                $$ = $1;
                            }
                    ;

soma                :   MAIS
                            { 
                                $$ = newExpNode(OpK);
                                $$->attr.op = MAIS; 
                				$$->type=Integer;  
                				$$->attr.name = "+";                          
                            }
                    |   MENOS
                            { 
                                $$ = newExpNode(OpK);
                                $$->attr.op = MENOS;   
                				$$->type=Integer; 
                				$$->attr.name = "-";                        
                            }
                    ;

termo               :   termo mult fator
                            { 
                                $$ = $2;
                                $$->child[0] = $1;
                                $$->child[1] = $3;
                            }
                    |   fator
                            { 
                                $$ = $1;
                            }
                    ;

mult                :   VEZES
                            {     
                                $$ = newExpNode(OpK);
                                $$->attr.op = VEZES;
                                $$->attr.name = "*";
                                $$->type = Integer;                         
                            }
                    |   DIVIDIDO
                            {     
                                $$ = newExpNode(OpK);
                                $$->attr.op = DIVIDIDO;
                                $$->attr.name = "/";
                                $$->type = Integer;                          
                            }
                    ;

fator               :   EPARENTESES expressao DPARENTESES
                            { 
                                $$ = $2;
                            }
                    |   var
                            { 
                                $$ = $1;
                            }
                    |   ativacao
                            { 
                                $$ = $1;
                            }
                    |   num
                            { 
                                $$ = $1;
                            }
                    ;

ativacao            :   ident EPARENTESES arg_lista DPARENTESES
                            { 
                                $$ = $1;
                                $$->child[0] = $3;
                                $$->kind.exp = CallK;
                            }
                    |   ident EPARENTESES DPARENTESES
                            { 
                                $$ = $1;
                                $$->kind.exp = CallK;
                            }
                    ;

arg_lista           :   arg_lista VIRGULA expressao
                            { 
                                YYSTYPE t = $1;
                                if(t != NULL){     
                                    while(t->sibling != NULL)
                                        t = t->sibling;
                                    t->sibling = $3;
                                    $$ = $1;
                                }
                                else
                                    $$ = $3;
                            }
                    |   expressao
                            { 
                                $$ = $1;
                            }
                    ;

ident               :   ID
                            { 
                                $$ = newExpNode(IdK);
                                $$->attr.name = copyString(tokenString);
                            }
                    ;

num                 :   NUMERO
                            { 
                                $$ = newExpNode(ConstK);
                                $$->attr.val = atoi(tokenString);
				                $$->type=Integer;
                            }
                    ;

%%

int yyerror(char* message){
    fprintf(listing, "ERRO SINTÁTICO: %s, LINHA %d\n",tokenString,lineno);
    Error = TRUE;
    return 0;
}

static int yylex(void){
    return getToken();
}

TreeNode * parse(void){
    yyparse();
    return arvore;
}
