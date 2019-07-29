%{
#define YYERROR_VERBOSE
#include<stdio.h>
#include<string.h>
#include "hashtable.h"
#include "iloc_emitter.h"
#include "att.h"
//#include "attr.h"
//#include "instrutil.h"
//#include "symtab.h"
int lineNum = 1;
int errorNum = 0;
int numVar = 0;
int dim = 0;
int curType = -1;
FILE *outfile;

%}

%union {
    var variable; 
    regInfo Reg;
}

%token <int> INT
%token <int> CHAR
%token <int> AND OR
%token <Reg> IF THEN ELSE
%token <Reg> FOR
%token <int> PROCEDURE
%token <int> READ
%token <int> TO
%token <Reg> WHILE
%token <int> WRITE
%token <int> BY
%token <int> ADD
%token <int> MINUS
%token <int> MUL
%token <int> DIVIDE
%token <int> LT
%token <int> LE
%token <int> EQ
%token <int> GT
%token <int> GE
%token <int> NE
%token <int> OPEN_CB
%token <int> CLOSE_CB
%token <int> COLON
%token <int> SEMI_COLON
%token <int> OPEN_SQUARE_BRACKETS
%token <int> CLOSE_SQUARE_BRACKETS
%token <int> OPEN_ROUND_BRACKETS
%token <int> CLOSE_ROUND_BRACKETS
%token <int> EQUAL
%token <int> COMMA
%token <variable> NAME
%token <variable> NUMBER
%token <variable> CHARCONST
%token <int> NOT
%token <int> ERRORT


%type <int> Procedure
%type <Reg> Decls
%type <Reg> Decl
%type <Reg> Type
%type <Reg> SpecList
%type <Reg> Spec

%type <Reg> Exprs
%type <Reg> Expr
%type <Reg> Term
%type <Reg> Bounds
%type <Reg> Bound
%type <Reg> Stmts
%type <Reg> Stmt
%type <Reg> AndTerm
%type <Reg> Bool
%type <Reg> OrTerm
%type <Reg> Factor
%type <Reg> RelExpr
%type <Reg> Reference
%type <Reg> WithElse
%type <Reg> allStmts
%type <Reg> IfPre
%type <Reg> ElsePre

%start Procedure

%%
Reference: NAME 
         { $$.name = $1.c; 
            sym_entry* entry = search($1.c); 
            if(entry == NULL){
               // printf("Variable %s does not delcare.\n", $1.c);
                yyerror("Variable does not delcare.\n");
            }
            else{
              $$.type = entry -> type;
              $$.name = entry -> name;
              $$.target = entry -> reg;
            }
        } 
         | NAME OPEN_SQUARE_BRACKETS Exprs CLOSE_SQUARE_BRACKETS{
            sym_entry* entry = search($1.c);
            if(entry == NULL){
                // printf("Variable %s does not delcare.\n", $1.c);
                 yyerror("Variable does not delcare.\n");
             }
             else{
               int base = entry -> base;
               int size = 4;
               if(entry -> type == TYPE_CHAR)
                    size = 1;
               int new_reg = nextReg();
               if($3.dim == 1){
                    int offset = base + $3.val*size;
                    int new_reg_offset = nextReg();
                    int new_reg_size = nextReg();
                   // printf("Computing the arrary target:\n");
                    
                    emit(-1, loadI, size, new_reg_size, 0);
                    
                    emit(-1, mult, new_reg_size, $3.target, new_reg_offset);
                    emit(-1, addI, new_reg_offset, base, new_reg);
                    
                   // printf("real address: %d\n", offset);
               }
               else{
                    int length_2d = entry -> array_end[1] - entry -> array_start[1] + 1;
                    int new_reg_offset = nextReg();
                    int new_reg_size = nextReg();
                   // printf("Computing the arrary target:");
                    emit(-1, loadI, size, new_reg_size, 0);
                    emit(-1, multI, $3.bounds[1], length_2d, new_reg_offset);
                    emit(-1, add, $3.bounds[0], new_reg_offset, new_reg_offset);
                    emit(-1, mult, new_reg_offset, new_reg_size, new_reg_offset);
                    emit(-1, addI, new_reg_offset, base, new_reg);
                    
                    //printf("bound[1]: %d bound[0]: %d\n", $3.bounds[1], $3.bounds[0]);


               }
               int new_reg_loaded = nextReg();
               if(entry -> type == TYPE_INT){
                    emit(-1, load, new_reg, new_reg_loaded, -1);
               }
               else if(entry -> type == TYPE_CHAR){
                    emit(-1, cload, new_reg, new_reg_loaded, -1);
               }
               else{
                    yyerror("Unknow reference type.");
               }
                
               $$.type = entry -> type;
               $$.name = entry -> name;
               $$.target = new_reg_loaded;
               $$.target_addr = new_reg;
                
             }
            //printf("hi, this is an array name.\n");
         };

