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

%token SC            
%token COLON
%token DOT_DOT         
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
%token PRIVATE         
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

%token ERR

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

%nterm stm            
%nterm assign
%nterm expr
%nterm lval
%nterm decl
%nterm decl_area
%nterm param_list


%start program

%%

program: decl_area 

decl_area:        decl
                | decl_area decl

decl:             var_decl
                | proc_decl  
                | func_decl  
                | pack_decl  
                | type_decl  
             /* | use_decl */     /* TODO */
             /* | import_decl */ /* TODO */

var_decl:         NAME COLON INTEGERTY /* NAME */ ASG expr SC 
                | NAME COLON INTEGERTY SC                                               
                                                                                        
proc_decl:        PROCEDURE NAME IS decl_area BEGIN_KW body END NAME SC
                | PROCEDURE NAME LPAR param_list RPAR BEGIN_KW body END NAME SC
                | OVERRIDING proc_decl

func_decl:        FUNCTION NAME RETURN NAME IS decl_area BEGIN_KW body END NAME SC                 
                | FUNCTION NAME LPAR param_list RPAR RETURN NAME IS decl_area BEGIN_KW body END NAME SC
                | OVERRIDING func_decl /* может не работать */

pack_decl:        PACKAGE NAME IS decl_area END NAME SC         
                | PACKAGE NAME IS decl_area PRIVATE decl_area END NAME SC         

type_decl:        record_decl
                  /* enum_decl */

record_decl:      TYPE NAME IS RECORD vars_decl END RECORD SC 
                | TYPE NAME IS NEW NAME WITH RECORD vars_decl END RECORD SC

vars_decl:        var_decl
                | vars_decl var_decl

param:            NAME COLON NAME

param_list        param
                | param_list SC param
                                                                   
/* ----------------------------------------------------------------------------------------------- */
body:             stms

stms:             stm
                | stms stm

stm:              oper
                | if_stm
             /* | case_stm */
                | while_stm
                | for_stm
                | call_stm
                    
call_stm:         call SC

oper:             assign

assign:           lval ASG expr SC            
                                                
lval:             NAME                      

expr:             expr PLUS expr              
                | expr MINUS expr             
                | expr MUL expr               
                | expr DIV expr               
                | expr MOD expr               
                | MINUS expr %prec UMINUS     
                | NAME                        
                | INTEGER /* literal */       
                | LPAR expr RPAR              
             /* | LITERAL */ 
                | call

call: NAME LPAR RPAR 

                /*логический операции, конкатенация, взятие элемента массива*/
                 
/* ----------------------------------------------------------------------------------------------- */
if_stm:          if_head body END IF SC
               | if_head body elsifs END IF SC
               | if_head body elsifs else END IF SC
               | if_head body else END IF SC

elsifs:          elsif         
               | elsifs elsif  

elsif:           ELSIF expr THEN body

else:            ELSE body

if_head:         IF expr THEN 

/* ----------------------------------------------------------------------------------------------- */
for_stm:        FOR NAME IN range LOOP body END LOOP

range:        expr DOT_DOT expr


while_stm:        WHILE expr LOOP body END LOOP   



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
