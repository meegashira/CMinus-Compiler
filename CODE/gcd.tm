Compilação C- para código intermediário
Arquivo: /mnt/c/Users/mikae/Desktop/Compilador2020/Comp112245/CODE/gcd.t
# If: 
# Return: 
# EndReturn: 
# Return: 
# EndReturn: 
# EndIf
# Assign: 
# EndAssign
# Assign: 
# EndAssign
# Fim da execução.

********** Código intermediário **********

1: (function, gcd, _, _)
2: (getParam, u, _, _)
3: (getParam, v, _, _)
4: (equal, v, 0, t1)
5: (jumpIfFalse, t1, L1, _)
6: (return, u, _, _)
7: (goto, L2, _, _)
8: (label, L1, _, _)
9: (paramList, 2, _, _)
10: (division, u, v, t2)
11: (multiplication, t2, v, t3)
12: (subtraction, u, t3, t4)
13: (setParam, t4, _, _)
14: (division, u, v, t5)
15: (multiplication, t5, v, t6)
16: (subtraction, u, t6, t7)
17: (setParam, t7, _, _)
18: (call, gcd, 2, t8)
19: (return, t8, _, _)
20: (label, L2, _, _)
21: (return, _, _, _)
22: (function, main, _, _)
23: (paramList, 0, _, _)
24: (call, input, 0, t9)
25: (assign, x, t9, _)
26: (paramList, 0, _, _)
27: (call, input, 0, t10)
28: (assign, y, t10, _)
29: (paramList, 1, _, _)
30: (paramList, 2, _, _)
31: (setParam, y, _, _)
32: (setParam, y, _, _)
33: (call, gcd, 2, t11)
34: (setParam, t11, _, _)
35: (call, output, 1, t12)
36: (halt, _, _, _)
