%option c++

%{
    #include <iostream>
    #include "string_utility.hpp"
    #include "grammar.tab.hh"
%}

WS            [ \t\n\r\v]+
SC            ";"
COLON         ":"
EQ            "="
NEQ           "/="
ASG           ":="
LPAR          "("
RPAR          ")"
DOT           "."
AMPER         "&"
APOSTR        "'"
NAME          [a-zA-Z_][a-zA-Z0-9_]*
DIGIT         [0-9]
HEXDIGIT      [0-9A-Fa-f]

IF            if
THEN          then
ELSE          else
ELSIF         elsif

FOR           for
LOOP          loop
IN            in
EXIT          exit
WHEN          when
WHILE         while

WITH          with
USE           use

PROCEDURE     procedure
FUNCTION      function
BEGIN         begin
IS            is
END           end
RETURN        return

PACKAGE       package
BODY          body
TYPE          type
TAGGED        tagged
RECORD        record
OVERRIDING    overriding
NEW           new     

INTEGERTY     Integer
STRINGTY      String
CHARACTERTY   Character
FLOATTY       Float
BOOLTY        Boolean

INTEGER       {DIGIT}(_?{DIGIT})*
BINARY        2#[01](_?[01])*#
OCT           8#[0-7](_?[0-7])*#
HEX           16#{HEXDIGIT}(_?{HEXDIGIT})*#
FLOAT         {INTEGER}\.{INTEGER}([Ee][+-]?{INTEGER})?
FALSE         False
TRUE          True
NULL          null

CHAR          \'([^\'\n])\'
STRING        \"([^\"\n]|\"\")*\"

BAD_CHAR_EMPTY     \'\'
BAD_CHAR_LONG      \'([^\']{2,})\'
BAD_CHAR_UNTERM    \'([^\']+)
BAD_STRING_NO_CLOSE_DOUBLE_QUOTE \"([^\"\n]|\"\")*\n?

%x COMMENT

PLUS          "+"
MINUS         "-"
DIV           "/"
MUL           "*"
LESS          "<"
MORE          ">"

OR            or
NOT           not
AND           and

NEWLINE       New_Line
PUTLINE       Put_Line
IMAGE         Image

%%

"--"                  { BEGIN(COMMENT); }   /* вошли в состояние COMMENT */
 
<COMMENT>.*           { 
                        std::cout << "COMMENT " << yytext << std::endl; 
                        BEGIN(INITIAL); 
                      }

{WS}            /* игнорируем пробелы и переводы строк */

{SC}                  { std::cout << "SC ;" << std::endl; }
{COLON}               { std::cout << "COLON :" << std::endl; }
{EQ}                  { std::cout << "EQ =" << std::endl; }
{NEQ}                 { std::cout << "NEQ /=" << std::endl; }
{ASG}                 { std::cout << "ASG :=" << std::endl; }
{LPAR}                { std::cout << "LPAR (" << std::endl; }
{RPAR}                { std::cout << "RPAR )" << std::endl; }
{DOT}                 { std::cout << "DOT ." << std::endl; }
{AMPER}               { std::cout << "AMPER &" << std::endl; }
{APOSTR}              { std::cout << "APOSTR '" << std::endl; }
      
{WHILE}               { std::cout << "WHILE " << yytext << std::endl; }
{ELSE}                { std::cout << "ELSE " << yytext << std::endl; }
{IF}                  { std::cout << "IF " << yytext << std::endl; }
{THEN}                { std::cout << "THEN " << yytext << std::endl; }
{ELSIF}               { std::cout << "ELSIF " << yytext << std::endl; }
{FOR}                 { std::cout << "FOR " << yytext << std::endl; }
{LOOP}                { std::cout << "LOOP " << yytext << std::endl; }
{IN}                  { std::cout << "IN " << yytext << std::endl; }
{EXIT}                { std::cout << "EXIT " << yytext << std::endl; }
{WHEN}                { std::cout << "WHEN " << yytext << std::endl; }
{WITH}                { std::cout << "WITH " << yytext << std::endl; }
{USE}                 { std::cout << "USE " << yytext << std::endl; }
{PROCEDURE}           { std::cout << "PROCEDURE " << yytext << std::endl; }
{FUNCTION}            { std::cout << "FUNCTION " << yytext <<   std::endl; }
{BEGIN}               { std::cout << "BEGIN " << yytext << std::endl; }
{IS}                  { std::cout << "IS " << yytext << std::endl; }
{END}                 { std::cout << "END " << yytext << std::endl; }
{RETURN}              { std::cout << "RETURN " << yytext << std::endl; }
{PACKAGE}             { std::cout << "PACKAGE " << yytext << std::endl; }
{BODY}                { std::cout << "BODY " << yytext << std::endl; }
{TYPE}                { std::cout << "TYPE " << yytext << std::endl; }
{TAGGED}              { std::cout << "TAGGED " << yytext << std::endl; }
{RECORD}              { std::cout << "RECORD " << yytext << std::endl; }
{OVERRIDING}          { std::cout << "OVERRIDING " << yytext << std::endl; }
{NEW}                 { std::cout << "NEW " << yytext << std::endl; }
       