Expr: Expr ADD Term{
        if ($3.type == TYPE_INT && $1.type == TYPE_INT){
                if($3.target == -1 && $1.target == -1){
                    $$.val = $1.val + $3.val;
                    $$.target = -1;
                }
                else if ($1.target == -1 && $3.target != -1){
                    $$.target = nextReg();
                    sym_entry* entry = search($3.name);
                    emit(-1, addI, $3.target, $1.val, $$.target);
                    $$.val = 0;
                }
                else if($1.target != -1 && $3.target == -1){
                    $$.target = nextReg();
                    emit(-1, addI, $1.target, $3.val, $$.target);
                }
                else{
                    $$.target = nextReg();
                    emit(-1, add, $1.target, $3.target, $$.target);
                }
                
                $$.type = $3.type;
            //printf("%d\t %d\t %d \n", $1.val, $3.val, $$.target);
        }
        else{
            yyerror("This compiler does not provide char + char or int + char");
        }
    } 
    | Expr MINUS Term {
       if ($3.type == TYPE_INT && $1.type == TYPE_INT){
                 if($3.target == -1 && $1.target == -1){
                     $$.val = $1.val - $3.val;
                     $$.target = -1;
                 }
                 else if ($1.target == -1 && $3.target != -1){
                     $$.target = nextReg();
                     sym_entry* entry = search($3.name);
                     $1.target = nextReg();
                     emit(-1, loadI, $1.val, $1.target, -1);
                     emit(-1, sub, $1.target, $3.target, $$.target);
                     $$.val = 0;
                 }
                 else if($1.target != -1 && $3.target == -1){
                     $$.target = nextReg();
                     emit(-1, subI, $1.target, $3.val, $$.target);
                 }
                 else{
                     $$.target = nextReg();
                     emit(-1, sub, $1.target, $3.target, $$.target);
                 }

                 $$.type = $3.type;
             //printf("%d\t %d\t %d \n", $1.val, $3.val, $$.target);
         }
         else{
             yyerror("This compiler does not provide char - char or int - char");
         }
    }
    | Term{
        $$.target = $1.target;
        $$.type = $1.type;
        if($1.type == TYPE_INT){
            $$.val = $1.val;
        }
        else{
            //$$.valC = $1.valC;
        }
        $$.name = $1.name;
    };

Exprs:Expr COMMA Exprs {
        $$.dim = $3.dim + 1;
        if($1.target != -1){
            $$.target = $1.target;          
        }
        else{
            int new_reg = nextReg();
             emit(-1, loadI, $1.val, new_reg, -1);
             $$.target = new_reg;
        }
        // printf("bound: %d\n", $$.target);
        $$.bounds[$$.dim - 2] = $3.bounds[$$.dim - 2];
        $$.bounds[$$.dim - 1] = $$.target;
        
     }
     | Expr{
        $$.dim = 1;
        if($1.target != -1){
            $$.target = $1.target;
        }
        else{
            int new_reg = nextReg();
            emit(-1, loadI, $1.val, new_reg, -1);
            $$.target = new_reg;
        } 
        // printf("%d\n", $$.target);
        $$.bounds[0] = $$.target;
        $$.type = $1.type;
        $$.val = $1.val;
        $$.name = $1.name;
     };

Term: Term MUL Factor{
            if($1.type == $3.type && $1.type == TYPE_INT){
             if($1.target == -1 && $3.target == -1){
                 $$.val = $1.val * $3.val;
                 $$.target = -1;
             }
             else if($3.target == -1){
                 int new_reg = nextReg();
                 $$.target = new_reg;
                 emit(-1, multI, $1.target, $3.val, $$.target);

             }
             else if($1.target == -1){
                 int new_reg = nextReg();
                  $$.target = new_reg;
                  $1.target = new_reg;
                  emit(-1, loadI, $1.val, $1.target, -1);
                  emit(-1, mult, $1.target, $3.target, $$.target);
             }
             else{
                 int new_reg = nextReg();
                   $$.target = new_reg;
                   emit(-1, mult, $1.target, $3.target, $$.target);
             }
             $$.type = TYPE_INT;
        }
        else{
             yyerror("Only integer can be mutiplied or multiply.");
        }
    } 
    | Term DIVIDE Factor{
       if($1.type == $3.type && $1.type == TYPE_INT){
            if($1.target == -1 && $3.target == -1){
                $$.val = $1.val / $3.val;
                $$.target = -1;
            }
            else if($3.target == -1){
                int new_reg = nextReg();
                $$.target = new_reg;
                emit(-1, divI, $1.target, $3.val, $$.target);
            
            }
            else if($1.target == -1){
                int new_reg = nextReg();
                 $$.target = new_reg;
                 $1.target = new_reg;
                 emit(-1, loadI, $1.val, $1.target, -1);
                 emit(-1, divide, $1.target, $3.target, $$.target);
            }
            else{
                int new_reg = nextReg();
                  $$.target = new_reg;
                  emit(-1, divide, $1.target, $3.target, $$.target);
            }
            $$.type = TYPE_INT;
       }
       else{
            yyerror("Only integer can be devide or devide.");
       }
    } 
    | Factor{
                $$.target = $1.target;
                $$.type = $1.type;
                if($1.type == TYPE_INT)
                {
                    $$.val = $1.val;
                }
                else{
                   //printf("factor char\n");
                   // $$.valC = $1.valC;
                }
            };

