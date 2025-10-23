%option c++

%{
  #include <string>    
  #include <string_view>
  #include <regex>
  #include <sstream>

  #include "string_utility.hpp"
  #include "parser.hpp"
  #include "helper.hpp"

  #define YY_USER_ACTION helper::lineNo = yylineno; \
      helper::columnNo += yyleng;

  using helper::yylval;

  void handleLexicalError(std::string msg, std::string err = "") {
      std::stringstream ss;
      ss << helper::moduleFileNames.back()
         << ':' << helper::lineNo 
         << ':' << helper::columnNo 
         << ':';
      ss << " lexical error: ";
      ss << std::move(msg);
      ss << ": ";
      ss << std::move(err);
      helper::errs.push_back(ss.str());
      helper::hasErr = true;
  }
%}

NAME          [a-zA-Z_][a-zA-Z0-9_]*
DIGIT         [0-9]
HEXDIGIT      [0-9A-Fa-f]
INTEGER       {DIGIT}(_?{DIGIT})*
BINARY        2#[01](_?[01])*#
OCT           8#[0-7](_?[0-7])*#
HEX           16#{HEXDIGIT}(_?{HEXDIGIT})*#`
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
 
<COMMENT>.*           { BEGIN(INITIAL); }

[ \t\r\v]+            /* игнорируем пробелы и переводы строк */
"\n"                  { helper::columnNo = 1; }

";"                   { return yy::parser::token_type::SC; }
":"                   { return yy::parser::token_type::COLON; }
"="                   { return yy::parser::token_type::EQ; }
"/="                  { return yy::parser::token_type::NEQ; }
":="                  { return yy::parser::token_type::ASG; }
"("                   { return yy::parser::token_type::LPAR; }
")"                   { return yy::parser::token_type::RPAR;}
"."                   { return yy::parser::token_type::DOT; }
"&"                   { return yy::parser::token_type::AMPER; }
"'"                   { return yy::parser::token_type::APOSTR; }
","                   { return yy::parser::token_type::COMMA; }

"if"                  { return yy::parser::token_type::IF; }
"then"                { return yy::parser::token_type::THEN; }
"else"                { return yy::parser::token_type::ELSE; }
"elsif"               { return yy::parser::token_type::ELSIF; }

"for"                 { return yy::parser::token_type::FOR; }
"loop"                { return yy::parser::token_type::LOOP; }
"in"                  { return yy::parser::token_type::IN; }
"exit"                { return yy::parser::token_type::EXIT; }
"when"                { return yy::parser::token_type::WHEN; }
"while"               { return yy::parser::token_type::WHILE; }

"with"                { return yy::parser::token_type::WITH; }
"use"                 { return yy::parser::token_type::USE; }

"procedure"           { return yy::parser::token_type::PROCEDURE; }
"function"            { return yy::parser::token_type::FUNCTION; }
"begin"               { return yy::parser::token_type::BEGIN_KW; }
"is"                  { return yy::parser::token_type::IS; }
"end"                 { return yy::parser::token_type::END; }
"return"              { return yy::parser::token_type::RETURN; }

"package"             { return yy::parser::token_type::PACKAGE; } 
"body"                { return yy::parser::token_type::BODY; } 
"type"                { return yy::parser::token_type::TYPE; } 
"tagged"              { return yy::parser::token_type::TAGGED; } 
"record"              { return yy::parser::token_type::RECORD; } 
"overriding"          { return yy::parser::token_type::OVERRIDING; } 
"new"                 { return yy::parser::token_type::NEW; } 

"Integer"             { return yy::parser::token_type::INTEGERTY; } 
"String"              { return yy::parser::token_type::STRINGTY; } 
"Character"           { return yy::parser::token_type::CHARACTERTY; }
"Float"               { return yy::parser::token_type::FLOATTY; } 
"Boolean"             { return yy::parser::token_type::BOOLTY; } 
      
