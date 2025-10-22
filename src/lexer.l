%option c++

%{
    #include <iostream>
    #include "string_utility.hpp"
    // #include "grammar.tab.hh"
%}

NAME          [a-zA-Z_][a-zA-Z0-9_]*
DIGIT         [0-9]
HEXDIGIT      [0-9A-Fa-f]
INTEGER       {DIGIT}(_?{DIGIT})*
BINARY        2#[01](_?[01])*#
OCT           8#[0-7](_?[0-7])*#
HEX           16#{HEXDIGIT}(_?{HEXDIGIT})*#
FLOAT         {INTEGER}\.{INTEGER}([Ee][+-]?{INTEGER})?


CHAR          \'([^\'\n])\'
STRING        \"([^\"\n]|\"\")*\"

ATTRIBUTE_CALL    {NAME}\s*\'\s*{NAME}
BAD_CHAR_EMPTY     \'\'
BAD_CHAR_LONG      \'([^\'\n]{2,})\'
BAD_CHAR_UNTERM    \'([^\'\n]+)
BAD_STRING_NO_CLOSE_DOUBLE_QUOTE \"([^\"\n]|\"\")*\n?
BAD_STRING_LINE_FEED \"(([^\"]|\"\")*\n+([^\"]|\"\")*)*\"


%x COMMENT


%%

"--"                  { BEGIN(COMMENT); }   /* вошли в состояние COMMENT */
 
<COMMENT>.*           { 
                        std::cout << "COMMENT " << yytext << std::endl; 
                        BEGIN(INITIAL); 
                      }

[ \t\n\r\v]+          /* игнорируем пробелы и переводы строк */

";"                   { std::cout << "SC ;" << std::endl; }
":"                   { std::cout << "COLON :" << std::endl; }
"="                   { std::cout << "EQ =" << std::endl; }
"/="                  { std::cout << "NEQ /=" << std::endl; }
":="                  { std::cout << "ASG :=" << std::endl; }
"("                   { std::cout << "LPAR (" << std::endl; }
")"                   { std::cout << "RPAR )" << std::endl; }
"."                   { std::cout << "DOT ." << std::endl; }
"&"                   { std::cout << "AMPER &" << std::endl; }
"'"                   { std::cout << "APOSTR '" << std::endl; }
","                   { std::cout << "COMMA ," << std::endl; }

"if"                  { std::cout << "IF " << yytext << std::endl; }
"then"                { std::cout << "THEN " << yytext << std::endl; }
"else"                { std::cout << "ELSE " << yytext << std::endl; }
"elsif"               { std::cout << "ELSIF " << yytext << std::endl; }

"for"                 { std::cout << "FOR " << yytext << std::endl; }
"loop"                { std::cout << "LOOP " << yytext << std::endl; }
"in"                   { std::cout << "IN " << yytext << std::endl; }
"exit"                 { std::cout << "EXIT " << yytext << std::endl; }
"when"                 { std::cout << "WHEN " << yytext << std::endl; }
"while"                { std::cout << "WHILE " << yytext << std::endl; }

"with"                 { std::cout << "WITH " << yytext << std::endl; }
"use"                  { std::cout << "USE " << yytext << std::endl; }

"procedure"            { std::cout << "PROCEDURE " << yytext << std::endl; }
"function"             { std::cout << "FUNCTION " << yytext << std::endl; }
"begin"                { std::cout << "BEGIN " << yytext << std::endl; }
"is"                   { std::cout << "IS " << yytext << std::endl; }
"end"                  { std::cout << "END " << yytext << std::endl; }
"return"               { std::cout << "RETURN " << yytext << std::endl; }

"package"              { std::cout << "PACKAGE " << yytext << std::endl; }
"body"                 { std::cout << "BODY " << yytext << std::endl; }
"type"                 { std::cout << "TYPE " << yytext << std::endl; }
"tagged"               { std::cout << "TAGGED " << yytext << std::endl; }
"record"               { std::cout << "RECORD " << yytext << std::endl; }
"overriding"           { std::cout << "OVERRIDING " << yytext << std::endl; }
"new"                  { std::cout << "NEW " << yytext << std::endl; }

"Integer"              { std::cout << "INTEGERTY " << yytext << std::endl; }
"String"               { std::cout << "STRINGTY " << yytext << std::endl; }
"Character"            { std::cout << "CHARACTERTY " << yytext << std::endl; }
"Float"                { std::cout << "FLOATTY " << yytext << std::endl; }
"Boolean"              { std::cout << "BOOLTY " << yytext << std::endl; }
      
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
{FLOAT}               { 
                        std::string text(yytext);
                        utility::replaceAll(text, "_", "");
                        float res = std::stof(text);
                        std::cout << "FLOAT " << res << std::endl; 
                      }
{CHAR}                { std::cout << "CHAR " << yytext[1] << std::endl; }
{STRING}              { 
                        std::string_view sv(yytext);
                        sv.remove_prefix(1);
                        sv.remove_suffix(1);
                        std::string res(sv.begin(), sv.end());
                        utility::replaceAll(res, "\"\"", "\"");
                        std::cout << "STRING " << res << std::endl; 
                      }
                      
"False"                { std::cout << "FALSE " << yytext << std::endl; }
"True"                 { std::cout << "TRUE " << yytext << std::endl; }
"null"                 { std::cout << "NULL " << yytext << std::endl; }

"+"                    { std::cout << "PLUS +" << std::endl; }
"-"                    { std::cout << "MINUS -" << std::endl; }
"/"                    { std::cout << "DIV /" << std::endl; }
"*"                    { std::cout << "MUL *" << std::endl; }
"<"                    { std::cout << "LESS <" << std::endl; }
">"                    { std::cout << "MORE >" << std::endl; }

"or"                   { std::cout << "OR " << yytext << std::endl; }
"not"                  { std::cout << "NOT " << yytext << std::endl; }
"and"                  { std::cout << "AND " << yytext << std::endl; }

"New_Line"             { std::cout << "NEWLINE " << yytext << std::endl; }
"Put_Line"             { std::cout << "PUTLINE " << yytext << std::endl; }
      
{NAME}                { std::cout << "NAME " << yytext << std::endl; }
{ATTRIBUTE_CALL}      { std::cout << "ATTRIBUTE_CALL: "<< yytext << std::endl; }

{BAD_CHAR_EMPTY}      { std::cout << "Empty char: " << yytext << std::endl; }
{BAD_CHAR_LONG}       { std::cout << "Bad char long: " << yytext << std::endl; }
{BAD_CHAR_UNTERM}     { std::cout << "Bad char unterm: " << yytext << std::endl; }
{BAD_STRING_LINE_FEED} { std::cout << "Bad string with LF: " << yytext << std::endl; }
{BAD_STRING_NO_CLOSE_DOUBLE_QUOTE} {
                        std::cout   
                          << "Bad string: no close double quote: "
                          << yytext
                          << std::endl;
                      }

.                     { std::cout << "UNKNOWN " << yytext << std::endl; }

%%
