#include "globals.h"
#include "symtab.h"
#include "code.h"
#include "cgen.h"

/* number of arguments
 * of current function call to be inserted
 * in the intermediate code instruction list
 */
ParamStack paramHead = NULL;

/* Cabeça da Lista de representações quádruplas */
Quadruple head = NULL;

/* Cabeça da Lista de representações quádruplas */
LocationStack locationHead = NULL;

/* Numero para geração de nomes de variáveis temporárias */
static int temporario = 1;

/* Numero para geração de labels */
static int label = 1;

Operand operandoAtual;

InstructionKind instrucaoAtual;

/* Útil para saber quando deve-se retornar o endereço do vetor */
Quadruple ultimaQuadrupla;

static int linha = 0;

/* temporary string to help printing text */
static char tempString[20];

/* Protótipo para o gerador de código interno recursivo */
static void cGen (TreeNode * tree);

static char * createLabelName() {
    char * temp = (char *) malloc(5);
    sprintf(temp, "L%d", label);
    ++label;
    return temp;
}

static char * createTemporaryOperandName() {
    char * temp = (char *) malloc(5);
    sprintf(temp, "t%d", temporario);
    ++temporario;
    return temp;
}

Operand createOperand(void) {
    Operand op = (Operand) malloc(sizeof(struct operand));
    return op;
}

static Operand createTemporaryOperand() {
    Operand temp = createOperand();
    temp->kind = String;
    temp->contents.variable.scope = NULL;
    temp->contents.variable.name = createTemporaryOperandName();
    return temp;
}