{INTEGER}             { 
                        std::string text(yytext, yyleng);
                        utility::replaceAll(text, "_", "");
                        int res = std::stoi(text);
                        yylval->emplace<int>(res);
                        return yy::parser::token_type::INTEGER;
                      }
{BINARY}              { 
                        std::string_view sv(yytext, yyleng);
                        sv.remove_prefix(2);
                        sv.remove_suffix(1);
                        std::string text(sv.begin(), sv.end());
                        utility::replaceAll(text, "_", "");
                        int res = std::stoi(text, nullptr, 2);
                        yylval->emplace<int>(res);
                        return yy::parser::token_type::INTEGER;
                      }
{OCT}                 { 
                        std::string_view sv(yytext, yyleng);
                        sv.remove_prefix(2);
                        sv.remove_suffix(1);
                        std::string text(sv.begin(), sv.end());
                        utility::replaceAll(text, "_", "");
                        int res = std::stoi(text, nullptr, 8);
                        yylval->emplace<int>(res);
                        return yy::parser::token_type::INTEGER;
                      }
{HEX}                 { 
                        std::string_view sv(yytext, yyleng);
                        sv.remove_prefix(3);
                        sv.remove_suffix(1);
                        std::string text(sv.begin(), sv.end());
                        utility::replaceAll(text, "_", "");
                        int res = std::stoi(text, nullptr, 16);
                        yylval->emplace<int>(res);
                        return yy::parser::token_type::INTEGER;
                      }
{FLOAT}               { 
                        std::string text(yytext, yyleng);
                        utility::replaceAll(text, "_", "");
                        float res = std::stof(text);
                        yylval->emplace<float>(res);
                        return yy::parser::token_type::FLOAT;
                      }
{CHAR}                { 
                        yylval->emplace<char>(yytext[1]);
                        return yy::parser::token_type::CHAR;
                      }
{STRING}              { 
                        std::string_view sv(yytext, yyleng);
                        sv.remove_prefix(1);
                        sv.remove_suffix(1);
                        std::string res(sv.begin(), sv.end());
                        utility::replaceAll(res, "\"\"", "\"");
                        yylval->emplace<std::string>(res);
                        return yy::parser::token_type::STRING;
                      }
                      
"False"               { return yy::parser::token_type::FALSE; }
"True"                { return yy::parser::token_type::TRUE; }
"null"                { return yy::parser::token_type::NULL_KW; }

"+"                   { return yy::parser::token_type::PLUS;} 
"-"                   { return yy::parser::token_type::MINUS;} 
"/"                   { return yy::parser::token_type::DIV;} 
"*"                   { return yy::parser::token_type::MUL;} 
"<"                   { return yy::parser::token_type::MORE;} 
">"                   { return yy::parser::token_type::LESS;} 

"or"                  { return yy::parser::token_type::OR; } 
"not"                 { return yy::parser::token_type::NOT; } 
"and"                 { return yy::parser::token_type::AND; } 

      
{NAME}                { 
                        std::string text(yytext, yyleng);
                        yylval->emplace<std::string>(std::move(text));
                        return yy::parser::token_type::NAME;
                      }
{ATTRIBUTE_CALL}      { 
                        std::string text(yytext, yyleng);
                        std::regex re(R"(([a-zA-Z_][a-zA-Z0-9_]*)\s*'\s*([a-zA-Z_][a-zA-Z0-9_]*))");
                        std::smatch match;
                        std::regex_match(text, match, re);
                        std::pair<std::string, std::string> res(match[1], match[2]);
                        yylval->
                          emplace<std::pair<std::string, std::string>>(std::move(res));
                        return yy::parser::token_type::ATTRIBUTE_CALL;
                        
                      }

{BAD_CHAR_EMPTY}      { handleLexicalError("Empty char"); }
{BAD_CHAR_LONG}       { handleLexicalError("Bad char long", std::string(yytext, yyleng)); }
{BAD_CHAR_UNTERM}     { handleLexicalError("Bad char unterm", std::string(yytext, yyleng)); }
{BAD_STRING_LINE_FEED} { handleLexicalError("Bad string with LF", std::string(yytext, yyleng)); }
{BAD_STRING_NO_CLOSE_DOUBLE_QUOTE} { handleLexicalError(
                                      "Bad string no close double quote",
                                       std::string(yytext, yyleng));
                                   }

.                     { handleLexicalError("Unknown input text", std::string(yytext, yyleng)); }

%%
