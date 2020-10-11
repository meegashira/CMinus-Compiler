#!/bin/bash

# Verifica se foi passado um path do programa a ser compilado
if [[ ! -z $1 ]] ; then
    # Caminho do programa a ser compilado
    PROG_PATH=$(realpath -q CODE/$1)
fi

# Compila o código
flex lex/cm.l
bison -d bison/cm.y
gcc -c *.c
gcc -o cm *.o -ly -lfl

# Executa a compilação
./cm ${PROG_PATH} $2

# Limpa o ambiente após a compilação (Remove arquivos gerados)
rm -rf *.tab.*
rm -rf *.yy.*
rm -rf *.gch
rm -rf *.o
rm cm