allStmts:Reference EQUAL Expr SEMI_COLON{
    // printf("%d\n", $1.target);
     sym_entry* entry = search($1.name);
     if(entry == NULL){
        yyerror("The variable does not be declared yet.");
     }
    
     if(entry -> type != $3.type){
         printf("Type does not match.\n");
         yyerror("Type Error");
     }
     if(entry -> dim == 0){
         if($3.type == TYPE_INT && $3.target == -1) {
             emit(-1, loadI, $3.val, $1.target, 0);
         }
         else if($3.type == TYPE_INT) {
             emit(-1, i2i, $3.target, $1.target, 0);
         }
         else if($3.type == TYPE_CHAR){
           printf("assigning CHAR\n"); 
            emit(-1, i2i, $3.target, $1.target, -1);
         }
     }
     else{
         //printf("Ref dim:%d\n", entry -> dim);
         if($3.target == -1){
             int new_reg = nextReg();
             if($$.type == TYPE_INT){
                 emit(-1, loadI, $3.val, new_reg, -1);
                 emit(-1, store, new_reg, $1.target_addr, 0);
            }
             else{
                 emit(-1, cstore, new_reg, $1.target_addr, 0);
             }

         }
         else{
             if($$.type == TYPE_INT)
                 emit(-1, store, $3.target, $1.target_addr, -1);
            else{

                 emit(-1, cstore, $3.target, $1.target_addr, -1);
             }
         }
     }
    }
    |OPEN_CB Stmts CLOSE_CB
     | WHILE{
         int LB = nextLabel();
         $1.label[0] = LB;
         emit(LB, br, -1, -1, -1);
         emit(LB, nop, -1, -1, -1);
     } OPEN_ROUND_BRACKETS Bool {
         int L0 = nextLabel();
         int L1 = nextLabel();

         $4.label[0] = L0;
         $4.label[1] = L1;
         emit_cbr($4.label[0], $4.label[1], $4.target);
         emit(L0, nop, -1, -1, -1);
      }

     CLOSE_ROUND_BRACKETS OPEN_CB Stmts CLOSE_CB{
         emit($1.label[0], br, -1, -1, -1);
         emit($4.label[1], nop, -1, -1, -1);
     }

     | READ Reference SEMI_COLON {
         sym_entry* entry = search($2.name);
         if(entry == NULL){
             yyerror("The variable does not be declared yet.");
         }
         else{
             if(entry -> dim == 0){
                 if($2.type == TYPE_INT){
                    emit(-1, read, $2.target, -1, -1);
                 }
                 else{
                  emit(-1, cread, $2.target, -1, -1);  
                 }
             }
             else{
                 int new_reg = nextReg();
                 if($2.type == TYPE_INT){
                    emit(-1, read, new_reg, -1, -1);
                    emit(-1, store, new_reg, $2.target_addr, -1);
                 }
                 else if($2.type == TYPE_CHAR){
                     emit(-1, cread, new_reg, -1, -1);
                     emit(-1, cstore, new_reg, $2.target_addr, -1);
                 }
                 else{
                     yyerror("Unkonw type to read.");
                 }
             }
         }

     }

     | WRITE Expr SEMI_COLON {

             if($2.type == TYPE_INT && $2.target == -1){
                 int new_reg = nextReg();
                 emit(-1, loadI, $2.val, new_reg, -1);
                 $2.target = new_reg;
             }
             if($2.type == TYPE_INT){
                 emit(-1, write, $2.target, -1, -1);
             }
             else{
             
                //printf("writing char r%d\n", $2.target);
                 emit(-1, cwrite, $2.target, -1, -1);
             }
     }
     |FOR NAME EQUAL Expr{
         sym_entry* entry = search($2.c);
         if(entry == NULL){
             int new_reg = nextReg();
             if($4.type == TYPE_INT){
                  emit(-1, loadI, $4.val, new_reg, -1);
              }
              else{
                  yyerror("For loop cannot use char as start point.");
              }
              $1.target = new_reg;
         }
         else{
             if(entry -> dim != 0 || entry -> type != TYPE_INT){
                yyerror("Array variable or Char referenced as a scalar");
             }
             $1.target = entry -> reg;

             if($4.target != -1){
                 if($4.type == TYPE_INT){
                     emit(-1, i2i, $4.target, $1.target, -1);
                 }
                 else{
                     yyerror("For loop cannot use char as lower bound.");
                  }
             }
             else{
                 if($4.type == TYPE_INT){
                     emit(-1, loadI, $4.val, $1.target, -1);
                 }
                 else{
                     yyerror("For loop cannot use char as upper bound.");
                 }
             }
         }
     } TO Expr {
         int L1 = nextLabel();
         int L2 = nextLabel();
         int L3 = nextLabel();

         int new_reg_con = nextReg();

         if($7.type == TYPE_INT && $7.target == -1){
             int new_reg = nextReg();
             $7.target = new_reg;
             emit(-1, loadI, $7.val, $7.target, -1);
         }
         else if($7.type == TYPE_CHAR){
             yyerror("For loop cannot use char as lower bound.");
         }
         $7.label[0] = L1;
         $7.label[1] = L2;
         $7.label[2] = L3;
         //printf("%d %d %d \n", L1, L2, L3);
         emit(L1, br, -1, -1, -1);
         emit(L1, nop, -1, -1, -1);
         emit(-1, cmp_LE, $1.target, $7.target, new_reg_con);
         emit_cbr( L2, L3, new_reg_con);
         emit(L2, nop, -1, -1, -1);

     } BY Expr OPEN_CB Stmts CLOSE_CB {
         if($10.target != -1){
             emit(-1, add, $1.target, $10.target, $1.target);
         }
         else{
             emit(-1, addI, $1.target, $10.val, $1.target);
         }
         emit($7.label[0], br, -1, -1, -1);
         emit($7.label[2], nop, -1, -1, -1);
     };