/** Procedure genStmt generates code at a statement node */
static void genStmt(TreeNode * tree) {
    Quadruple * q;
    Operand op1, op2, op3;
    TreeNode * p1, * p2, * p3;
    switch (tree->kind.stmt) {
        case IfK:
            emitComment("If: ",0);
            p1 = tree->child[0];
            p2 = tree->child[1];
            p3 = tree->child[2];
            /* Gera código para a expressão de teste */
            cGen(p1);
            /* Atribui como o primeiro operando */
            op1 = operandoAtual;
            /* Atribui o tipo de instrução */
            instrucaoAtual = JUMPFALSE; //jump if false
            /* Cria e insere uma nova representação em código intermediário */
            q = insertQuad(createQuad(instrucaoAtual, op1, NULL, NULL));
            /* Salva a IR do if para atualizar com o label que representa o fim do bloco then */
            pushLocation(createLocation(q));
            /* Gera código para o bloco then */
            cGen(p2);
            /* set second operand */
            op2 = createOperand();
            op2->kind = String;
            op2->contents.variable.name = createLabelName();
            op2->contents.variable.scope = tree->scope;
            /* update if intermediate code instruction */
            updateLocation(op2);
            popLocation();

            if(p3 != NULL) {
                q = insertQuad(createQuad(GOTO, NULL, NULL, NULL));
                pushLocation(createLocation(q));
            }

            /* Label usado para marcar o fim do bloco then */
            insertQuad(createQuad(LBL, op2, NULL, NULL));
            cGen(p3);

            if(p3 != NULL) {
                op1 = createOperand();
                op1->kind = String;
                op1->contents.variable.name = createLabelName();
                op1->contents.variable.scope = tree->scope;
                /* update if intermediate code instruction */
                updateLocation(op1);
                popLocation();

                /* Label usado para marcar o fim do bloco else */
                insertQuad(createQuad(LBL, op1, NULL, NULL));
            }
            emitComment("EndIf",0);
            break; /* IfK */

        case WhileK:
            emitComment("While: ",0);
            p1 = tree->child[0];
            p2 = tree->child[1];

            op1 = createOperand();
            op1->kind = String;
            op1->contents.variable.name = createLabelName();
            op1->contents.variable.scope = tree->scope;
            insertQuad(createQuad(LBL, op1, NULL, NULL));
            /* build code for test expression */
            cGen(p1);
            /* set as first operand */
            op2 = operandoAtual;
            /* set intermediate code kind */
            instrucaoAtual = JUMPFALSE; //jump if false
            /* Cria e insere uma nova representação em código intermediário */
            q = insertQuad(createQuad(instrucaoAtual, op2, NULL, NULL));
            /* Salva a IR do if para atualizar com o label que representa o fim do bloco then */
            pushLocation(createLocation(q));
            /* build code for while block */
            cGen(p2);
            /* go back to while test expression */
            /* set intermediate code kind */
            instrucaoAtual = GOTO;
            /* Cria e insere uma nova representação em código intermediário */
            insertQuad(createQuad(instrucaoAtual, op1, NULL, NULL));

            op3 = createOperand();
            op3->kind = String;
            op3->contents.variable.name = createLabelName();
            op3->contents.variable.scope = tree->scope;
            insertQuad(createQuad(LBL, op3, NULL, NULL));
            updateLocation(op3);
            popLocation();
            emitComment("EndWhile",0);
            break; /* WhileK */

        case AssignK:
            emitComment("Assign: ",0);
            p1 = tree->child[0];
            p2 = tree->child[1];
            /* Gera código para o operando da direita */
            cGen(p2);
            /* Atribui como o segundo operando */
            op2 = operandoAtual;
            /* Gera código para o operando da esquerda */
            cGen(p1);
            /* Atribui como o primeiro operando */
            op1 = operandoAtual;
            op3 = NULL;

            // ------------------- Caso especial ------------------- //
            /* Na atribuição, se o operando da esquerda for um vetor, deve-se
             * fazer um store na posição de memória dessa variável
             */
            if(p1->kind.exp == VecIndexK) {
                ultimaQuadrupla->instruction = VEC_ADDR;
                // Se o índice for uma constante, adiciona o offset como o terceiro operando (para codigo objeto)
                if(ultimaQuadrupla->op2->kind == IntConst) {
                    op3 = createOperand();
                    op3->kind = IntConst;
                    op3->contents.val = ultimaQuadrupla->op2->contents.val;
                    op1 = ultimaQuadrupla->op1;
                }
            }

            instrucaoAtual = ASN;
            insertQuad(createQuad(instrucaoAtual, op1, op2, op3));
            emitComment("EndAssign",0);
            break; /* AssignK */
        
        case ReturnK:
            emitComment("Return: ",0);
            p1 = tree->child[0];
            /* Gera codigo para a expressao */
            cGen(p1);
            if(p1) {
                /* Atribui como o primeiro operando */
                op1 = operandoAtual;
            } else {
                /* Nao retorna nada */
                op1 = NULL;
            }
            /* Atribui o tipo de instrucao */
            instrucaoAtual = RTN; //return
            /* Cria e insere uma nova representação em codigo intermediario */
            insertQuad(createQuad(instrucaoAtual, op1, NULL, NULL));
            emitComment("EndReturn: ",0);
            break; /* ReturnK */

        case VarK:
            emitComment("Var:",0);
            break;

        case VecK:
            emitComment("Vec:",0);
            break; /* VecK */

        case FuncK:
            verificaFimInstrucaoAnterior();
            op1 = createOperand();
            op1->kind = String;
            op1->contents.variable.name = tree->attr.name;
            op1->contents.variable.scope = tree->scope;
            insertQuad(createQuad(FUNC, op1, NULL, NULL));

            /* lista de parametros */
            p1 = tree->child[0];
            while(p1 != NULL) {
                op2 = createOperand();
                op2->kind = String;
                op2->contents.variable.name = p1->child[0]->attr.name;
                op2->contents.variable.scope = p1->child[0]->scope;
                insertQuad(createQuad(GET_PARAM, op2, NULL, NULL));
                p1 = p1->sibling;
            }

            /* codigo para bloco da funcao */
            p2 = tree->child[1];
            cGen(p2);
            break;
        
        default:
            break;


    }
} /* genStmt */

