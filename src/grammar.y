%language "c++"

%skeleton "lalr1.cc"

%param {FlexLexer* lexer}
%define api.value.type variant
%define parse.trace  

%code requires
{
  #include <string>
  #include <utility>
  #include <iostream>                        // TODO delete then
  #include <unordered_map>                   // TODO: delete then
  
  namespace dummy {
    extern std::unordered_map<std::string, int> vars; // TODO: delete then
  }

  class FlexLexer; 
}

%code 
{
  #include <FlexLexer.h>

  namespace yy {
      parser::token_type yylex(parser::semantic_type* yylval, FlexLexer* lexer); 
  }
}

%token SC            
%token COLON         
%token EQ            
%token NEQ           
%right ASG           
%token LPAR          
%token RPAR          
%token DOT           
%token AMPER         
%token APOSTR        
%token<std::string> NAME          
%token COMMA        

%token IF            
%token THEN          
%token ELSE          
%token ELSIF         

%token FOR           
%token LOOP          
%token IN            
%token EXIT          
%token WHEN          
%token WHILE         

%token WITH          
%token USE           

%token PROCEDURE     
%token FUNCTION      
%token BEGIN_KW         
%token IS            
%token END           
%token RETURN        

%token PACKAGE       
%token BODY          
%token TYPE          
%token TAGGED        
%token RECORD        
%token OVERRIDING    
%token NEW           

%token INTEGERTY     
%token STRINGTY      
%token CHARACTERTY   
%token FLOATTY       
%token BOOLTY        

%token<int> INTEGER               
%token<float> FLOAT         
%token FALSE         
%token TRUE          
%token NULL_KW

%token<char> CHAR          
%token<std::string> STRING        

%token<std::pair<std::string, std::string>> ATTRIBUTE_CALL 

%left INPUT
%left OR
%left AND
%left NOT
%left PLUS 
%left MINUS
%left MUL 
%left DIV 
%left MOD
%left MORE
%left LESS

%nonassoc UMINUS

%nterm <int> stm            
%nterm assign
%nterm <int> expr
%nterm <int*> lval



%start program

%%

program: decl_area

decl_area:        decl_area var_decl
                | decl_area proc_decl
             /* | decl_area func_decl */ 
             /* | decl_area pack_decl */
             /* | decl_area type_decl */
             /* | decl_area import_decl */
                | %empty

var_decl:         NAME COLON INTEGERTY /* NAME */ ASG INTEGER /* literal or expr */ SC { 
                                                                                          std::cout << "Var decl:" << $1 << ' ' << $5 << '\n';
                                                                                          dummy::vars[$1] = $5; 
                                                                                        }
                | NAME COLON INTEGERTY SC                                               {
                                                                                          std::cout << "Var decl:" << $1 << '\n';
                                                                                          dummy::vars[$1] = 0; 
                                                                                        }

proc_decl:        PROCEDURE NAME IS decl_area BEGIN_KW body END NAME SC

body:             stms

stms:             stm
                | stms stm

stm:              oper

oper:             assign

assign:           lval ASG expr SC            { 
                                                std::cout << $3 << '\n';
                                                *$1 = $3;
                                              }

lval:             NAME                        {  
                                                std::cout << $1 << " -- NOW:";
                                                $$ = &dummy::vars[$1]; 
                                              }

expr:             expr PLUS expr              { $$ = $1 + $3; }
                | expr MINUS expr             { $$ = $1 - $3; }
                | expr MUL expr               { $$ = $1 * $3; }
                | expr DIV expr               { $$ = $1 / $3; }
                | expr MOD expr               { $$ = $1 % $3; }
                | MINUS expr %prec UMINUS     { $$ = -$2; }
                | NAME                        { $$ = dummy::vars.at($1); }
                | INTEGER /* literal */       { $$ = $1; }
                | LPAR expr RPAR              { $$ = $2; }
                 


%%



#include "helper.hpp"

namespace yy {
  parser::token_type yylex(parser::semantic_type* val, FlexLexer* lexer) {
    helper::yylval = val; 
    auto tt = static_cast<parser::token_type>(lexer->yylex());
    return tt;
  }

  void parser::error(const std::string& s) {
    std::cerr << "Error: " << s << std::endl;
  }
}
