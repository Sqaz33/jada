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
/* %define parse.trace */  /* uncom for trace */ 

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

%nterm<node::IDecl*> decl
%nterm<node::IDecl*> var_decl
%nterm<node::IDecl*> import_decl
%nterm<node::IDecl*> use_decl
%nterm<attribute::QualifiedName> qualified_name
%nterm<attribute::Attribute> getting_attribute
%nterm<node::IDecl*> proc_decl
%nterm<node::IDecl*> func_decl
%nterm<node::IDecl*> pack_decl
%nterm<node::IDecl*> type_decl
%nterm<node::IDecl*> record_decl
%nterm<std::vector<node::VarDecl*>> vars_decl
%nterm<std::pair<node::VarDecl*, node::ParamMode>> param
%nterm<std::vector<std::pair<node::VarDecl*, node::ParamMode>>> param_list
%nterm<node::IType*> string_type
%nterm<node::IType*> array_type
%nterm<node::IType*> type
%nterm<std::vector<std::pair<int, int>>> array_range
%nterm<std::vector<std::pair<int, int>>> static_ranges
%nterm<std::pair<int, int>> static_range
%nterm<node::Body*> body
%nterm<std::vector<node::IStm*>> stms
%nterm<node::IStm*> stm
%nterm<node::IStm*> oper
%nterm<node::IStm*> assign
%nterm<node::IExpr*> lval
%nterm<node::IStm*> return_stm
%nterm<node::IExpr*> expr
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
%nterm<node::IDecl*> compile_unit 
%nterm<node::IDecl*> type_alias_decl
%nterm<node::DeclArea*> optional_decl_area
%nterm<node::DeclArea*> decl_area
%nterm<std::vector<node::IDecl*>> optional_imports

%start program

%%

program: optional_imports compile_unit                                { 
                                                                        node::DeclArea root;
                                                                        for (auto* decl : $1) {
                                                                          root.addDecl(decl);
                                                                        }
                                                                        root.addDecl($2);
                                                                        root.print(0);
                                                                      }

/* declarations */
/* ################################################################################ */
decl_area:        decl                                                { $$ = new node::DeclArea(); $$->addDecl($1); }  
                | decl_area decl                                      { $$ = $1; $$->addDecl($2); }
 
decl:             var_decl
                | use_decl 
                | proc_decl  
                | func_decl  
                | pack_decl  
                | type_decl

var_decl:         NAME COLON type ASG expr SC                           { $$ = new node::VarDecl($1, $3, $5); }
                | NAME COLON type ASG aggregate SC                      { $$ = new node::VarDecl($1, $3, $5); }
                | NAME COLON type SC                                    { $$ = new node::VarDecl($1, $3); }
 
import_decl:      WITH qualified_name SC                                { $$ = new node::WithDecl(std::move($2)); }

use_decl:         USE qualified_name SC                                 { $$ = new node::UseDecl(std::move($2)); }

optional_imports: import_decl                                           { $$ = std::vector({$1}); }
                | use_decl                                              { $$ = std::vector({$1}); }
                | optional_imports import_decl                          { $$ = std::move($1); $$.push_back($2); }
                | optional_imports use_decl                             { $$ = std::move($1); $$.push_back($2); }

qualified_name:   NAME                                                  { $$ = attribute::QualifiedName($1); } 
                | qualified_name DOT NAME                               { $$ = std::move($1); $$.push($3); }       

proc_decl:        PROCEDURE NAME IS optional_decl_area BEGIN_KW body END NAME SC                                     { $$ = new node::ProcDecl($2, {}, $4, $6); }
                | PROCEDURE NAME LPAR param_list RPAR IS optional_decl_area BEGIN_KW body END NAME SC                { $$ = new node::ProcDecl($2, $4, $7, $9); }
                | OVERRIDING proc_decl                                                                               { $$ = $2; }

func_decl:        FUNCTION NAME RETURN type IS optional_decl_area BEGIN_KW body END NAME SC                          { $$ = new node::FuncDecl($2, {}, $4, $6, $8); }
                | FUNCTION NAME LPAR param_list RPAR RETURN type IS optional_decl_area BEGIN_KW body END NAME SC     { $$ = new node::FuncDecl($2, $4, $7, $9, $11); }
                | OVERRIDING func_decl                                                                               { $$ = $2; }

pack_decl:        PACKAGE NAME IS decl_area END NAME SC                                       { $$ = new node::PackDecl($2, $4); }
                | PACKAGE NAME IS decl_area PRIVATE decl_area END NAME SC                     { $$ = new node::PackDecl($2, $4, $6); }

