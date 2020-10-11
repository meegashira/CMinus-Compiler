#include "globals.h"
#include "symtab.h"
#include "analyze.h"

static int location = 0;

static void typeError(TreeNode * t, char * message) { 
    fprintf(listing,"ERRO SEMÂNTICO: %s, LINHA: %d\n",message,t->lineno);
    Error = TRUE;
}

static void nullProc(TreeNode * t) { 
    if (t==NULL) return;
    else return;
}

static void traverse(TreeNode * t,void (* preProc) (TreeNode *),void (* postProc) (TreeNode *)) { 
    if (t != NULL){ 
        preProc(t);
        {
         int i;
         for (i=0; i < MAXCHILDREN; i++)
            traverse(t->child[i],preProc,postProc);
        }
        postProc(t);
        traverse(t->sibling,preProc,postProc);
  }
}

static void insertNode( TreeNode * t) { 
    switch (t->nodekind) { 
        case StmtK:
          switch (t->kind.stmt) { 
            case VarK:
                if(st_lookup(t->attr.name, "global") >= 0){
                    switch(st_getIdType(t->attr.name, "global")){
                        case variableT:
                            typeError(t, "Variável já foi declarada anteriormente");
                            break;
                        case vectorT:
                            typeError(t, "Variável tem identificador de vetor");
                            break;
                        case functionT:
                            typeError(t, "Variável tem identificador de função");
                            break;
                    }
                }
                else if(st_lookup(t->attr.name, t->scope) >= 0){
                    switch(st_getIdType(t->attr.name, t->scope)){
                        case variableT:
                            typeError(t, "Variável já foi declarada anteriormente");
                            break;
                        case vectorT:
                            typeError(t, "Variável tem identificador de vetor");
                            break;
                        case functionT:
                            typeError(t, "Variável tem identificador de função");
                            break;
                    }
                }
                else
                    st_insert(t->attr.name, t->scope, variableT, integerT, t->lineno, location++);
                break;

            case VecK:
                if(st_lookup(t->attr.name, "global") >= 0){
                    switch(st_getIdType(t->attr.name, "global")){
                        case variableT:
                            typeError(t, "Vetor tem identificador de variável");
                            break;
                        case vectorT:
                            typeError(t, "Vetor já foi declarado anteriormente");
                            break;
                        case functionT:
                            typeError(t, "Vetor tem identificador de função");
                            break;
                    }
                }
                else if(st_lookup(t->attr.name, t->scope) >= 0){
                    switch(st_getIdType(t->attr.name, t->scope)){
                        case variableT:
                            typeError(t, "Vetor tem identificador de variável");
                            break;
                        case vectorT:
                            typeError(t, "Vetor já foi declarado anteriormente");
                            break;
                        case functionT:
                            typeError(t, "Vetor tem identificador de função");
                            break;
                    }
                }
                else
                    st_insert(t->attr.name, t->scope, vectorT, integerT, t->lineno, location++);
                break;
            case ReturnK:
          		break;
        	case FuncK:
                if(st_lookup(t->attr.name, "global") >= 0){
                    switch(st_getIdType(t->attr.name, "global")){
                        case variableT:
                            typeError(t, "Função tem identificador de variável");
                            break;
                        case vectorT:
                            typeError(t, "Função tem identificador de vetor");
                            break;
                        case functionT:
                            typeError(t, "Função já foi declarado anteriormente");
                            break;
                    }
                }
                else if(st_lookup(t->attr.name, t->scope) >= 0){
                    switch(st_getIdType(t->attr.name, t->scope)){
                        case variableT:
                            typeError(t, "Função tem identificador de variável");
                            break;
                        case vectorT:
                            typeError(t, "Função tem identificador de vetor");
                            break;
                        case functionT:
                            typeError(t, "Função já foi declarado anteriormente");
                            break;
                    }
                }
                else{
                    if(t->type == Integer)
                        st_insert(t->attr.name, t->scope, functionT, integerT, t->lineno, location++);
                    else if(t->type == Void)
                        st_insert(t->attr.name, t->scope, functionT, voidT, t->lineno, location++);
                }
                break;
            default:
                break;
         }
         break;

        case ExpK:

                 switch(t->kind.exp){

                    case IdK:
                        if((st_lookup(t->attr.name, "global") == -1) && (st_lookup(t->attr.name, t->scope) == -1))
                            typeError(t, "Atribuição à variável não declarada anteriormente");
                        else if(st_lookup(t->attr.name, "global") >= 0)
                            st_insert(t->attr.name, "global", 0, 0, t->lineno, 0);
                        else if(st_lookup(t->attr.name, t->scope) >= 0)
                            st_insert(t->attr.name, t->scope, 0, 0, t->lineno, 0);
                        break;

                    case VecIndexK:
                        if((st_lookup(t->attr.name, "global") == -1) && (st_lookup(t->attr.name, t->scope) == -1))
                            typeError(t, "Atribuição à posição de vetor não declarada anteriormente");
                        else if(st_lookup(t->attr.name, "global") >= 0)
                            st_insert(t->attr.name, "global", 0, 0, t->lineno, 0);
                        else if(st_lookup(t->attr.name, t->scope) >= 0)
                            st_insert(t->attr.name, t->scope, 0, 0, t->lineno, 0);
                        break;

                    case CallK:
                        if((st_lookup(t->attr.name, "global") == -1) && (st_lookup(t->attr.name, t->scope) == -1) && (strcmp(t->attr.name,"input")!=0) 
                        && (strcmp(t->attr.name,"output")!=0))
                            typeError(t, "Chamada de função não declarada anteriormente");
                        else if(st_lookup(t->attr.name, "global") >= 0)
                            st_insert(t->attr.name, "global", 0, 0, t->lineno, 0);
                        else if(st_lookup(t->attr.name, t->scope) >= 0)
                            st_insert(t->attr.name, t->scope, 0, 0, t->lineno, 0);
                        break;

                    default:
                        break;
                }
                break;
        default: break;
    }
}

