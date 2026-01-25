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
/* %define parse.trace */   /* uncom for trace */ 

%code requires
{
  #include <string>
  #include <utility>
  #include <sstream>
  #include <iostream>
  #include <set>
  #include <queue>
  
  #include "location.hh"

  #include "node.hpp"
  #include "graphviz.hpp"
  #include "module.hpp"
  #include "string_utility.hpp"

  class FlexLexer; 

  using OptionalImports = typename 
    std::pair<std::vector<std::shared_ptr<node::With>>, 
                std::vector<std::shared_ptr<node::Use>>>;
                
  using ArgsType = typename std::vector<std::shared_ptr<node::IExpr>>;

  namespace helper {
      extern std::vector<
          std::shared_ptr<mdl::Module>> modules;
      extern std::set<std::string> allModules;
      extern std::queue<std::string> modulesForPars;
      extern std::string curModuleFileName;
      extern std::string curModuleName;
      extern bool rightEnding;
      extern std::string curModuleFileExtension;
  } // namespace helper
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

%token SC COLON COMMA DOT_DOT APOSTR
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
%left AND OR XOR NOT
%left EQ NEQ MORE LESS GTE LTE
%left PLUS MINUS AMPER
%left MUL DIV MOD
%left DOT
%left LPAR RPAR

%token ERR

%nonassoc UMINUS

%nterm<std::shared_ptr<node::IDecl>> decl
%nterm<std::shared_ptr<node::IDecl>> pack_decl_decl
%nterm<std::shared_ptr<node::IDecl>> var_decl
%nterm<std::shared_ptr<node::With>> with
%nterm<std::shared_ptr<node::Use>> use
%nterm<attribute::QualifiedName> qualified_name
%nterm<attribute::Attribute> getting_attribute
%nterm<std::shared_ptr<node::IDecl>> proc_body
%nterm<std::shared_ptr<node::IDecl>> func_body
%nterm<std::shared_ptr<node::IDecl>> proc_decl
%nterm<std::shared_ptr<node::IDecl>> func_decl
%nterm<std::shared_ptr<node::IDecl>> pack_body
%nterm<std::shared_ptr<node::IDecl>> pack_decl
%nterm<std::shared_ptr<node::IDecl>> type_decl
%nterm<std::shared_ptr<node::IDecl>> record_decl
%nterm<std::shared_ptr<node::DeclArea>> vars_decl
%nterm<std::shared_ptr<node::VarDecl>> param
%nterm<std::vector<std::shared_ptr<node::VarDecl>>> param_list
%nterm<std::shared_ptr<node::IType>> string_type
%nterm<std::shared_ptr<node::IType>> array_type
%nterm<std::shared_ptr<node::IType>> type
%nterm<std::shared_ptr<node::IType>> type_with_no_arr
%nterm<std::vector<std::pair<int, int>>> array_range
%nterm<std::vector<std::pair<int, int>>> static_ranges
%nterm<std::pair<int, int>> static_range
%nterm<std::shared_ptr<node::Body>> body
%nterm<std::vector<std::shared_ptr<node::IStm>>> stms
%nterm<std::shared_ptr<node::IStm>> stm
%nterm<std::shared_ptr<node::IStm>> oper
%nterm<std::shared_ptr<node::IStm>> assign
%nterm<std::shared_ptr<node::IStm>> return_stm
%nterm<std::shared_ptr<node::IExpr>> expr
%nterm<std::vector<std::shared_ptr<node::IExpr>>> args
%nterm<std::shared_ptr<node::ILiteral>> literal
%nterm<std::vector<std::shared_ptr<node::ILiteral>>> literals
%nterm<std::shared_ptr<node::ILiteral>> aggregate
%nterm<std::shared_ptr<node::IStm>> if_stm
%nterm<std::shared_ptr<node::IExpr>> if_head
%nterm<std::vector<std::pair<std::shared_ptr<node::IExpr>, std::shared_ptr<node::Body>>>> elsifs
%nterm<std::pair<std::shared_ptr<node::IExpr>, std::shared_ptr<node::Body>>> elsif
%nterm<std::shared_ptr<node::Body>> else
%nterm<std::shared_ptr<node::IStm>> for_stm
%nterm<std::shared_ptr<node::IStm>> while_stm
%nterm<std::pair<std::shared_ptr<node::IExpr>, std::shared_ptr<node::IExpr>>> range
%nterm<std::shared_ptr<node::IDecl>> compile_unit
%nterm<std::shared_ptr<node::IDecl>> type_alias_decl
%nterm<std::shared_ptr<node::DeclArea>> optional_decl_area
%nterm<std::shared_ptr<node::DeclArea>> decl_area
%nterm<std::shared_ptr<node::DeclArea>> pack_decl_decl_area
%nterm<OptionalImports> optional_imports
%nterm<OptionalImports> imports
%nterm<std::shared_ptr<node::IStm>> mb_call

%start program

%%

program: optional_imports compile_unit                                  { 
                                                                          auto mod = std::make_shared<mdl::Module>(
                                                                            $2, $1.first, $1.second, 
                                                                            helper::curModuleName, helper::curModuleFileName,     
                                                                            helper::curModuleFileExtension);
                                                                          helper::modules.push_back(mod);
                                                                        }

/* declarations */
/* ################################################################################ */
with:             WITH qualified_name SC                                { 
                                                                          auto mdl = $2.first();
                                                                          utility::toLower(mdl);
                                                                          if (!helper::allModules.contains(mdl) && mdl != helper::curModuleName)
                                                                          { 
                                                                            helper::modulesForPars.push($2.first()); 
                                                                            helper::allModules.insert(mdl);
                                                                          }
                                                                          $$.reset(new node::With($2)); 
                                                                        }

use:              USE qualified_name SC                                 { $$.reset(new node::Use($2)); } 

optional_imports: imports
                | %empty                                                { $$ = OptionalImports({}, {}); }

imports:          with                                                  { $$ = OptionalImports({$1}, {}); }
                | use                                                   { $$ = OptionalImports({}, {$1}); }
                | imports with                                          { $$ = std::move($1); $$.first.push_back($2); }
                | imports use                                           { $$ = std::move($1); $$.second.push_back($2); }

decl_area:        decl                                                  { $$.reset(new node::DeclArea()); $$->addDecl($1); }  
                | decl_area decl                                        { $$ = $1; $$->addDecl($2); }

pack_decl_decl_area: pack_decl_decl                                     { $$.reset(new node::DeclArea()); $$->addDecl($1); }  
                |    pack_decl_decl_area pack_decl_decl                 { $$ = $1; $$->addDecl($2); }

decl:             var_decl
                | proc_body  
                | func_body
                | pack_decl
                | pack_body  
                | type_decl

pack_decl_decl:   var_decl
                | proc_decl 
                | func_decl
                | pack_decl
                | type_decl


var_decl:         NAME COLON type ASG expr SC                           { $$.reset(new node::VarDecl($1, $3, $5)); }
                | NAME COLON type SC                                    { $$.reset(new node::VarDecl($1, $3)); }

qualified_name:   NAME                                                  { $$ = attribute::QualifiedName($1); } 
                | qualified_name DOT NAME                               { $$ = std::move($1); $$.push($3); }       

proc_body:        PROCEDURE NAME IS optional_decl_area BEGIN_KW body END NAME SC                                     { 
                                                                                                                        $$.reset(new node::ProcBody($2, {}, $4, $6));
                                                                                                                        helper::rightEnding = ($2 == $8) && helper::rightEnding;
                                                                                                                     }
                | PROCEDURE NAME LPAR param_list RPAR IS optional_decl_area BEGIN_KW body END NAME SC                { 
                                                                                                                        $$.reset(new node::ProcBody($2, $4, $7, $9)); 
                                                                                                                        helper::rightEnding = ($2 == $11) && helper::rightEnding;
                                                                                                                     }

func_body:        FUNCTION NAME RETURN type IS optional_decl_area BEGIN_KW body END NAME SC                          { 
                                                                                                                        $$.reset(new node::FuncBody($2, {}, $6, $8, $4)); 
                                                                                                                        helper::rightEnding = ($2 == $10) && helper::rightEnding;
                                                                                                                     }  
                | FUNCTION NAME LPAR param_list RPAR RETURN type IS optional_decl_area BEGIN_KW body END NAME SC     { 
                                                                                                                        $$.reset(new node::FuncBody($2, $4, $9, $11, $7)); 
                                                                                                                        helper::rightEnding = ($2 == $13) && helper::rightEnding;
                                                                                                                     }

proc_decl:        PROCEDURE NAME SC                                                                                  { $$.reset(new node::ProcDecl($2)); }
                | PROCEDURE NAME LPAR param_list RPAR SC                                                             { $$.reset(new node::ProcDecl($2, $4)); }

func_decl:        FUNCTION NAME RETURN type SC                                                                       { $$.reset(new node::FuncDecl($2, {}, $4)); }  
                | FUNCTION NAME LPAR param_list RPAR RETURN type SC                                                  { $$.reset(new node::FuncDecl($2, $4, $7)); }

pack_decl:        PACKAGE NAME IS pack_decl_decl_area PRIVATE pack_decl_decl_area END NAME SC                        { 
                                                                                                                        $$.reset(new node::PackDecl($2, $4, $6)); 
                                                                                                                        helper::rightEnding = ($2 == $8) && helper::rightEnding;
                                                                                                                     }
                | PACKAGE NAME IS pack_decl_decl_area END NAME SC                                                    {  
                                                                                                                        $$.reset(new node::PackDecl($2, $4)); 
                                                                                                                        helper::rightEnding = ($2 == $6) && helper::rightEnding;
                                                                                                                     }
                | PACKAGE NAME IS PRIVATE pack_decl_decl_area END NAME SC                                            { 
                                                                                                                        $$.reset(new node::PackDecl($2, nullptr, $5)); 
                                                                                                                        helper::rightEnding = ($2 == $7) && helper::rightEnding;
                                                                                                                     }

pack_body:        PACKAGE BODY NAME IS decl_area END NAME SC                                                         { 
                                                                                                                        $$.reset(new node::PackBody($3, $5)); 
                                                                                                                        helper::rightEnding = ($3 == $7) && helper::rightEnding;
                                                                                                                     }

type_decl:        record_decl                    
                 | type_alias_decl
                  /* enum_decl */ /* TODO */

record_decl:      TYPE NAME IS RECORD vars_decl END RECORD SC                                                        { $$.reset(new node::RecordDecl($2, $5)); }
                | TYPE NAME IS TAGGED RECORD vars_decl END RECORD SC                                                 { $$.reset(new node::RecordDecl($2, $6, {}, true)); }
                | TYPE NAME IS NEW qualified_name WITH RECORD vars_decl END RECORD SC                                { $$.reset(new node::RecordDecl($2, $8, $5)); }

vars_decl:        var_decl                                              { 
                                                                          $$ = std::make_shared<node::DeclArea>();
                                                                          $$->addDecl($1);
                                                                        }
                | vars_decl var_decl                                    { 
                                                                          $$ = $1;  
                                                                          $$->addDecl($2); 
                                                                        }

param_list:       param                                                 { $$ = std::vector({$1}); }
                | param_list SC param                                   { $$ = std::move($1); $$.push_back($3); }

param:            NAME COLON type                                       { 
                                                                          if (auto str = std::dynamic_pointer_cast<node::StringType>($3)) {
                                                                            str->setInf();
                                                                          }
                                                                          std::shared_ptr<node::VarDecl> decl(new node::VarDecl($1, $3));
                                                                          decl->setIn(true);
                                                                          decl->setOut(false);
                                                                          $$ = decl; 
                                                                        }
                | NAME COLON IN type                                    { 
                                                                          if (auto str = std::dynamic_pointer_cast<node::StringType>($4)) {
                                                                            str->setInf();
                                                                          }
                                                                          std::shared_ptr<node::VarDecl> decl(new node::VarDecl($1, $4));
                                                                          decl->setIn(true);
                                                                          decl->setOut(false);
                                                                          $$ = decl; 
                                                                        }
                | NAME COLON OUT type                                   { 
                                                                          if (auto str = std::dynamic_pointer_cast<node::StringType>($4)) {
                                                                            str->setInf();
                                                                          }
                                                                          std::shared_ptr<node::VarDecl> decl(new node::VarDecl($1, $4));
                                                                          decl->setIn(false);
                                                                          decl->setOut(true);
                                                                          $$ = decl; 
                                                                        }
                | NAME COLON IN OUT type                                { 
                                                                          if (auto str = std::dynamic_pointer_cast<node::StringType>($5)) {
                                                                            str->setInf();
                                                                          }
                                                                          std::shared_ptr<node::VarDecl> decl(new node::VarDecl($1, $5));
                                                                          decl->setIn(true);
                                                                          decl->setOut(true);
                                                                          $$ = decl; 
                                                                        }
                  

optional_decl_area: %empty                                              { $$ = nullptr; }
                |   decl_area                                           

type_alias_decl:  TYPE NAME IS array_type SC                            { $$.reset(new node::TypeAliasDecl($2, $4)); }     
                | TYPE NAME IS NEW type_with_no_arr SC                  { $$.reset(new node::TypeAliasDecl($2, $5)); }        
                  


compile_unit:     proc_body                                             
                | func_body           
                | pack_decl 
                | pack_body

/* types */
/* ################################################################################ */
type_with_no_arr: INTEGERTY                                             { 
                                                                           $$.reset(new node::SimpleLiteralType(
                                                                                    node::SimpleType::INTEGER)); 
                                                                        }
                | FLOATTY                                               {
                                                                           $$.reset(new node::SimpleLiteralType(
                                                                                    node::SimpleType::FLOAT)); 
                                                                        }
                | CHARACTERTY                                           {
                                                                           $$.reset(new node::SimpleLiteralType(
                                                                                    node::SimpleType::CHAR)); 
                                                                        }
                | BOOLTY                                                {
                                                                           $$.reset(new node::SimpleLiteralType(
                                                                                    node::SimpleType::BOOL)); 
                                                                        }
                | qualified_name                                        { $$.reset(new node::TypeName($1)); }
                | string_type                                            
                | getting_attribute                                     { $$.reset(new node::TypeName($1)); }   

type:             type_with_no_arr 
                | array_type          


getting_attribute:   qualified_name DOT GETTING_ATTRIBUTE               { 
                                                                          $1.push($3.first);
                                                                          $$ = attribute::Attribute($1, $3.second);
                                                                        }
                |    GETTING_ATTRIBUTE                                  {
                                                                          $$ = attribute::Attribute($1.first, $1.second);
                                                                        }

string_type:      STRINGTY LPAR static_range RPAR                       { $$.reset(new node::StringType($3)); }

array_type:       ARRAY array_range OF type                             { $$.reset(new node::ArrayType($2, $4)); }

array_range:      LPAR static_ranges RPAR                               { $$ = std::move($2); }              

static_ranges:    static_range                                          { $$ = std::vector({$1}); }
                | static_ranges COMMA static_range                      { $$ = std::move($1); $$.push_back($3); }

static_range:     INTEGER DOT_DOT INTEGER                               { 
                                                                          $$ = std::make_pair($1, $3); 
                                                                          if ($1 != 1 || $1 > $3) {
                                                                            throw std::logic_error("Error range");
                                                                          }
                                                                        }

/* statements */
/* ################################################################################ */
body:             stms                                                  { $$.reset(new node::Body($1)); }

stms:             stm                                                   { $$ = std::vector({$1}); }
                | stms stm                                              { $$ = std::move($1); $$.push_back($2); }

stm:              oper                                                   
                | if_stm                                                 
                | while_stm                                              
                | for_stm                                                
                | return_stm                                             

oper:             assign                                                 
                | mb_call

mb_call:        expr SC                                                 { $$.reset(new node::MBCall($1)); }

assign:           expr ASG expr SC                                      { $$.reset(new node::Assign($1, $3)); }
                                                                               
return_stm:       RETURN expr SC                                        { $$.reset(new node::Return($2)); } 
                | RETURN SC                                             { $$.reset(new node::Return()); }                                        

expr:             expr EQ expr                                          { $$.reset(new node::Op($1, node::OpType::EQ, $3));          }
                | expr NEQ expr                                         { $$.reset(new node::Op($1, node::OpType::NEQ, $3));         }
                | expr MORE expr                                        { $$.reset(new node::Op($1, node::OpType::MORE, $3));        }
                | expr LESS expr                                        { $$.reset(new node::Op($1, node::OpType::LESS, $3));        }
                | expr GTE expr                                         { $$.reset(new node::Op($1, node::OpType::GTE, $3));         }
                | expr LTE expr                                         { $$.reset(new node::Op($1, node::OpType::LTE, $3));         }
                | expr AMPER expr                                       { $$.reset(new node::Op($1, node::OpType::AMPER, $3));       }
                | expr PLUS expr                                        { $$.reset(new node::Op($1, node::OpType::PLUS, $3));        }
                | expr MINUS expr                                       { $$.reset(new node::Op($1, node::OpType::MINUS, $3));       }
                | expr MUL expr                                         { $$.reset(new node::Op($1, node::OpType::MUL, $3));         }
                | expr DIV expr                                         { $$.reset(new node::Op($1, node::OpType::DIV, $3));         }
                | expr MOD expr                                         { $$.reset(new node::Op($1, node::OpType::MOD, $3));         }
                | LPAR expr RPAR                                        { $$ = $2; $$->setInBrackets();                              }
                | MINUS expr %prec UMINUS                               { $$.reset(new node::Op(nullptr, node::OpType::UMINUS, $2)); }
                | expr LPAR args RPAR                                   { $$.reset(new node::CallOrIdxExpr($1, $3));                 }
                | expr DOT expr                                         { $$.reset(new node::Op($1, node::OpType::DOT, $3));         }
                | LPAR expr AND expr RPAR                               { $$.reset(new node::Op($2, node::OpType::AND, $4));         }
                | LPAR expr OR expr RPAR                                { $$.reset(new node::Op($2, node::OpType::OR, $4));          }
                | LPAR expr XOR expr RPAR                               { $$.reset(new node::Op($2, node::OpType::XOR, $4));         }
                | LPAR NOT expr RPAR                                    { $$.reset(new node::Op(nullptr, node::OpType::NOT, $3));    }
                | literal                                               { $$ = $1;                                                   }
                | NAME                                                  { $$.reset(new node::NameExpr($1));                          }
                | GETTING_ATTRIBUTE                                     { 
                                                                          attribute::Attribute attr($1.first, $1.second);
                                                                          $$.reset(new node::AttributeExpr(attr));                     
                                                                        }

args:             expr                                                  { $$ = std::vector({$1}); }
                | args COMMA expr                                       { $$ = std::move($1); $$.push_back($3); }

literal:          INTEGER                                               { 
                                                                          std::shared_ptr<node::SimpleLiteralType> type 
                                                                            (new node::SimpleLiteralType(node::SimpleType::INTEGER));
                                                                          $$.reset(new node::SimpleLiteral(type, $1));
                                                                        }
                | BOOL                                                  {
                                                                          std::shared_ptr<node::SimpleLiteralType> type 
                                                                            (new node::SimpleLiteralType(node::SimpleType::BOOL));
                                                                          $$.reset(new node::SimpleLiteral(type, $1));
                                                                        }
                | CHAR                                                  {
                                                                          std::shared_ptr<node::SimpleLiteralType> type 
                                                                            (new node::SimpleLiteralType(node::SimpleType::CHAR));
                                                                          $$.reset(new node::SimpleLiteral(type, $1));
                                                                        }
                | STRING                                                {
                                                                          std::shared_ptr<node::StringType> type(
                                                                              new node::StringType(std::make_pair(1, $1.length())));
                                                                          $$.reset(new node::StringLiteral(type, $1));
                                                                        }
                | FLOAT                                                 {
                                                                          std::shared_ptr<node::SimpleLiteralType> type 
                                                                            (new node::SimpleLiteralType(node::SimpleType::FLOAT));
                                                                          $$.reset(new node::SimpleLiteral(type, $1));
                                                                        }
                | aggregate
                 
aggregate:       LPAR literals RPAR                                     { $$.reset(new node::Aggregate($2)); }

  
literals:         literal  COMMA literal                                { $$ = std::vector({$1, $3}); }
                | literals COMMA literal                                { $$ = std::move($1); $$.push_back($3); }


/* control structures */
/* ################################################################################ */
if_stm:          if_head body END IF SC                                 { $$.reset(new node::If($1, $2)); }
               | if_head body elsifs END IF SC                          { $$.reset(new node::If($1, $2, nullptr, $3)); }
               | if_head body elsifs else END IF SC                     { $$.reset(new node::If($1, $2, $4, $3)); }
               | if_head body else END IF SC                            { $$.reset(new node::If($1, $2, $3)); }

elsifs:          elsif                                                  { $$ = std::vector({$1}); }
               | elsifs elsif                                           { $$ = std::move($1); $$.push_back($2); }
 
elsif:           ELSIF expr THEN body                                   { $$ = std::make_pair($2, $4); }

else:            ELSE body                                              { $$ = $2; }

if_head:         IF expr THEN                                           { $$ = $2 ; }

for_stm:         FOR NAME IN range LOOP body END LOOP SC                { $$.reset(new node::For($2, $4, $6)); }
 
range:           expr DOT_DOT expr                                      { $$ = std::make_pair($1, $3); }

while_stm:       WHILE expr LOOP body END LOOP SC                       { $$.reset(new node::While($2, $4)); }

%%

#include "helper.hpp"

namespace yy {
  parser::token_type yylex(parser::semantic_type* val, 
                            yy::parser::location_type* loc, 
                            FlexLexer* lexer) 
  {
    helper::yylval = val; 
    auto tt = static_cast<parser::token_type>(lexer->yylex());
    loc->initialize(&helper::curModuleFileName);
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