Stmt: /*Reference EQUAL Expr SEMI_COLON{
   // printf("%d\n", $1.target);
    sym_entry* entry = search($1.name);
    if(entry -> type != $3.type){
        printf("Type does not match.\n");
        yyerror("Type Error");
    }
    if(entry -> dim == 0){
        if($3.type == TYPE_INT && $3.target == -1) {
            emit(-1, loadI, $3.val, $1.target, 0);
        }
        else if($3.type == TYPE_INT) {
            emit(-1, i2i, $3.target, $1.target, 0);
        }
    }
    else{
        printf("Ref dim:%d\n", entry -> dim);
        if($3.target == -1){
            int new_reg = nextReg();
            if($$.type == TYPE_INT){
                emit(-1, loadI, $3.val, new_reg, -1);
                emit(-1, store, new_reg, $1.target_addr, 0);
           }
            else{
                emit(-1, cstore, new_reg, $1.target_addr, 0);
            }
                
        }
        else{
            if($$.type == TYPE_INT)
                emit(-1, store, $3.target, $1.target_addr, -1);
           else{
                
                emit(-1, cstore, $3.target, $1.target_addr, -1);
            }
        }   
    }
}
    | OPEN_CB Stmts CLOSE_CB{
        
    }
    | WHILE{ 
        int LB = nextLabel();
        $1.label[0] = LB;
        emit(LB, br, -1, -1, -1);
        emit(LB, nop, -1, -1, -1);    
    } OPEN_ROUND_BRACKETS Bool {
        int L0 = nextLabel();
        int L1 = nextLabel();

        $4.label[0] = L0;
        $4.label[1] = L1;
        emit_cbr($4.label[0], $4.label[1], $4.target);
        emit(L0, nop, -1, -1, -1);
     } 
    
    CLOSE_ROUND_BRACKETS OPEN_CB Stmts CLOSE_CB{ 
        emit($1.label[0], br, -1, -1, -1);
        emit($4.label[1], nop, -1, -1, -1);
    }

    | READ Reference SEMI_COLON {
        sym_entry* entry = search($2.name);
        if(entry == NULL){
            yyerror("The variable does not be declared yet.");
        }
        else{
            if(entry -> dim == 0){
                emit(-1, read, $2.target, -1, -1);
            }
            else{
                int new_reg = nextReg();
                emit(-1, read, new_reg, -1, -1);
                if($2.type == TYPE_INT){
                    emit(-1, store, new_reg, $2.target_addr, -1);
                }
                else if($2.type == TYPE_CHAR){
                    emit(-1, cstore, new_reg, $2.target_addr, -1);
                }
                else{
                    yyerror("Unkonw type to read.");
                }
            }
        }

    }
    
    | WRITE Expr SEMI_COLON {

            if($2.target == -1){
                int new_reg = nextReg();   
                emit(-1, loadI, $2.val, new_reg, -1);
                $2.target = new_reg;
            }
            if($2.type == TYPE_INT){
                emit(-1, write, $2.target, -1, -1);
            }
            else{
                emit(-1, cwrite, $2.target, -1, -1);
            }  
    }
    | FOR NAME EQUAL Expr{
        sym_entry* entry = search($2.c);
        if(entry == NULL){
            int new_reg = nextReg();
            if($4.type == TYPE_INT){
                 emit(-1, loadI, $4.val, new_reg, -1);
             }
             else{
                 yyerror("For loop cannot use char as start point.");
             }
             $1.target = new_reg;
        }
        else{
            $1.target = entry -> reg;

            if($4.target != -1){
                if($4.type == TYPE_INT){
                    emit(-1, i2i, $4.target, $1.target, -1);
                }
                else{
                    yyerror("For loop cannot use char as lower bound.");
                 }
            }
            else{
                if($4.type == TYPE_INT){
                    emit(-1, loadI, $4.val, $1.target, -1);
                }
                else{
                    yyerror("For loop cannot use char as upper bound.");
                }
            }
        }
    } TO Expr {
        int L1 = nextLabel();
        int L2 = nextLabel();
        int L3 = nextLabel();

        int new_reg_con = nextReg();
        
        if($7.type == TYPE_INT && $7.target == -1){
            int new_reg = nextReg();
            $7.target = new_reg;
            emit(-1, loadI, $7.val, $7.target, -1);
        }
        else if($7.type == TYPE_CHAR){
            yyerror("For loop cannot use char as lower bound.");
        }
        $7.label[0] = L1;
        $7.label[1] = L2;
        $7.label[2] = L3;
        printf("%d %d %d \n", L1, L2, L3);
        emit(L1, br, -1, -1, -1);
        emit(L1, nop, -1, -1, -1);
        emit(-1, cmp_LE, $1.target, $7.target, new_reg_con);
        emit_cbr( L2, L3, new_reg_con);
        emit(L2, nop, -1, -1, -1);
        
    } BY Expr OPEN_CB Stmts CLOSE_CB {
        if($10.target != -1){
            emit(-1, add, $1.target, $10.target, $1.target);
        }
        else{
            emit(-1, addI, $1.target, $10.val, $1.target);
        }
        emit($7.label[0], br, -1, -1, -1);
        emit($7.label[2], nop, -1, -1, -1);
    }*/
    allStmts
    | IfPre Stmt {
         emit($1.label[1], nop, -1, -1, -1); 
         //printf("this is if.\n");
        }
    | ElsePre ELSE Stmt {     
         emit($1.label[2], br, -1, -1, -1); 
         emit($1.label[2], nop, -1, -1, -1);
        // printf("this is if else.\n");
         }
    | OPEN_CB CLOSE_CB {yyerror("Empty Statement");}
    | OPEN_CB SEMI_COLON CLOSE_CB {yyerror("Empty Statement");}
    | error SEMI_COLON{yyerrok;};

