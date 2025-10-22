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
%token EQ            
%token NEQ           
%token ASG           
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
%token BEGIN         
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
%token NULL          

%token<char> CHAR          
%token<std::string> STRING        

%token<std::pair<std::string, std::string>> ATTRIBUTE_CALL 





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
