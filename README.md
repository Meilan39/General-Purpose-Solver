# General Purpose Solver

## 実行方法
Makeでプロジェクトを実行ファイルを生成
```
make
```
ルートからソルバー実行ファイルに入力ファイルのパスを入力
```
./gpsolve ./~
```
### 例：既存のLP問題を実行する場合
ルート内の「example」ディレクトリの中にある「lpmin.txt」。
```
Minimize
    6x1 + 3x2;
Constrain
    1.4x1 + x2 >= 7;
    -2x1 + x2 <= 2;
    x2 <= 5.5;
```
次のように実行する。
```
./gpsolve ./examples/lpmin.txt
```
結果ファイルは入力ファイルと同名の「.sol」ファイルに出力
```
Solution file for : ./examples/lp.txt

Optimum : 23.647059
	x1  =    1.47;
	x2  =    4.94;
Slack Variables
	s1  =    0.00;
	s2  =    0.00;
	s3  =    0.56;
```
## LP ファイル形式
BNF規定：
```
<lp> ::= <objective> <linear_expression> ;
         Constrain {<linear_constraint>}*

<linear_expression> ::= <linear_additive>

<linear_additive> ::= <linear_multiplicative> {<linear_additive_suffix>}*
<linear_additive_suffix> ::=  + <linear_multiplicative>
                           |  - <linear_multiplicative>

<linear_multiplicative> ::= {<sign>}? {<real>}? {*}? <variable>
                          | {<sing>}? <real>

<linear_constraint> ::= <linear_expression> <comparison> <linear_expression> ;

<real> ::= <rational> | <irrational>
<rational> ::= <decimal> | <number> | <zero>
<objective> ::= Minimize | Maximize
<irrational> ::= e | pi
<comparison> ::=  = | <= | >= | > | <
```
例：./tests/lp.txt
```
Minimize
    6x1 + 3x2;
Constrain
    1.4x1 + x2 >= 7;
    -2x1 + x2 <= 2;
    x2 <= 5.5;
```
## ILP ファイル形式
BNF規定：
```
<ilp> ::= <objective> <linear_expression> ;
          Constrain {<linear_constraint>}* 
          Integer {<variable_constraint}*

<linear_expression> ::= <linear_additive>

<linear_additive> ::= <linear_multiplicative> {<linear_additive_suffix>}*
<linear_additive_suffix> ::=  + <linear_multiplicative>
                           |  - <linear_multiplicative>

<linear_multiplicative> ::= {<sign>}? {<real>}? {*}? <variable>
                          | {<sing>}? <real>

<linear_constraint> ::= <linear_expression> <comparison> <linear_expression> ;

<variable_constraint> ::= <variable> ;

<real> ::= <rational> | <irrational>
<rational> ::= <decimal> | <number> | <zero>
<objective> ::= Minimize | Maximize
<irrational> ::= e | pi
<comparison> ::=  = | <= | >= | > | <
```
例：./tests/ilp.txt
```
Minimize
    6x1 + 3x2;
Constrain
    1.4x1 + x2 >= 7;
    -2x1 + x2 <= 2;
    x2 <= 5.5;
Integer
    x1; x2;
```
## UNLP ファイル形式
BNF規定：
```
<unlp> ::= <objective> <nonlinear_expression> ;
           Bound {<variable_bound>}*

<nonlinear_expression> ::= <nonlinear_additive>

<nonlinear_additive> ::= <nonlinear_multiplicative> {<nonlinear_additive_suffix>}*
<nonlinear_additive_suffix> ::=  + <nonlinear_multiplicative>
                               | - <nonlinear_multiplicative>

<nonlinear_multiplicative> ::= {<sign>}? <nonlinear_exponential> {<nonlinear_multiplicative_suffix>}*
<nonlinear_multiplicative_suffix> ::=  * <nonlinear_exponential>
                                     | / <nonlinear_exponential>
                                     | <nonlinear_exponential>

<nonlinear_exponential> ::= <nonlinear_primary> {<nonlinear_exponential_suffix>}*
<nonlinear_exponential_suffix> ::= ^ <nonlinear_primary>

<nonlinear_primary> ::= <real>
                      | <functions>
                      | <variable>
                      | <nonlinear_parenthesis>

<nonlinear_parenthesis> ::= ( <nonlinear_expression> )
                          | { <nonlinear_expression> }

<functions> ::= <root> | <sqrt> | <log> | <sqrt> | <log> | <ln> | <sin> | <cos> | <tan> 
<root> ::= root ( <number> , <nonlinear_additive> )
<sqrt> ::= sqrt <nonlinear_primary>
<log> ::= log ( <number> , <nonlinear_additive> )
<ln> ::= ln <nonlinear_primary>
<sin> ::= sin <nonlinear_primary>
<cos> ::= cos <nonlinear_primary>
<tan> ::= tan <nonlinear_primary>

<nonlinear_constraint> ::= <nonlinear_expression> = 0 ;
                         | <nonlinear_expression> ;

<variable_bound> ::= {<sign>}? <real_number> < <variable> < {<sign>}? <real_number> ;

<real_number> ::= <rational> | <irrational>

<rational> ::= <decimal> | <number> | <zero>

<objective> ::= Minimize | Maximize

<irrational> ::= e | pi

<comparison> ::=  = | <= | >= | > | <
```
例：./tests/unlp.txt
```
Minimize
    (1.5 - x + x*y)^2 + (2.25 - x + x*y^2)^2 + (2.625 - x + x*y^3)^2;
Bound
    -2 < x < 6;
    -4 < y < 4;
```
## CNLP ファイル形式
CNLP問題は等号条件のみに対応している。
BNF規定：
```
<cnlp> ::= <objective> <nonlinear_expression> ;
           Bound {<variable_bound>}*
           Constrain {<nonlinear_constraint>}*

<nonlinear_expression> ::= <nonlinear_additive>

<nonlinear_additive> ::= <nonlinear_multiplicative> {<nonlinear_additive_suffix>}*
<nonlinear_additive_suffix> ::=  + <nonlinear_multiplicative>
                               | - <nonlinear_multiplicative>

<nonlinear_multiplicative> ::= {<sign>}? <nonlinear_exponential> {<nonlinear_multiplicative_suffix>}*
<nonlinear_multiplicative_suffix> ::=  * <nonlinear_exponential>
                                     | / <nonlinear_exponential>
                                     | <nonlinear_exponential>

<nonlinear_exponential> ::= <nonlinear_primary> {<nonlinear_exponential_suffix>}*
<nonlinear_exponential_suffix> ::= ^ <nonlinear_primary>

<nonlinear_primary> ::= <real>
                      | <functions>
                      | <variable>
                      | <nonlinear_parenthesis>

<nonlinear_parenthesis> ::= ( <nonlinear_expression> )
                          | { <nonlinear_expression> }

<functions> ::= <root> | <sqrt> | <log> | <sqrt> | <log> | <ln> | <sin> | <cos> | <tan> 
<root> ::= root ( <number> , <nonlinear_additive> )
<sqrt> ::= sqrt <nonlinear_primary>
<log> ::= log ( <number> , <nonlinear_additive> )
<ln> ::= ln <nonlinear_primary>
<sin> ::= sin <nonlinear_primary>
<cos> ::= cos <nonlinear_primary>
<tan> ::= tan <nonlinear_primary>

<nonlinear_constraint> ::= <nonlinear_expression> = 0 ;
                         | <nonlinear_expression> ;

<variable_bound> ::= {<sign>}? <real_number> < <variable> < {<sign>}? <real_number> ;

<real_number> ::= <rational> | <irrational>

<rational> ::= <decimal> | <number> | <zero>

<objective> ::= Minimize | Maximize

<irrational> ::= e | pi

<comparison> ::=  = | <= | >= | > | <
```
例：./tests/cnlp.txt
```
Minimize
    sin(x) + cos(y);
Bound
    -10 < x < 10;
    -10 < y < 10;
Constrain
    x^2 + y^2 - 4 = 0;
```