IfPre: IF OPEN_ROUND_BRACKETS Bool CLOSE_ROUND_BRACKETS THEN {
        int L1 = nextLabel();
        int L2 = nextLabel();
        int L3 = nextLabel();
        
        emit_cbr( L1, L2, $3.target);
        emit(L1, nop, -1, -1, -1);
        
        $$.label[0] = L1;
        $$.label[1] = L2;
        $$.label[2] = L3;
     }

ElsePre: IfPre WithElse{
            $$ = $1;
            emit($$.label[2], br, -1, -1, -1);
            emit($$.label[1], nop, -1, -1, -1);
       };

WithElse: ElsePre ELSE WithElse {
            $$ = $1;
            emit($1.label[2], br, -1, -1, -1);
            emit($1.label[2], nop, -1, -1, -1);
        }
        | allStmts
       /* | Reference EQUAL Expr SEMI_COLON {}
    | OPEN_CB Stmts CLOSE_CB {}
    | WHILE OPEN_ROUND_BRACKETS Bool CLOSE_ROUND_BRACKETS OPEN_CB Stmts CLOSE_CB {}
        | READ Reference SEMI_COLON {emit(-1, read, $2.target, -1, -1);}
        | WRITE Expr SEMI_COLON {
            printf("if else, writing\n");
            if($2.target == -1){
                 int new_reg = nextReg();
                 emit(-1, loadI, $2.val, new_reg, -1);
                 $2.target = new_reg;
             }
             if($2.type == TYPE_INT){
                 emit(-1, write, $2.target, -1, -1);
             }
             else{
                 emit(-1, cwrite, $2.target, -1, -1);
             }
        }
        | FOR NAME EQUAL Expr TO Expr BY Expr OPEN_CB Stmts CLOSE_CB{}*/
    ;

