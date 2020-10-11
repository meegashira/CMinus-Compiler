#include "globals.h"

/* set NO_PARSE to TRUE to get a scanner-only compiler */
#define NO_PARSE FALSE
/* set NO_ANALYZE to TRUE to get a parser-only compiler */
#define NO_ANALYZE FALSE

/* set NO_CODE to TRUE to get a compiler that does not
 * generate code
 */
#define NO_CODE FALSE

#include "util.h"
#if NO_PARSE
#include "scan.h"
#else
#include "parse.h"
#if !NO_ANALYZE
#include "analyze.h"
#if !NO_CODE
#include "cgen.h"
#endif
#endif
#endif

/* allocate global variables */
int lineno = 0;
FILE * source;
FILE * listing;
FILE * code;

/* allocate and set tracing flags */
int EchoSource = FALSE;
int TraceScan = TRUE;
int TraceParse = TRUE;
int TraceAnalyze = TRUE;
int TraceCode = TRUE;

int Error = FALSE;

int main(int argc, char *argv[]){
    TreeNode * syntaxTree;
    char pgm[120]; /* source code file name */
    if(argc!=2){
        fprintf(stderr,"uso: %s <filename>\n",argv[0]);
        exit(1);
    }
    strcpy(pgm,argv[1]);
    if(strchr(pgm, '.')==NULL)
        strcat(pgm,".cm");
    source = fopen(pgm,"r");
    if(source==NULL){
        fprintf(stderr,"Arquivo %s não encontrado\n",pgm);
        exit(1);
    }
    listing = stdout; /* send listing to screen */
    fprintf(listing,"\nCompilador C-: %s\n",pgm);
    #if NO_PARSE
    while(getToken()!=ENDFILE);
    #else
    syntaxTree = parse();
    if(TraceParse){
        fprintf(listing,"\nÁrvore Sintática:\n");
        printTree(syntaxTree);
    }
    #if !NO_ANALYZE
    if(!Error){
        if(TraceAnalyze)
            fprintf(listing,"\nConstruindo a Tabela de Símbolos...\n");
        buildSymtab(syntaxTree);
        if(TraceAnalyze)
            fprintf(listing,"\nChecando Tipos...\n");
        typeCheck(syntaxTree);
        if(TraceAnalyze)
            fprintf(listing,"\nChecar Tipos terminado!\n");
    }
    #if !NO_CODE
    if(!Error){
        char * codefile;
        int fnlen = strcspn(pgm,".");
        codefile = (char *) calloc(fnlen+4, sizeof(char));
        strncpy(codefile,pgm,fnlen);
        strcat(codefile,".tm");
        code = fopen(codefile,"w");
        if(code==NULL){
            printf("Não é possível abrir %s\n",codefile);
            exit(1);
        }
        if (TraceCode) 
            fprintf(listing, "\nGerando código intermediário...\n");
        codeGen(syntaxTree,codefile);
        free(syntaxTree);
        fclose(code);
        if (TraceCode) 
            fprintf(listing, "\nGeração de código intermediário concluída!\n");
    }
    #endif
    #endif
    #endif
    fclose(source);
    return 0;
}
