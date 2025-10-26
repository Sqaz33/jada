/* 
  file: grammar.y
  description: Ada language grammar
  authors: Ryzhkov, Matveev
  Volgograd 2025 
*/

%language "c++"

%skeleton "lalr1.cc"

%param {FlexLexer* lexer}
%define api.value.type variant
%define parse.trace  

%code requires
{
  #include <string>
  #include <utility>

  class FlexLexer; 
}

%code 
{
  #include <FlexLexer.h>

  namespace yy {
      parser::token_type yylex(parser::semantic_type* yylval, FlexLexer* lexer); 
  }
}

%defines

%token SC COLON COMMA DOT DOT_DOT LPAR RPAR APOSTR
%right ASG
%token IF THEN ELSE ELSIF WHEN
%token FOR LOOP WHILE EXIT IN OUT
%token PROCEDURE FUNCTION RETURN IS BEGIN_KW END OVERRIDING NEW
%token PACKAGE BODY PRIVATE WITH USE
%token ARRAY OF TYPE TAGGED RECORD
%token INTEGERTY STRINGTY CHARACTERTY FLOATTY BOOLTY
%token FALSE TRUE NULL_KW
%token<std::string> NAME
%token<int> INTEGER
%token<float> FLOAT
%token<char> CHAR
%token<std::string> STRING
%token<std::pair<std::string, std::string>> GETTING_ATTRIBUTE
%left INPUT
%left OR AND NOT
%left EQ NEQ MORE LESS GTE LTE
%left PLUS MINUS AMPER
%left MUL DIV MOD

%token ERR

%nonassoc UMINUS

%nterm stm            
%nterm decl_area
%nterm decl
%nterm var_decl
%nterm import_decl
%nterm use_decl
%nterm qualified_name
%nterm proc_decl
%nterm func_decl
%nterm pack_decl
%nterm type_decl
%nterm record_decl
%nterm vars_decl
%nterm param
%nterm param_list
%nterm scalar_type
%nterm any_type
%nterm string_type
%nterm array_type
%nterm array_range
%nterm static_ranges
%nterm static_range
%nterm body
%nterm stms
%nterm oper
%nterm assign
%nterm lval
%nterm expr
%nterm call_or_indexing
%nterm args
%nterm literal
%nterm literals
%nterm aggregate
%nterm if_stm
%nterm if_head
%nterm elsifs
%nterm elsif
%nterm else
%nterm for_stm
%nterm while_stm
%nterm range
%nterm call_or_indexing_stm
%nterm optional_args

%start program

%%

program: decl_area 

/* declarations */
/* ################################################################################ */
decl_area:        decl
                | decl_area decl

decl:             var_decl
                | import_decl
                | use_decl 
                | proc_decl  
                | func_decl  
                | pack_decl  
                | type_decl  

var_decl:         NAME COLON any_type ASG expr SC 
                | NAME COLON any_type ASG aggregate SC 
                | NAME COLON any_type SC                                               

import_decl:      WITH qualified_name SC

use_decl:         USE qualified_name SC

qualified_name:   NAME
                | qualified_name DOT NAME

proc_decl:        PROCEDURE NAME IS optional_decl_area BEGIN_KW body END NAME SC
                | PROCEDURE NAME LPAR param_list RPAR IS optional_decl_area BEGIN_KW body END NAME SC
                | OVERRIDING proc_decl

func_decl:        FUNCTION NAME RETURN any_type IS optional_decl_area BEGIN_KW body END NAME SC                 
                | FUNCTION NAME LPAR param_list RPAR RETURN any_type IS optional_decl_area BEGIN_KW body END NAME SC
                | OVERRIDING func_decl 

pack_decl:        PACKAGE NAME IS decl_area END NAME SC         
                | PACKAGE NAME IS decl_area PRIVATE decl_area END NAME SC         

type_decl:        record_decl
                  /* enum_decl */ /* TODO */

record_decl:      TYPE NAME IS RECORD vars_decl END RECORD SC 
                | TYPE NAME IS NEW qualified_name WITH RECORD vars_decl END RECORD SC

vars_decl:        var_decl
                | vars_decl var_decl

param_list:       param
                | param_list SC param

param:            NAME COLON any_type
                | NAME COLON IN OUT any_type
                | NAME COLON IN any_type
                | NAME COLON OUT any_type

optional_decl_area: %empty
                |   decl_area

/* types */
/* ################################################################################ */
any_type:         scalar_type
                | array_type

scalar_type:      INTEGERTY 
                | FLOATTY
                | CHARACTERTY
                | string_type  
                | qualified_name
                | getting_attribute

string_type:      STRINGTY LPAR static_range RPAR

array_type:       ARRAY array_range OF scalar_type

array_range:      LPAR static_ranges RPAR

static_ranges:    static_range
                | static_range COMMA static_range

static_range:     INTEGER DOT_DOT INTEGER

/* statements */
/* ################################################################################ */
body:             stms

stms:             stm
                | stms stm

stm:              oper
                | if_stm
                | while_stm
                | for_stm
                | return_stm

oper:             assign
                | call_or_indexing_stm

call_or_indexing_stm:  call_or_indexing SC

assign:           lval ASG expr SC            
                                                
lval:             qualified_name       
                | call_or_indexing               

return_stm:       RETURN expr SC
                | RETURN SC 

expr:             expr EQ expr
                | expr NEQ expr                                 
                | expr MORE expr
                | expr LESS expr
                | expr GTE expr
                | expr LTE expr
                | expr AMPER expr 
                | expr PLUS expr              
                | expr MINUS expr             
                | expr MUL expr               
                | expr DIV expr               
                | expr MOD expr               
                | MINUS expr %prec UMINUS     
                | qualified_name
                | call_or_indexing
                | literal

call_or_indexing:   qualified_name LPAR optional_args RPAR
                |   getting_attribute LPAR optional_args RPAR

optional_args:    %empty
                | args

args:             expr
                | aggregate
                | args COMMA expr
                | args COMMA aggregate

getting_attribute:   qualified_name DOT GETTING_ATTRIBUTE
                |    GETTING_ATTRIBUTE

literal:          INTEGER
                | FALSE
                | TRUE
                | CHAR
                | STRING

aggregate:        LPAR inits RPAR
                | LPAR literals RPAR 

inits:            init
                | inits init

init:             NAME EQ MORE literal
                | INTEGER EQ MORE literal 
                | NAME EQ MORE aggregate
                | INTEGER EQ MORE aggregate 

literals:         literal  COMMA literal
                | literals COMMA literal
                | aggregate COMMA aggregate
                | literals COMMA aggregate


/* control structures */
/* ################################################################################ */
if_stm:          if_head body END IF SC
               | if_head body elsifs END IF SC
               | if_head body elsifs else END IF SC
               | if_head body else END IF SC

elsifs:          elsif         
               | elsifs elsif  

elsif:           ELSIF expr THEN body

else:            ELSE body

if_head:         IF expr THEN 

for_stm:         FOR NAME IN range LOOP body END LOOP SC

range:           expr DOT_DOT expr

while_stm:       WHILE expr LOOP body END LOOP SC

%%

#include "helper.hpp"

namespace yy {
  parser::token_type yylex(parser::semantic_type* val, FlexLexer* lexer) {
    helper::yylval = val; 
    auto tt = static_cast<parser::token_type>(lexer->yylex());
    return tt;
  }

  void parser::error(const std::string& s) {
    std::cerr << "Error: " << s << std::endl; // TODO
  }
}
