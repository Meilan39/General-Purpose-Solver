# General Purpose Solver

## 実行方法
ルートからソルバー実行ファイルを呼び出し、メイン引数として入力ファイルの相対パス「〜」を指定
```
./gpsolve ./~
```
例：既存のILP最小化問題を実行する場合
```
./gpsolve ./tests/unlp.txt
```
## LP ファイル形式
BNF規定：
```
<lp> ::= <objective> <linear_expression> ";"
         "Constrain" {<linear_constraint>}*

<linear_constraint> ::= <linear_expression> <comparison> <linear_expression> ";"

<objective> ::= "Minimize" | "Maximize"
<irrational> ::= "e" | "pi"
<comparison> ::=  "=" | "<=" | ">=" | ">" | "<"
```
例：./tests/lpmax.txt
```
Maximize
    2x1 + 3x2 + 4x3;
Constrain
    3x1 + 2x2 + x3 <= 10;
    2x1 + 3x2 + 3x3 <= 15;
    x1 + x2 - x3 >= 4;
```
## ILP ファイル形式
BNF規定：
```
<ilp> ::= <objective> <linear_expression> ";"
          "Constrain" {<linear_constraint>}* 
          "General" {<variable_constraint}*

<variable_constraint> ::= <variable> ";"
```
例：./tests/ilpmin.txt
```
Minimize
    6x1 + 3x2;
Constrain
    1.4x1 + x2 >= 7;
    -2x1 + x2 <= 2;
    x2 <= 5.5;
General
    x1; x2;
```
## UNLP ファイル形式
BNF規定：
```
<unlp> ::= <objective> <nonlinear_expression> ";"
```
例：./tests/unlp.txt
```
Maximize 
    2x^2 + (e^2 + 1)/4 - log(2, 10);
```
## CNLP ファイル形式
BNF規定：
```
<cnlp> ::= <objective> <nonlinear_expression> ";"
           "Constrain" {<nonlinear_constraint>}*
```
例：./tests/cnlp.txt
```
Minimize 
    2x1^2 + (e^2 + x2)/4 - log(2, 10);
Constrain
    x1^2 - x2^2 <= 1;
    x1 >= 2;
```