type_decl:        record_decl                    
                 | type_alias_decl
                  /* enum_decl */ /* TODO */

record_decl:      TYPE NAME IS RECORD vars_decl END RECORD SC                                 { $$ = new node::RecordDecl($2, $5); }
                | TYPE NAME IS TAGGED RECORD vars_decl END RECORD SC                          { $$ = new node::RecordDecl($2, $6, {}, true); }
                | TYPE NAME IS NEW qualified_name WITH RECORD vars_decl END RECORD SC         { $$ = new node::RecordDecl($2, std::move($8), $5); }

vars_decl:        var_decl                                              { 
                                                                          auto* decl = dynamic_cast<node::VarDecl*>($1);
                                                                          $$ = std::vector<node::VarDecl*>({decl}); 
                                                                        }
                | vars_decl var_decl                                    { 
                                                                          $$ = std::move($1); 
                                                                          auto* decl = dynamic_cast<node::VarDecl*>($2);
                                                                          $$.push_back(decl); 
                                                                        }

param_list:       param                                                 { $$ = std::vector({$1}); }
                | param_list SC param                                   { $$ = std::move($1); $$.push_back($3); }

param:            NAME COLON type                                       { 
                                                                          auto* decl = new node::VarDecl($1, $3);
                                                                          $$ = std::make_pair(decl, node::ParamMode::IN); 
                                                                        }
                | NAME COLON IN OUT type                                { 
                                                                          auto* decl = new node::VarDecl($1, $5);
                                                                          $$ = std::make_pair(decl, node::ParamMode::IN_OUT); 
                                                                        }
                | NAME COLON IN type                                    { 
                                                                          auto* decl = new node::VarDecl($1, $4);
                                                                          $$ = std::make_pair(decl, node::ParamMode::IN); 
                                                                        }
                | NAME COLON OUT type                                   { 
                                                                          auto* decl = new node::VarDecl($1, $4);
                                                                          $$ = std::make_pair(decl, node::ParamMode::OUT); 
                                                                        }

optional_decl_area: %empty                                              { $$ = nullptr; }
                |   decl_area                                           

type_alias_decl:    TYPE NAME IS type SC                                { $$ = new node::TypeAliasDecl($2, $4); }        

compile_unit:     proc_decl                                             
                | func_decl                 
                | pack_decl 

/* types */
/* ################################################################################ */

type:             INTEGERTY                                              { 
                                                                            $$ = new node::SimpleLiteralType(
                                                                                    node::SimpleType::INTEGER); 
                                                                         }
                | FLOATTY                                                {
                                                                            $$ = new node::SimpleLiteralType(
                                                                                    node::SimpleType::FLOAT); 
                                                                         }
                | CHARACTERTY                                            {
                                                                            $$ = new node::SimpleLiteralType(
                                                                                    node::SimpleType::CHAR); 
                                                                         }
                | string_type                                            
                | qualified_name                                         { $$ = new node::TypeName($1); }
                | getting_attribute                                      { $$ = new node::TypeName($1); }             
                | array_type                                                                

string_type:      STRINGTY LPAR static_range RPAR                        { $$ = new node::StringType($3); }

array_type:       ARRAY array_range OF type                              { $$ = new node::ArrayType(std::move($2), $4); }

array_range:      LPAR static_ranges RPAR                                { $$ = std::move($2); }              

static_ranges:    static_range                                           { $$ = std::vector({$1}); }
                | static_ranges COMMA static_range                       { $$ = std::move($1); $$.push_back($3); }

static_range:     INTEGER DOT_DOT INTEGER                                { $$ = std::make_pair($1, $3); }

/* statements */
/* ################################################################################ */
body:             stms                                                    { $$ = new node::Body(std::move($1)); }

stms:             stm                                                     { $$ = std::vector({$1}); }
                | stms stm                                                { $$ = std::move($1); $$.push_back($2); }

stm:              oper                                                   
                | if_stm                                                 
                | while_stm                                              
                | for_stm                                                
                | return_stm                                             

oper:             assign                                                 
                | call_or_indexing_or_var_stm                            

call_or_indexing_or_var_stm:  call_or_indexing_or_var SC                  { 
                                                                            auto* CIV = dynamic_cast<
                                                                              node::CallOrIndexingOrVar*>($1);
                                                                            $$ = new node::CallOrIndexingOrVarStm(CIV); 
                                                                          }