{INTEGERTY}           { std::cout << "INTEGERTY " << yytext << std::endl; }
{STRINGTY}            { std::cout << "STRINGTY " << yytext << std::endl; }
{CHARACTERTY}         { std::cout << "CHARACTERTY " << yytext << std::endl; }
{FLOATTY}             { std::cout << "FLOATTY " << yytext << std::endl; }
{BOOLTY}              { std::cout << "BOOLTY " << yytext << std::endl; }
      
{INTEGER}             { 
                        std::string text(yytext);
                        utility::replaceAll(text, "_", "");
                        int res = std::stoi(text);
                        std::cout << "INTEGER " << res << std::endl; 
                      }
{BINARY}              { 
                        std::string_view sv(yytext);
                        sv.remove_prefix(2);
                        sv.remove_suffix(1);
                        std::string text(sv.begin(), sv.end());
                        utility::replaceAll(text, "_", "");
                        int res = std::stoi(text, nullptr, 2);
                        std::cout << "BINARY " << res << std::endl; 
                      }
{OCT}                 { 
                        std::string_view sv(yytext);
                        sv.remove_prefix(2);
                        sv.remove_suffix(1);
                        std::string text(sv.begin(), sv.end());
                        utility::replaceAll(text, "_", "");
                        int res = std::stoi(text, nullptr, 8);
                        std::cout << "OCT " << res << std::endl; 
                      }
{HEX}                 { 
                        std::string_view sv(yytext);
                        sv.remove_prefix(3);
                        sv.remove_suffix(1);
                        std::string text(sv.begin(), sv.end());
                        utility::replaceAll(text, "_", "");
                        int res = std::stoi(text, nullptr, 16);
                        std::cout << "HEX " << res << std::endl; 
                      }
{FLOAT}               { std::cout << "FLOAT " << yytext << std::endl; }
{CHAR}                { std::cout << "CHAR " << yytext[1] << std::endl; }
{STRING}              { 
                        std::string_view sv(yytext);
                        sv.remove_prefix(1);
                        sv.remove_suffix(1);
                        std::string res(sv.begin(), sv.end());
                        utility::replaceAll(res, "\"\"", "\"");
                        std::cout << "STRING " << res << std::endl; 
                      }
{FALSE}               { std::cout << "FALSE " << yytext << std::endl; }
{TRUE}                { std::cout << "TRUE " << yytext << std::endl; }
{NULL}                { std::cout << "NULL " << yytext << std::endl; }
      
{PLUS}                { std::cout << "PLUS +" << std::endl; }
{MINUS}               { std::cout << "MINUS -" << std::endl; }
{DIV}                 { std::cout << "DIV /" << std::endl; }
{MUL}                 { std::cout << "MUL *" << std::endl; }
{LESS}                { std::cout << "LESS <" << std::endl; }
{MORE}                { std::cout << "MORE >" << std::endl; }
      
{OR}                  { std::cout << "OR " << yytext << std::endl; }
{NOT}                 { std::cout << "NOT " << yytext << std::endl; }
{AND}                 { std::cout << "AND " << yytext << std::endl; }
      
{NEWLINE}             { std::cout << "NEWLINE " << yytext << std::endl; }
{PUTLINE}             { std::cout << "PUTLINE " << yytext << std::endl; }
{IMAGE}               { std::cout << "IMAGE " << yytext << std::endl; }
      
{NAME}                { std::cout << "NAME " << yytext << std::endl; }
      

{BAD_CHAR_EMPTY}      { std::cout << "Empty char: " << yytext << std::endl; }
{BAD_CHAR_LONG}       { std::cout << "Bad char long: " << yytext << std::endl; }
{BAD_CHAR_UNTERM}     { std::cout << "Bad char unterm: " << yytext << std::endl; }
{BAD_STRING_NO_CLOSE_DOUBLE_QUOTE} {
                        std::cout   
                          << "Bad string: no close double quote: "
                          << yytext
                          << std::endl;
                      }

.                     { std::cout << "UNKNOWN " << yytext << std::endl; }

%%