/* Procedure genExp generates code at an expression node */
static void genExp(TreeNode * tree) {
    Quadruple q;
    Operand op1, op2, op3;
    TreeNode * p1, * p2, * p3;
    int qtdParams, offset, display = -1;
    switch (tree->kind.stmt) {
        case OpK:
            p1 = tree->child[0];
            p2 = tree->child[1];
            /* Gera código para o operando da esquerda */
            cGen(p1);
            /* Atribui como o primeiro operando */
            op1 = operandoAtual;
            /* Gera código para o operando da direita */
            cGen(p2);
            /* Atribui como o segundo operando */
            op2 = operandoAtual;

            switch (tree->attr.op) {
                case MAIS:
                    instrucaoAtual = ADD;
                    break;
                case MENOS:
                    instrucaoAtual = SUB;
                    break;
                case VEZES:
                    instrucaoAtual = MULT;
                    break;
                case DIVIDIDO:
                    instrucaoAtual = DIV;
                    break;
                case IGUAL:
                    instrucaoAtual = EQ;
                    break;
                case DIFERENTEDE:
                    instrucaoAtual = NE;
                    break;
                case MENORQUE:
                    instrucaoAtual = LT;
                    break;
                case MENORIGUALQUE:
                    instrucaoAtual = LET;
                    break;
                case MAIORQUE:
                    instrucaoAtual = GT;
                    break;
                case MAIORIGUALQUE:
                    instrucaoAtual = GET;
                    break;
                default:
                    emitComment("ERRO DE OpK",0);
                    instrucaoAtual = HALT;
                    break;
            }
            
            operandoAtual = createTemporaryOperand();
            insertQuad(createQuad(instrucaoAtual, op1, op2, operandoAtual));
            break; /* OpK */

        case ConstK:
            /* Atribui o operando atual */
            operandoAtual = createOperand();
            operandoAtual->kind = IntConst;
            operandoAtual->contents.val = tree->attr.val;
            break; /* CONSTK */

        case IdK:
            /* Atribui o operando atual */
            operandoAtual = createOperand();
            operandoAtual->kind = String;
            operandoAtual->contents.variable.name = tree->attr.name;
            operandoAtual->contents.variable.scope = tree->scope;
            break; /* IdK */

        case CallK:
            /* Argumentos */
            p1 = tree->child[0];
            /* Atribui o primeiro operando */
            op1 = createOperand();
            op1->kind = String;
            op1->contents.variable.name = tree->attr.name;
            op1->contents.variable.scope = tree->scope;
            /* Atribui o segundo operando */
            qtdParams = getQuantidadeParametros(tree);
            pushParam(&qtdParams);
            op2 = createOperand();
            if(qtdParams > 0) {
                op2->kind = IntConst;
                op2->contents.val = qtdParams;
            } else {
                op2->kind = IntConst;
                op2->contents.val = 0;
            }
            /* build code for function call */
            instrucaoAtual = PARAM_LIST;
            op3 = createOperand();
            op3->kind = IntConst;
            op3->contents.val = getQuantidadeParametros(tree);
            insertQuad(createQuad(instrucaoAtual, op3, NULL, NULL));
            while(p1 != NULL) {
                cGen(p1);
                /* Atribui o tipo de instrução */
                instrucaoAtual = SET_PARAM;
                /* Cria e insere uma nova representação em código intermediário */
                insertQuad(createQuad(instrucaoAtual, operandoAtual, NULL, NULL));
                /* Decrementa qtdParams */
                --qtdParams;
                /* Se for um chamado de OUTPUT, verifica o display de exibição */
                /*if(!strcmp(tree->attr.name, "output") && p1->sibling == NULL) {
                    display = p1->attr.val;
                }*/
                p1 = p1->sibling;
            }
            popParam();
            /* Atribui o tipo de instrução */
            instrucaoAtual = CALL;
            /* Armazena possível offset para manipulação da memória reservada de índice de programas */
            if (op1->kind == IntConst) {
                offset = operandoAtual->contents.val;
            }
            /* Atualiza o operando atual */
            operandoAtual = createTemporaryOperand();
            /* Cria e insere uma nova representação em código intermediário */
            q = createQuad(instrucaoAtual, op1, op2, operandoAtual);
            /*if(display != -1) {
                q->display = display;
            }*/
            insertQuad(q);
            break;

        case VecIndexK:
            p1 = tree->child[0];
            /* Atualiza o operando atual como o id do vetor e seta como op1 */
            operandoAtual = createOperand();
            operandoAtual->kind = String;
            operandoAtual->contents.variable.name = tree->attr.name;
            operandoAtual->contents.variable.scope = tree->scope;
            op1 = operandoAtual;
            /* Gera código para a posição do vetor */
            cGen(p1);
            /* Indice do vetor */
            op2 = operandoAtual;
            /* Atribui a instrução atual */
            instrucaoAtual = VEC;
            /* Temporário */
            op3 = createTemporaryOperand();
            /* Atualiza o operando atual */
            operandoAtual = op3;
            /* Cria e insere uma nova representação em código intermediário */
            ultimaQuadrupla = createQuad(instrucaoAtual, op1, op2, op3);
            insertQuad(ultimaQuadrupla);
            
            break;
            
        default:
            break;
    }
} /* genExp */

/* Procedimento cGen gera o código recursivamente
 * pela árvore sintática
 */
static void cGen(TreeNode * tree) {
    if (tree != NULL) {
        switch (tree->nodekind) {
            case StmtK:
                genStmt(tree);
                break;
            case ExpK:
                genExp(tree);
                break;
            default:
                break;
        }
        cGen(tree->sibling);
    }
}

void verificaFimInstrucaoAnterior(void) {
    if(head != NULL) {
        Quadruple temp = head;
        while(temp->next != NULL) {
            temp = temp->next;
        }
        /* Insere um return forçadamente caso nao haja no codigo de alto nivel */
        if(temp->instruction != RTN) {
            insertQuad(createQuad(RTN, NULL, NULL, NULL));
        }
    }
}