assign:           lval ASG expr SC                                        { 
                                                                            auto* CIV = dynamic_cast<
                                                                              node::CallOrIndexingOrVar*>($1);
                                                                            $$ = new node::Assign(CIV, $3); 
                                                                          }
                                                
lval:             call_or_indexing_or_var                                 

return_stm:       RETURN expr SC                                          { $$ = new node::Return($2); } 
                | RETURN SC                                               { $$ = new node::Return(); }                                        

expr:             expr EQ expr                                            { $$ = new node::Op($1, node::OpType::EQ, $3);          }
                | expr NEQ expr                                           { $$ = new node::Op($1, node::OpType::NEQ, $3);         }
                | expr MORE expr                                          { $$ = new node::Op($1, node::OpType::MORE, $3);        }
                | expr LESS expr                                          { $$ = new node::Op($1, node::OpType::LESS, $3);        }
                | expr GTE expr                                           { $$ = new node::Op($1, node::OpType::GTE, $3);         }
                | expr LTE expr                                           { $$ = new node::Op($1, node::OpType::LTE, $3);         }
                | expr AMPER expr                                         { $$ = new node::Op($1, node::OpType::AMPER, $3);       }
                | expr PLUS expr                                          { $$ = new node::Op($1, node::OpType::PLUS, $3);        }
                | expr MINUS expr                                         { $$ = new node::Op($1, node::OpType::MINUS, $3);       }
                | expr MUL expr                                           { $$ = new node::Op($1, node::OpType::MUL, $3);         }
                | expr DIV expr                                           { $$ = new node::Op($1, node::OpType::DIV, $3);         }
                | expr MOD expr                                           { $$ = new node::Op($1, node::OpType::MOD, $3);         }
                | MINUS expr %prec UMINUS                                 { $$ = new node::Op(nullptr, node::OpType::UMINUS, $2); }
                | call_or_indexing_or_var                                 
                | literal                                                 { $$ = $1; }

call_or_indexing_or_var:   qualified_name LPAR optional_args RPAR         { $$ = new node::CallOrIndexingOrVar($1, std::move($3)); }
                |          getting_attribute LPAR optional_args RPAR      { $$ = new node::CallOrIndexingOrVar($1, std::move($3)); }
                |          qualified_name                                 { $$ = new node::CallOrIndexingOrVar($1); }

optional_args:    %empty                                                  { $$ = std::vector<node::IExpr*>(); }
                | args                                                    { $$ = std::move($1); }

args:             expr                                                    { $$ = std::vector({$1}); }
                | aggregate                                               { $$ = std::vector<node::IExpr*>({$1}); }
                | args COMMA expr                                         { $$ = std::move($1); $$.push_back($3); }
                | args COMMA aggregate                                    { $$ = std::move($1); $$.push_back($3); }

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

inits:            init                                                    { $$ = std::vector({$1}); }
                | inits COMMA init                                        { $$ = std::move($1); $$.push_back($3); }

init:             NAME EQ MORE literal                                    { $$ = std::make_pair(std::pair<int, std::string>(0, $1), $4); }
                | INTEGER EQ MORE literal                                 { $$ = std::make_pair(std::pair<int, std::string>($1, ""), $4); }
                | NAME EQ MORE aggregate                                  { $$ = std::make_pair(std::pair<int, std::string>(0, $1), $4); }
                | INTEGER EQ MORE aggregate                               { $$ = std::make_pair(std::pair<int, std::string>($1, ""), $4); }
  
literals:         literal  COMMA literal                                  { $$ = std::vector({$1, $3}); }
                | literals COMMA literal                                  { $$ = std::move($1); $$.push_back($3); }
                | aggregate COMMA aggregate                               { $$ = std::vector({$1, $3}); }
                | literals COMMA aggregate                                { $$ = std::move($1); $$.push_back($3); }


/* control structures */
/* ################################################################################ */
if_stm:          if_head body END IF SC                                   { $$ = new node::If($1, $2); }
               | if_head body elsifs END IF SC                            { $$ = new node::If($1, $2, nullptr, std::move($3)); }
               | if_head body elsifs else END IF SC                       { $$ = new node::If($1, $2, $4, std::move($3)); }
               | if_head body else END IF SC                              { $$ = new node::If($1, $2, $3); }

elsifs:          elsif                                                    { $$ = std::vector({$1}); }
               | elsifs elsif                                             { $$ = std::move($1); $$.push_back($2); }
 
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
