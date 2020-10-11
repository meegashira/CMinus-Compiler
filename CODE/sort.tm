Compilação C- para código intermediário
Arquivo: /mnt/c/Users/mikae/Desktop/Compilador2020/Comp112245/CODE/sort.
# Vec:
# Assign: 
# EndAssign
# Assign: 
# EndAssign
# Assign: 
# EndAssign
# While: 
# If: 
# Assign: 
# EndAssign
# Assign: 
# EndAssign
# EndIf
# Assign: 
# EndAssign
# EndWhile
# Return: 
# EndReturn: 
# Assign: 
# EndAssign
# While: 
# Assign: 
# EndAssign
# Assign: 
# EndAssign
# Assign: 
# EndAssign
# Assign: 
# EndAssign
# Assign: 
# EndAssign
# EndWhile
# Assign: 
# EndAssign
# While: 
# Assign: 
# EndAssign
# Assign: 
# EndAssign
# EndWhile
# Assign: 
# EndAssign
# While: 
# Assign: 
# EndAssign
# EndWhile
# Fim da execução.

********** Código intermediário **********

1: (function, minloc, _, _)
2: (getParam, a, _, _)
3: (getParam, low, _, _)
4: (getParam, high, _, _)
5: (assign, k, low, _)
6: (vectorValue, a, low, t1)
7: (assign, x, t1, _)
8: (addition, low, 1, t2)
9: (assign, i, t2, _)
10: (label, L1, _, _)
11: (lessThan, i, high, t3)
12: (jumpIfFalse, t3, L3, _)
13: (vectorValue, a, i, t4)
14: (lessThan, t4, x, t5)
15: (jumpIfFalse, t5, L2, _)
16: (vectorValue, a, i, t6)
17: (assign, x, t6, _)
18: (assign, k, i, _)
19: (label, L2, _, _)
20: (addition, i, 1, t7)
21: (assign, i, t7, _)
22: (goto, L1, _, _)
23: (label, L3, _, _)
24: (return, k, _, _)
25: (function, sort, _, _)
26: (getParam, a, _, _)
27: (getParam, low, _, _)
28: (getParam, high, _, _)
29: (assign, i, low, _)
30: (label, L4, _, _)
31: (subtraction, high, 1, t8)
32: (lessThan, i, t8, t9)
33: (jumpIfFalse, t9, L5, _)
34: (paramList, 3, _, _)
35: (setParam, high, _, _)
36: (setParam, high, _, _)
37: (setParam, high, _, _)
38: (call, minloc, 3, t10)
39: (assign, k, t10, _)
40: (vectorValue, a, k, t11)
41: (assign, t, t11, _)
42: (vectorValue, a, i, t12)
43: (vectorAddress, a, k, t13)
44: (assign, t13, t12, _)
45: (vectorAddress, a, i, t14)
46: (assign, t14, t, _)
47: (addition, i, 1, t15)
48: (assign, i, t15, _)
49: (goto, L4, _, _)
50: (label, L5, _, _)
51: (return, _, _, _)
52: (function, main, _, _)
53: (assign, i, 0, _)
54: (label, L6, _, _)
55: (lessThan, i, 10, t16)
56: (jumpIfFalse, t16, L7, _)
57: (paramList, 0, _, _)
58: (call, input, 0, t17)
59: (vectorAddress, vet, i, t18)
60: (assign, t18, t17, _)
61: (addition, i, 1, t19)
62: (assign, i, t19, _)
63: (goto, L6, _, _)
64: (label, L7, _, _)
65: (paramList, 3, _, _)
66: (setParam, 10, _, _)
67: (setParam, 10, _, _)
68: (setParam, 10, _, _)
69: (call, sort, 3, t20)
70: (assign, i, 0, _)
71: (label, L8, _, _)
72: (lessThan, i, 10, t21)
73: (jumpIfFalse, t21, L9, _)
74: (paramList, 1, _, _)
75: (vectorValue, vet, i, t22)
76: (setParam, t22, _, _)
77: (call, output, 1, t23)
78: (addition, i, 1, t24)
79: (assign, i, t24, _)
80: (goto, L8, _, _)
81: (label, L9, _, _)
82: (halt, _, _, _)