RelExpr: Expr {
            $$.type = $1.type;
            if($1.target != -1){
                $$.target = $1.target;
            }
            else{
                int new_reg = nextReg();
                if($1.type == TYPE_INT){
                    emit(-1, loadI, $1.val, new_reg, -1);
                }
                else{
                    //emit(-1, loadI, $1.valC[0], new_reg, -1);
                }
                $$.target = new_reg;
            }
       }
       | RelExpr LT Expr {
            int new_reg = nextReg();
            if($3.target != -1){
                emit(-1, cmp_LT, $1.target, $3.target, new_reg);
            }
            else{
                int new_reg_tmp = nextReg();
                emit(-1, loadI, $3.val, new_reg_tmp, -1);
                emit(-1, cmp_LT, $1.target, new_reg_tmp, new_reg);
            }
            $$.target = new_reg;
       } 
       | RelExpr LE Expr {
            int new_reg = nextReg();
             if($3.target != -1){
                emit(-1, cmp_LE, $1.target, $3.target, new_reg);
             }
             else{
                int new_reg_tmp = nextReg();
                emit(-1, loadI, $3.val, new_reg_tmp, -1);
                emit(-1, cmp_LE, $1.target, new_reg_tmp, new_reg);
             }
             $$.target = new_reg;
       }
       | RelExpr EQ Expr {
              int new_reg = nextReg();
              if($3.target != -1){
                 emit(-1, cmp_EQ, $1.target, $3.target, new_reg);
              }
              else{
                 int new_reg_tmp = nextReg();
                 emit(-1, loadI, $3.val, new_reg_tmp, -1);
                 emit(-1, cmp_EQ, $1.target, new_reg_tmp, new_reg);
              }
              $$.target = new_reg;
       }
       | RelExpr NE Expr {
              int new_reg = nextReg();
              if($3.target != -1){
                 emit(-1, cmp_NE, $1.target, $3.target, new_reg);
              }
              else{
                 int new_reg_tmp = nextReg();
                 emit(-1, loadI, $3.val, new_reg_tmp, -1);
                 emit(-1, cmp_NE, $1.target, new_reg_tmp, new_reg);
              }
              $$.target = new_reg;
       }
       | RelExpr GE Expr {
              int new_reg = nextReg();
              if($3.target != -1){
                 emit(-1, cmp_GE, $1.target, $3.target, new_reg);
              }
              else{
                 int new_reg_tmp = nextReg();
                 emit(-1, loadI, $3.val, new_reg_tmp, -1);
                 emit(-1, cmp_GE, $1.target, new_reg_tmp, new_reg);
              }
              $$.target = new_reg;
       }
       | RelExpr GT Expr {
              int new_reg = nextReg();
              if($3.target != -1){
                 emit(-1, cmp_GT, $1.target, $3.target, new_reg);
              }
              else{
                 int new_reg_tmp = nextReg();
                 emit(-1, loadI, $3.val, new_reg_tmp, -1);
                 emit(-1, cmp_GT, $1.target, new_reg_tmp, new_reg);
              }
              $$.target = new_reg;
       };

Bool: NOT OrTerm { 
        $$.target = nextReg();
        emit(-1, not, $2.target, $$.target, -1);
    } 
    | OrTerm { 
        $$ = $1; 
    };


OrTerm: OrTerm OR OrTerm { 
        $$.target = nextReg();
        emit(-1, or, $1.target, $3.target, $$.target);  
      } 
      | AndTerm { $$ = $1; };

AndTerm: AndTerm AND RelExpr{ $$.target = nextReg();
         emit(-1, and, $1.target, $3.target, $$.target); } 
       | RelExpr{ $$ = $1; }; 

