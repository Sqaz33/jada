%language "c++"

%skeleton "lalr1.cc"
%defines
%define api.value.type variant
%define parse.trace  

%%code requires 
{ #include <iostream> }


%token <std::string> NAME
%token <int> INTEGER
%token PROCEDURE IS BEGIN END
%token INTEGERTY
%token OUTPUT
%token NEW_LINE
%token IMAGE
%token ASG
%left PLUS MINUS
%left MUL DIV MOD

%nterm <int> expr

%start program

%% 
           

namespace yy {

void parser::error(const std::string&) { 
    throw std::invalid_argument("Unknown token"); 
}

} 