static void checkNode(TreeNode * t){ 
    switch (t->nodekind){ 
        case ExpK:
          switch(t->kind.exp){
            case TypeK:
                break;
            case OpK:
                if((t->child[0]->type != Integer) || (t->child[1]->type != Integer))
                    typeError(t, "Operador aritmético aplicado a não inteiro");
                break;
            case ConstK:
                break;
            case IdK:
                break;
            case CallK:
                break;
            case VecIndexK:
                break;
            default:
                break;
                }
          break;

        case StmtK:
          switch(t->kind.stmt){
            case IfK:
                if(t->child[0]->type != Boolean)
                    typeError(t->child[0], "Condição IF não booleano");
            break;

            case WhileK:
                if(t->child[0]->type != Boolean)
                    typeError(t->child[0], "Condicao WHILE não booleano");
                break;

            case AssignK:
                if(t->child[1]->nodekind == ExpK && t->child[1]->kind.exp == CallK){
                    if((st_getDataType(t->child[1]->attr.name, t->child[1]->scope) != integerT) && (st_getDataType(t->child[1]->attr.name, "global") != integerT) && (strcmp(t->child[1]->attr.name,"input")!=0))
                        typeError(t->child[1], "Atribuição a variável inválida");
                }
                else if(t->child[1]->type != Integer)
                    typeError(t->child[1], "Atribuição a variável inválida");
                break;

            case ReturnK:
                break;

            case VarK:
                break;

            case VecK:
                break;

            case FuncK:
                break;

            default:
                break;
                }
          break;

        default: break;

      }
}

void buildSymtab(TreeNode * syntaxTree){ 
    st_initialize();
    traverse(syntaxTree,insertNode,nullProc);

    if(st_lookup("main","global") == -1){
        fprintf(listing, "Função main não foi declarada");
        Error = TRUE;
    }
    
    if (TraceAnalyze){ 
        fprintf(listing,"\nTabela de símbolos:\n\n");
        printSymTab(listing);
    }
}

void typeCheck(TreeNode * syntaxTree) {
    traverse(syntaxTree,nullProc,checkNode);
}