Factor: OPEN_ROUND_BRACKETS Expr CLOSE_ROUND_BRACKETS 
      | Reference { 
            $$.target = $1.target;
            $$.name = $1.name;
            $$.type = $1.type;
        }
      | NUMBER { 
      //int new_reg = nextReg(); 
        //$$.target = new_reg; 
        $$.type = TYPE_INT;
        //printf("%d", $1.num);
        //emit(-1, loadI, $1.num, new_reg, 0); 
        $$.val = $1.num;
        $$.target = -1;
        } | CHARCONST
            {
                int new_reg = nextReg();
                $$.target = new_reg;
                //printf("%d\n", $1.c[1]);
                char* ch = $1.c;
                ch++;
                ch[strlen(ch) - 1] = '\0';
                //printf("%d\n", strlen(ch));
                //printf("%s\n", ch);
                if(strlen(ch) == 2){
                    //printf("%s\n", ch);
                    if(ch[0] == 92 && ch[1] == 'n'){
                        //printf("change a new line.\n");
                        emit(-1, loadI, '\n', new_reg, 0);
                    }
                    else if(ch[0] == 92 && ch[1] == 't'){
                        emit(-1, loadI, '\t', new_reg, 0);
                    }
                    else{
                        //printf("Char can not have invalid pattern\n");
                        yyerror("Char can not have invalid pattern");
                    }
                }                
                else if(strlen(ch) == 1)
                {
                    //printf("loading char\n");
                    emit(-1, loadI, ch[0], new_reg, 0);
                }
                else{
                    //printf("Char can not have invalid pattern\n");
                    yyerror("Char can not have invalid pattern.");
                }
                $$.type = TYPE_CHAR;
                $$.valC = ch;
            }
        | ERRORT {yyerror("char just expects to have one character.");};

Spec: NAME {
            //int offset = getOffset(1);
            //insert($1.c, $$.type, offset);
            $$.name = $1.c;
            $$.dim = 0;
            }
    | NAME OPEN_SQUARE_BRACKETS Bounds CLOSE_SQUARE_BRACKETS{$$.dim = $3.dim; $$.name = $1.c; $$.array_start[0] = $3.array_start[0]; $$.array_end[0] = $3.array_end[0]; $$.array_start[1] = $3.array_start[1]; $$.array_end[1] = $3.array_end[1];};

Bound: NUMBER COLON NUMBER {
     //printf("Into bound\n");
     $$.lhs = $1.num;
     $$.rhs = $3.num;
     //printf("%d:%d\n", $1.num, $3.num);
     dim += 1;
     }
Bounds: Bounds COMMA Bound {
      //printf("get second bound:\n");
      //printf("%d:%d\n", $3.lhs, $3.rhs);
      $$.dim = $1.dim + 1;
      if(dim <= 2){
        $$.array_start[( dim - 1 )  % 2] = $3.lhs;
        $$.array_end[( dim - 1 )  % 2] = $3.rhs;
      }
      //printf("CertainDim:%d, %d:%d, %d:%d \n", dim, $$.array_start[0], $$.array_end[0], $$.array_start[1], $$.array_end[1]);
      }
    | Bound{
            //printf("%d:%d\n", $1.lhs, $1.rhs);
            $$.dim = 1;
            if (dim <= 2){
                $$.array_start[( dim - 1 )  % 2] = $1.lhs;
                $$.array_end[( dim - 1 )  % 2] = $1.rhs;
            }
            //printf("CertainDim:%d\n", dim);
        };

