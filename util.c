#include "globals.h"
#include "util.h"

void printToken(TokenType token, const char* tokenString){

    switch(token){
        case ELSE: fprintf(listing,"Palavra reservada: %s\n",tokenString); break;
	    case IF: fprintf(listing,"Palavra reservada: %s\n",tokenString); break;
	    case INT: fprintf(listing,"Palavra reservada: %s\n",tokenString); break;
	    case RETURN: fprintf(listing,"Palavra reservada: %s\n",tokenString); break;
	    case VOID: fprintf(listing,"Palavra reservada: %s\n",tokenString); break;
	    case WHILE: fprintf(listing,"Palavra reservada: %s\n",tokenString); break;

        case MAIS: fprintf(listing,"Caracter especial: +\n"); break;
        case MENOS: fprintf(listing,"Caracter especial: - \n"); break;
        case VEZES: fprintf(listing,"Caracter especial: *\n"); break;
        case DIVIDIDO: fprintf(listing,"Caracter especial: /\n"); break;
        case MENORQUE: fprintf(listing,"Caracter especial: <\n"); break;
        case MENORIGUALQUE: fprintf(listing,"Caracter especial: <=\n"); break;
        case MAIORQUE: fprintf(listing,"Caracter especial: > \n"); break;
        case MAIORIGUALQUE: fprintf(listing,"Caracter especial: >=\n"); break;
        case IGUAL: fprintf(listing,"Caracter especial: ==\n"); break;
        case DIFERENTEDE: fprintf(listing,"Caracter especial: != \n"); break;
        case RECEBE: fprintf(listing,"Caracter especial: =\n"); break;
        case PONTOVIRGULA: fprintf(listing,"Caracter especial: ;\n"); break;
        case VIRGULA: fprintf(listing,"Caracter especial: ,\n"); break;
        case EPARENTESES: fprintf(listing,"Caracter especial: ( \n"); break;
        case DPARENTESES: fprintf(listing,"Caracter especial: )\n"); break;
        case ECOLCHETE: fprintf(listing,"Caracter especial: [\n"); break;
        case DCOLCHETE: fprintf(listing,"Caracter especial: ]\n"); break;
        case ECHAVE: fprintf(listing,"Caracter especial: { \n"); break;
        case DCHAVE: fprintf(listing,"Caracter especial: }\n"); break;

        case NUMERO: fprintf(listing, "NUM, valor: %s\n",tokenString); break;
        case ID: fprintf(listing, "ID, identificador: %s\n",  tokenString); break;
        case ENDFILE: fprintf(listing,"Fim do arquivo\n"); break;
        case ERRO: fprintf(listing, "ERRO: %s\n", tokenString); break;
        default: fprintf(listing,"Token não reconhecido: %d\n",token);
    }
}

/* Function newStmtNode creates a new statement
 * node for syntax tree construction
 */
TreeNode * newStmtNode(StmtKind kind){
    TreeNode * t = (TreeNode *) malloc(sizeof(TreeNode));
    int i;
    if(t==NULL)
        fprintf(listing,"ERRO: sem memoria na linha %d\n",lineno);
    else{
        for(i=0;i<MAXCHILDREN;i++)
            t->child[i] = NULL;

        t->sibling = NULL;
        t->nodekind = StmtK;
        t->kind.stmt = kind;
        t->lineno = lineno;
	t->scope = "global";
    }
    return t;
}

/* Function newExpNode creates a new expression 
 * node for syntax tree construction
 */
TreeNode * newExpNode(ExpKind kind){

    TreeNode * t = (TreeNode *) malloc(sizeof(TreeNode));
    int i;
    if(t==NULL)
        fprintf(listing,"ERRO: sem memoria na linha  %d\n",lineno);
    else{
        for(i=0;i<MAXCHILDREN;i++)
            t->child[i] = NULL;
        t->sibling = NULL;
        t->nodekind = ExpK;
        t->kind.exp = kind;
        t->lineno = lineno;
        t->type = Void;
	t->scope = "global";
    }
    return t;
}

/* Function copyString allocates and makes a new
 * copy of an existing string
 */
char * copyString(char * s){

    int n;
    char * t;
    
    if(s==NULL)
        return NULL;
    n = strlen(s)+1;
    t = malloc(n);
    if(t==NULL)
        fprintf(listing,"ERRO: sem memoria na linha %d\n",lineno);
    else
        strcpy(t,s);
    return t;
}

void aggScope(TreeNode* tree, char* scope){
	int i;
	while(tree != NULL){
        tree->scope = copyString(scope);
		for(i=0; i<MAXCHILDREN; ++i){
			aggScope(tree->child[i], scope);
		}
		tree = tree->sibling;
	}
}

/* Variable indentno is used by printTree to
 * store current number of spaces to indent
 */
static int indentno = 0;

/* macros to increase/decrease indentation */
#define INDENT indentno+=4
#define UNINDENT indentno-=4

/* printSpaces indents by printing spaces */
static void printSpaces(void){
    int i;
    for(i=0;i<indentno;i++)
        fprintf(listing," ");
}

/* procedure printTree prints a syntax tree to the 
 * listing file using indentation to indicate subtrees
 */
void printTree(TreeNode * tree){ 

    int i;
    INDENT;
    
    while(tree!=NULL){
        printSpaces();
        if(tree->nodekind==StmtK){
            switch (tree->kind.stmt){
                case IfK:
                    if(tree->child[2] != NULL)
                        fprintf(listing,"SeSenao: \n");
                    else
                        fprintf(listing,"Se: \n");
                    break;
                case WhileK:
                    fprintf(listing,"While: \n");
                    break;
                case AssignK:
                    fprintf(listing,"Atribui: \n");
                    break;
                case ReturnK:
                    if(tree->child[0] != NULL)
                        fprintf(listing,"Retorna: \n");
                    else
                        fprintf(listing,"Retorna: \n");
                    break;
                case VarK:
                    fprintf(listing,"Declaracao de variavel: %s\n", tree->attr.name);
                    break;
                case VecK:
                    fprintf(listing,"Declaracao de vetor: %s[%d]\n", tree->attr.name, tree->attr.val);
                    break;
                case FuncK:
                    if(tree->child[0] != NULL){
                        
                        fprintf(listing,"Declaracao de funcao: %s()\n",
                                tree->attr.name);
                    }
                    else
                        fprintf(listing,"Declaracao de funcao: %s(void)\n",tree->attr.name);
                    break;
                default:
                    fprintf(listing,"Tipo ExpNode Nao Conhecido\n");
                    break;
            }
        }
        else if(tree->nodekind==ExpK){
            switch(tree->kind.exp){
                case OpK:
                    fprintf(listing,"Operador: ");
                    printToken(tree->attr.op,"\0");
                    break;
                case ConstK:
                    fprintf(listing,"Constante: %d\n", tree->attr.val);
                    break;
                case IdK:
                    fprintf(listing,"Identificador: %s\n", tree->attr.name);
                    break;
                case VecIndexK:
                    fprintf(listing,"Indice do vetor: %s[]\n", tree->attr.name);
                    break;
                case TypeK:
                    fprintf(listing,"Tipo: %s\n", tree->attr.name);
                    break;
                case CallK:
                    fprintf(listing,"Chamada de funcao: %s\n", tree->attr.name);
                    break;
                default:
                    fprintf(listing,"Tipo ExpNode Nao Conhecido\n");
                    break;
            }
        }
        else
            fprintf(listing,"Tipo No Nao Conhecido\n");
        for(i=0;i<MAXCHILDREN;i++)
            printTree(tree->child[i]);
        tree = tree->sibling;
    }
    UNINDENT;
}
