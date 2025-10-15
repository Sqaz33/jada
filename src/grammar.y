/*
procedure Name is 

Name : Name := value;

-- comment

begin

Put_line(Name'Name(Name));

end Name;

*/




%language "c++"

%skeleton "lalr1.cc"
%defines
%define api.value.type variant
%define parse.trace  

%%code requires 
{ #include <iostream> }


%token COLON SC
%token <std::string> NAME
%token <int> INTEGER
%token PROCEDURE IS BEGIN END 
%token INTEGERTY
%token OUTPUT
%token NEW_LINE
%token IMAGE
%right ASG
%token ASG
%left PLUS MINUS
%left MUL DIV MOD

%nterm <int> expr

%start program

%% 

program: procedure

procedure: procedure_declaration var_defs body procedure_end

procedure_declaration: PROCEDURE NAME IS 

var_defs: var_decl
        | var_def
        | var_defs var_decl
        | var_defs var_def

var_decl: NAME COLON NAME SC
var_def:  NAME COLON NAME ASG VALUE SC

body: ...

procedure_end: END NAME SC

%%   

namespace yy {

void parser::error(const std::string&) { 
    throw std::invalid_argument("Unknown token"); 
}

} 