SpecList:SpecList COMMA Spec {  
                                int new_reg = nextReg();     
                                //printf("%d:%d:%d\n", $3.dim, $3.array_start[0], $3.array_end[0]);
                                $$.vars[ numVar ] = strdup($3.name);
                                //printf("speclist: %d:%d:%d\n", $3.dim, $3.array_start[0], $3.array_end[0]);
                                int size = 4;
                                if (curType == 2)
                                    size = 1;
                                if($3.dim == 0){
                                    //int offset = getOffset(1);
                                    // printf("SPECLIST: %s\n", $$.vars[numVar]);
                                    insert($3.name, $3.type, -1, new_reg);
                               }

                                else if($3.dim == 1){
                                    int base = getBase(-$3.array_start[0], $3.array_end[0] - $3.array_start[0] + 1, size);
                                    insert($3.name, $3.type, base, new_reg);
                                    sym_entry* entry = search($3.name);
                                    entry -> dim = 1;
                                    //printf("base: %d\n", base);
                                    //printf("%d:%d\n", $3.array_start[0], $3.array_end[0]);
                                }
                                else{ // now it just supports 2-dimension.
                                    int base = getBase4Two($3.array_start[0], $3.array_end[0], $3.array_start[1], $3.array_end[1], size);
                                    insert($3.name, $3.type, base, new_reg);
                                    sym_entry* entry = search($3.name);
                                    entry -> dim = 2;
                                    entry -> array_start[0] = $3.array_start[0];
                                    entry -> array_end[0] = $3.array_end[0];
                                    entry -> array_start[1] = $3.array_start[1];
                                    entry -> array_end[1] = $3.array_end[1];
                                }
                                dim = 0;
                                numVar += 1;
                            }
        |Spec{
            dim = 0;
            //printf("dim:%d\n", $1.dim);
            int new_reg = nextReg();
            $$.dim = $1.dim;
            //printf("spec: %d:%d:%d\n", $1.dim, $1.array_start[0], $1.array_end[0]);
            $$.vars[numVar] = strdup($1.name);
            int size = 4;
            if (curType == 2)
                size = 1;
            if($$.dim == 0){
                //int offset = getOffset(1);
                // printf("SPEC: %s\n", $1.name);
                insert($1.name, $$.type, -1, new_reg);
             }

             else if($$.dim == 1){
                //printf("in\n");
                int base = getBase(-1*$1.array_start[0], $1.array_end[0] - $1.array_start[0] + 1, size);
                //printf("base: %d\n", base);
                //printf("spec: %d:%d\n", $1.array_start[0], $1.array_end[0]);
                insert($1.name, $1.type, base, new_reg);
                sym_entry* entry = search($1.name);
                entry -> dim = 1;
                entry -> array_start[0] = $1.array_start[0];
                entry -> array_end[0] = $1.array_end[0];

             }
             
             else{
                int base = getBase4Two($1.array_start[0], $1.array_end[0], $1.array_start[1], $1.array_end[1], size);
                //printf("base: %d spec: %d, %d:%d, %d:%d\n", base, $1.dim, $1.array_start[0], $1.array_end[0], $1.array_start[1], $1.array_end[1]);
                insert($1.name, $1.type, base, new_reg);
                sym_entry* entry = search($1.name);
                entry -> dim = 2;
                entry -> array_start[0] = $1.array_start[0];
                entry -> array_end[0] = $1.array_end[0];
                entry -> array_start[1] = $1.array_start[1];
                entry -> array_end[1] = $1.array_end[1];
             }
             numVar += 1;
             };

Decl: Type SpecList {
                        $2.type = $1.type;
                        //printf("%d %s\n", $2.type, $2.name);
                        int i;
                        for(i = 0; i < numVar; ++i){
                            //printf("\n%s\n", $2.vars[i]);
                            sym_entry* entry = search($2.vars[i]);
                            //printf("\n%s\n", entry->name);
                            entry->type = $1.type;
                       }
                    curType = -1;
                    numVar = 0;
}; 

Decls: Decl SEMI_COLON 
     | Decls Decl SEMI_COLON
    | error SEMI_COLON{yyerrok;};	

Stmts: Stmt
     | Stmts Stmt;

Type: INT { $$.type = TYPE_INT; curType = 1;} |CHAR { $$.type = TYPE_CHAR; curType = 2;};	

Procedure: PROCEDURE NAME OPEN_CB Decls Stmts CLOSE_CB;


%%

int yyerror(const char*s)
{
    errorNum += 1;
    printf("Line %d: %s\n", lineNum, s);
}

int main(int argc, char* argv[])
{
    extern FILE *yyin;
    char* type;
    char* filename = (char *)malloc(456);

    
    if (argc == 2) {
        if (!strcmp("-h", argv[1])) {
            printf("-h : ./demo [file name] will execute the compile with that file.\n");
            printf("The compile result will present at <basename>.i file.\n");
            printf("If you do not add file name after the compiler, it will accept stdin instead.\n");
            printf("If you provide input through stdin, the parser will generate \"iloc.i\" file instead.\n");
            return 0;
        } else {
            
            filename = strdup(argv[1]);
            type = strstr(filename, ".demo");
            if(type == NULL){
               printf("Error: The file type is not .demo. It is invalid.\n");
               return 0;
             }
           
           type++;
            *type = '\0';
            strcat(filename, "i");
            //strncpy(type, ".i", 2);
            //printf("%s\n", filename);
            FILE *input;
            input=fopen(argv[1], "r");
            //char* filename = (char *)malloc(456);
            //filename = strdup(argv[1]);
            //strcat(filename, ".i");
            outfile = fopen(filename, "w");
            yyin = input;
        }
    } else {
        outfile = fopen("iloc.i", "w");
        yyin = stdin;
    }

    

    newHashTable();
    //insert("i", TYPE_INT, 4);
    //insert("j", TYPE_INT, 8);
    // printTable();
    errorNum = 0;
    yyparse();
    if (errorNum == 0){
        printf("parse successfully\n");
    }
    else{
        printf("parse fail and total errors: %d\n", errorNum);
    }
    printTable();
    return 0;
}