/**********************************************/
/* the primary function of the code generator */
/**********************************************/
/* Procedure codeGen generates code to a code
 * file by traversal of the syntax tree. The
 * second parameter (codefile) is the file name
 * of the code file, and is used to print the
 * file name as a comment in the code file
 */
void codeGen(TreeNode * syntaxTree, char * codefile) {
    char * s = (char *) malloc(strlen(codefile) + 7);
    strcpy(s,"Arquivo: ");
    strcat(s,codefile);
    emitCode("Compilação C- para código intermediário");
    emitCode(s);
    while(syntaxTree!=NULL) {
        cGen(syntaxTree->child[0]);
        syntaxTree=syntaxTree->sibling;
    }
    /* finish */
    emitComment("Fim da execução.", 0);
    insertQuad(createQuad(HALT, NULL, NULL, NULL));   

    emitCode("\n********** Código intermediário **********\n");
    printIntermediateCode();
}

void printIntermediateCode() {
    Quadruple q = head;
    char quad[100];

    while(q != NULL) {
        sprintf(quad, "%d: (", q->linha);
        strcat(quad, toStringInstruction(q->instruction));

        if(q->op1 == NULL) {
            strcat(quad, ", _");
        } else if(q->op1->kind == String) {
            strcat(quad, ", ");
            strcat(quad, q->op1->contents.variable.name);
        } else {
            sprintf(tempString, ", %d", q->op1->contents.val);
            strcat(quad,tempString);
        }

        if(q->op2 == NULL) {
            strcat(quad, ", _");
        } else if(q->op2->kind == String) {
            strcat(quad, ", ");
            strcat(quad, q->op2->contents.variable.name);
        } else {
            sprintf(tempString, ", %d", q->op2->contents.val);
            strcat(quad,tempString);
        }

        if(q->op3 == NULL) {
            strcat(quad, ", _)");
        } else if(q->op3->kind == String) {
            strcat(quad, ", ");
            strcat(quad, q->op3->contents.variable.name);
            strcat(quad, ")");
        } else {
            sprintf(tempString, ", %d", q->op3->contents.val);
            strcat(quad,tempString);
            strcat(quad, ")");
        }
        emitCode(quad);
        q = q->next;
    }
}

void pushLocation(LocationStack ls) {
    if(locationHead == NULL) {
        locationHead = ls;
        locationHead->next = NULL;
    } else {
        ls->next = locationHead;
        locationHead = ls;
    }
}

void popLocation() {
    if(locationHead != NULL) {
        LocationStack ls = locationHead;
        locationHead = locationHead->next;
        free(ls);
        ls = NULL;
    }
}

LocationStack createLocation(Quadruple * quad) {
    LocationStack ls = (LocationStack) malloc(sizeof(struct Location));
    ls->quad = quad;
    ls->next = NULL;
    return ls;
}

void updateLocation(Operand op) {
    Quadruple q = *locationHead->quad;
    if(q->instruction != JUMPFALSE) {
        q->op1 = op;
    } else {
        q->op2 = op;
    }
    *locationHead->quad = q;
}

void pushParam(int * count) {
    ParamStack ps = (ParamStack) malloc(sizeof(struct Param));
    ps->count = count;
    ps->next = NULL;
    if(paramHead == NULL) {
        paramHead = ps;
    } else {
        ps->next = paramHead;
        paramHead = ps;
    }
}

void popParam(void) {
    if(paramHead != NULL) {
        ParamStack ps = paramHead;
        paramHead = paramHead->next;
        free(ps);
        ps = NULL;
    }
}

Quadruple createQuad(InstructionKind instruction, Operand op1, Operand op2, Operand op3) {
    Quadruple q = (Quadruple) malloc(sizeof(struct Quad));
    q->instruction = instruction;
    q->op1 = op1;
    q->op2 = op2;
    q->op3 = op3;
    q->linha = ++linha;
    q->next = NULL;
    return q;
}

Quadruple * insertQuad(Quadruple q) {
    Quadruple * ptr = (Quadruple *) malloc(sizeof(struct Quad));
    if(head == NULL) {
        head = q;
        head->next = NULL;
        ptr = &head;
    } else {
        Quadruple temp = head;
        while(temp->next != NULL) {
            temp = temp->next;
        }
        temp->next = q;
        temp->next->next = NULL;
        ptr = &temp->next;
    }
    return ptr;
}

Quadruple getCodigoIntermediario(void) {
    return head;
}