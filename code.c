#include "globals.h"
#include "code.h"

const char * toStringInstruction(enum instrucao i) {
    const char * strings[] = {
        "addition", "subtraction", "multiplication", "division", "modulo",
        "bitwiseAnd", "bitwiseOr", "bitwiseXor", "not", "logicalAnd", "logicalOr",
        "shiftLeft", "shiftRight", "vectorValue", "vectorAddress",
        "equal", "notEqual", "lessThan", "lessThanEqualTo",
        "greaterThan", "greaterThanEqualTo", "assign",
        "function", "return", "getParam", "setParam", "call", "paramList",
        "jumpIfFalse", "goto", "label", "syscall", "halt"
    };
    return strings[i];
}

void emitSpaces(int indent){
    int i;
    for(i = 0; i < indent; ++i) {
        fprintf(code, " ");
    }
}

void emitCode(const char * c) {
    fprintf(code, "%s\n", c);
}

void emitComment(const char * c, int indent) {
    if (TraceCode) {
        emitSpaces(indent);
        fprintf(code, "# %s\n", c);
    }
}