/* 
  file: grammar.y
  description: Ada language grammar
  authors: Ryzhkov, Matveev
  Volgograd 2025 
*/

%language "c++"

%skeleton "lalr1.cc"

%locations
%param {FlexLexer* lexer}
%define api.value.type variant
%define parse.trace  

%code requires
{
  #include <string>
  #include <utility>
  #include <sstream>
  
  #include "location.hh"

  #include "node.hpp"

  class FlexLexer; 
}

%code 
{
  #include <FlexLexer.h>

  namespace yy {
      parser::token_type yylex(parser::semantic_type* yylval, 
                                yy::parser::location_type*,  
                                FlexLexer* lexer); 
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
%token<bool> BOOL NULL_KW
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
%nterm<attribute::QualifiedName> qualified_name
%nterm<attribute::Attribute> getting_attribute
%nterm proc_decl
%nterm func_decl
%nterm pack_decl
%nterm type_decl
%nterm record_decl
%nterm vars_decl
%nterm param
%nterm param_list
%nterm string_type
%nterm array_type
%nterm type
%nterm array_range
%nterm static_ranges
%nterm static_range
%nterm body
%nterm stms
%nterm oper
%nterm assign
%nterm lval
%nterm<node::IExpr*> expr
%nterm call_or_indexing_or_var  
%nterm<std::vector<node::IExpr*>> args
%nterm<std::vector<node::IExpr*>> optional_args
%nterm<node::ILiteral*> literal
%nterm<std::vector<node::ILiteral*>> literals
%nterm<node::ILiteral*> aggregate
%nterm<std::pair<std::pair<int, std::string>, node::ILiteral*>> init
%nterm<std::vector<std::pair<std::pair<int, std::string>, node::ILiteral*>>> inits
%nterm<node::IStm*> if_stm
%nterm<node::IExpr*> if_head
%nterm<std::vector<std::pair<node::IExpr*, node::Body*>>> elsifs
%nterm<std::pair<node::IExpr*, node::Body*>> elsif 
%nterm<node::Body*> else
%nterm<node::IStm*> for_stm
%nterm<node::IStm*> while_stm
%nterm<std::pair<node::IExpr*, node::IExpr*>> range
%nterm<node::IStm*> call_or_indexing_or_var_stm
%nterm<node::IExpr*> call_or_indexing_or_var

%start program

%%

program: optional_imports compile_unit

/* declarations */
/* ################################################################################ */
decl_area:        decl
                | decl_area decl

decl:             var_decl
                | use_decl 
                | proc_decl  
                | func_decl  
                | pack_decl  
                | type_decl

var_decl:         NAME COLON type ASG expr SC 
                | NAME COLON type ASG aggregate SC 
                | NAME COLON type SC                                               

import_decl:      WITH qualified_name SC

use_decl:         USE qualified_name SC

optional_imports: import_decl
                | use_decl
                | optional_imports import_decl
                | optional_imports use_decl

qualified_name:   NAME
                | qualified_name DOT NAME

proc_decl:        PROCEDURE NAME IS optional_decl_area BEGIN_KW body END NAME SC
                | PROCEDURE NAME LPAR param_list RPAR IS optional_decl_area BEGIN_KW body END NAME SC
                | OVERRIDING proc_decl

func_decl:        FUNCTION NAME RETURN type IS optional_decl_area BEGIN_KW body END NAME SC                 
                | FUNCTION NAME LPAR param_list RPAR RETURN type IS optional_decl_area BEGIN_KW body END NAME SC
                | OVERRIDING func_decl 

pack_decl:        PACKAGE NAME IS decl_area END NAME SC         
                | PACKAGE NAME IS decl_area PRIVATE decl_area END NAME SC         

type_decl:        record_decl
                 | type_alias_decl
                  /* enum_decl */ /* TODO */

record_decl:      TYPE NAME IS RECORD vars_decl END RECORD SC 
                | TYPE NAME IS NEW qualified_name WITH RECORD vars_decl END RECORD SC

vars_decl:        var_decl
                | vars_decl var_decl

param_list:       param
                | param_list SC param

param:            NAME COLON type
                | NAME COLON IN OUT type
                | NAME COLON IN type
                | NAME COLON OUT type

optional_decl_area: %empty
                |   decl_area

type_alias_decl:    TYPE NAME IS type SC

compile_unit:     proc_decl
                | func_decl
                | pack_decl

/* types */
/* ################################################################################ */

type:             INTEGERTY 
                | FLOATTY
                | CHARACTERTY
                | string_type  
                | qualified_name
                | getting_attribute
                | array_type

string_type:      STRINGTY LPAR static_range RPAR

array_type:       ARRAY array_range OF type

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
                | call_or_indexing_or_var_stm

call_or_indexing_or_var_stm:  call_or_indexing_or_var SC

assign:           lval ASG expr SC            
                                                
lval:             call_or_indexing_or_var               

return_stm:       RETURN expr SC
                | RETURN SC                                             

expr:             expr EQ expr                                          { $$ = new node::Op($1, node::OpType::EQ, $3);          }
                | expr NEQ expr                                         { $$ = new node::Op($1, node::OpType::NEQ, $3);         }
                | expr MORE expr                                        { $$ = new node::Op($1, node::OpType::MORE, $3);        }
                | expr LESS expr                                        { $$ = new node::Op($1, node::OpType::LESS, $3);        }
                | expr GTE expr                                         { $$ = new node::Op($1, node::OpType::GTE, $3);         }
                | expr LTE expr                                         { $$ = new node::Op($1, node::OpType::LTE, $3);         }
                | expr AMPER expr                                       { $$ = new node::Op($1, node::OpType::AMPER, $3);       }
                | expr PLUS expr                                        { $$ = new node::Op($1, node::OpType::PLUS, $3);        }
                | expr MINUS expr                                       { $$ = new node::Op($1, node::OpType::MINUS, $3);       }
                | expr MUL expr                                         { $$ = new node::Op($1, node::OpType::MUL, $3);         }
                | expr DIV expr                                         { $$ = new node::Op($1, node::OpType::DIV, $3);         }
                | expr MOD expr                                         { $$ = new node::Op($1, node::OpType::MOD, $3);         }
                | MINUS expr %prec UMINUS                               { $$ = new node::Op(nullptr, node::OpType::UMINUS, $2); }
                | call_or_indexing_or_var                               { $$ = $1;                                              }
                | literal                                               { $$ = $1;                                              }

call_or_indexing_or_var:   qualified_name LPAR optional_args RPAR       { $$ = new node::CallOrIndexingOrVar($1, $3); }
                |          getting_attribute LPAR optional_args RPAR    { $$ = new node::CallOrIndexingOrVar($1, $3); }
                |          qualified_name                               { $$ = new node::CallOrIndexingOrVar($1); }

optional_args:    %empty                                                { $$ = std::vector<node::IExpr*>(); }
                | args                                                  { $$ = std::move($1); }

args:             expr                                                   { $$ = std::vector<node::IExpr*>({$1}); }
                | aggregate                                              { $$ = std::vector<node::IExpr*>({$1}); }
                | args COMMA expr                                        { $$ = std::move($1); $$.push_back($3); }
                | args COMMA aggregate                                   { $$ = std::move($1); $$.push_back($3); }

getting_attribute:   qualified_name DOT GETTING_ATTRIBUTE                 { 
                                                                            $1.push($3.first);
                                                                            $$ = attribute::Attribute($1, $3.second);
                                                                          }
                |    GETTING_ATTRIBUTE                                    {
                                                                            $$ = attribute::Attribute($1.first, $1.second);
                                                                          }

literal:          INTEGER                                                 { 
                                                                            auto* type = new node::SimpleLiteralType(
                                                                                          node::SimpleType::INTEGER);
                                                                            $$ = new node::SimpleLiteral(type, $1);
                                                                          }
                | BOOL                                                    {
                                                                            auto* type = new node::SimpleLiteralType(
                                                                                          node::SimpleType::BOOL);
                                                                            $$ = new node::SimpleLiteral(type, $1);
                                                                          }
                | CHAR                                                    {
                                                                            auto* type = new node::SimpleLiteralType(
                                                                                          node::SimpleType::CHAR);
                                                                            $$ = new node::SimpleLiteral(type, $1);
                                                                          }
                | STRING                                                  {
                                                                            auto* type = new node::StringType(
                                                                                std::make_pair(1, $1.length()));
                                                                            $$ = new node::StringLiteral(type, $1);
                                                                          }
                | FLOAT                                                   {
                                                                            auto* type = new node::SimpleLiteralType(
                                                                                          node::SimpleType::FLOAT);
                                                                            $$ = new node::SimpleLiteral(type, $1);
                                                                          }
                 

aggregate:        LPAR inits RPAR                                         {   
                                                                            auto* aggr = new node::Aggregate();
                                                                            for (auto&& [init, lit] : $2) {
                                                                              if (init.second.empty()) {
                                                                                aggr->addInit(init.first, lit);
                                                                              } else {
                                                                                aggr->addInit(init.second, lit);
                                                                              }
                                                                            }
                                                                            $$ = aggr;
                                                                          }
                | LPAR literals RPAR                                      {
                                                                            auto* aggr = new node::Aggregate();
                                                                            for (auto* lit : $2) {
                                                                              aggr->addInit(lit);
                                                                            }
                                                                            $$ = aggr;
                                                                          }

inits:            init                                                    { 
                                                                            $$ = std::vector<
                                                                                std::pair<std::pair<int, std::string>, 
                                                                                node::ILiteral*>>({$1}); 
                                                                          }
                | inits init                                              { $$ = std::move($1); $$.push_back($2); }

init:             NAME EQ MORE literal                                    { $$ = std::make_pair(std::pair<int, std::string>(0, $1), $4); }
                | INTEGER EQ MORE literal                                 { $$ = std::make_pair(std::pair<int, std::string>($1, ""), $4); }
                | NAME EQ MORE aggregate                                  { $$ = std::make_pair(std::pair<int, std::string>(0, $1), $4); }
                | INTEGER EQ MORE aggregate                               { $$ = std::make_pair(std::pair<int, std::string>($1, ""), $4); }
  
literals:         literal  COMMA literal                                  { $$ = std::vector<node::ILiteral*>({$1, $3}); }
                | literals COMMA literal                                  { $$ = std::move($1); $$.push_back($3); }
                | aggregate COMMA aggregate                               { $$ = std::vector<node::ILiteral*>({$1, $3}); }
                | literals COMMA aggregate                                { $$ = std::move($1); $$.push_back($3); }


/* control structures */
/* ################################################################################ */
if_stm:          if_head body END IF SC
               | if_head body elsifs END IF SC
               | if_head body elsifs else END IF SC
               | if_head body else END IF SC

elsifs:          elsif                                                    { $$ = std::vector($1); }
               | elsifs elsif  

elsif:           ELSIF expr THEN body                                     { $$ = std::make_pair($2, $4); }

else:            ELSE body                                                { $$ = $2; }

if_head:         IF expr THEN                                             { $$ = $2; }

for_stm:         FOR NAME IN range LOOP body END LOOP SC                  { $$ = new node::For($2, $4, $6); }
 
range:           expr DOT_DOT expr                                        { $$ = std::make_pair($1, $3); }

while_stm:       WHILE expr LOOP body END LOOP SC                         { $$ = new node::While($2, $4); }

%%

#include "helper.hpp"

namespace yy {
  parser::token_type yylex(parser::semantic_type* val, 
                            yy::parser::location_type* loc, 
                            FlexLexer* lexer) 
  {
    helper::yylval = val; 
    auto tt = static_cast<parser::token_type>(lexer->yylex());
    loc->initialize(&helper::moduleFileNames.back());
    loc->begin.line = helper::first_line;
    loc->end.line = helper::last_line;
    loc->begin.column = helper::first_column - 1;
    loc->end.column = helper::last_column;
    return tt;
  }

  void parser::error(const yy::parser::location_type& loc, 
                      const std::string& msg) 
  {
    std::stringstream ss;
    ss << loc << ' ' << msg << std::endl;
    helper::errs.push_back(ss.str());
  }
}
