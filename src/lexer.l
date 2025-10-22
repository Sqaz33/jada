%option c++

%{
  #iclude <string>    
  #include <string_view>
  #include <regex>
  #include <sstream>

  #include "string_utility.hpp"
  #include "parser.hpp"

  #define YY_USER_ACTION helper::lineNo = yylineno; \
      helper::columnNo += yyleng;

  using namespace yy::parser;
  using helper::yylval;

  void handleLexicalError(std::string msg, std::string err = "") {
      std::stringstream ss;
      ss << helper::moduleFileNames.back();
         << ':' << helper::lineNo 
         << ':' << helper::columnNo 
         << ':';
      ss << " lexical error: "
      ss << std::move(msg);
      ss << ": "
      ss << std::move(err)
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
 
<COMMENT>.*           { BEGIN(INITIAL); }

[ \t\r\v]+            /* игнорируем пробелы и переводы строк */
"\n"                  { helper::columnNo = 1; }

";"                   { return token_type::SC; }
":"                   { return token_type::COLON; }
"="                   { return token_type::EQ; }
"/="                  { return token_type::NEQ; }
":="                  { return token_type::ASG; }
"("                   { return token_type::LPAR; }
")"                   { return token_type::RPAR;}
"."                   { return token_type::DOT; }
"&"                   { return token_type::AMPER; }
"'"                   { return token_type::APOSTRl; }
","                   { return token_type::COMMA; }

"if"                  { return token_type::IF; }
"then"                { return token_type::THEN; }
"else"                { return token_type::ELSE; }
"elsif"               { return token_type::ELSIF; }

"for"                 { return token_type::FOOR; }
"loop"                { return token_type::LOOP; }
"in"                  { return token_type::IN; }
"exit"                { return token_type::EXIT; }
"when"                { return token_type::WHEN; }
"while"               { return token_type::WHILE; }

"with"                { return token_type::WHITH; }
"use"                 { return token_type::USE; }

"procedure"           { return token_type::PROCEDURE; }
"function"            { return token_type::FUNCTION; }
"begin"               { return token_type::BEGIN; }
"is"                  { return token_type::IS; }
"end"                 { return token_type::END; }
"return"              { return token_type::RETURN; }

"package"             { return token_type::PACKAGE; } 
"body"                { return token_type::BODY; } 
"type"                { return token_type::TYPE; } 
"tagged"              { return token_type::TAGGED; } 
"record"              { return token_type::RECORD; } 
"overriding"          { return token_type::OVERRIDING; } 
"new"                 { return token_type::NEW; } 

"Integer"             { return token_type::INTEGERTY; } 
"String"              { return token_type::STRINGTY; } 
"Character"           { return token_type::CHARACTERTY; }
"Float"               { return token_type::FLOATTY; } 
"Boolean"             { return token_type::BOOLTY; } 
      
{INTEGER}             { 
                        std::string text(yytext, yylen);
                        utility::replaceAll(text, "_", "");
                        int res = std::stoi(text);
                        yylval->emplace<int>(res)
                        return token_type::INTEGER;
                      }
{BINARY}              { 
                        std::string_view sv(yytext, yylen);
                        sv.remove_prefix(2);
                        sv.remove_suffix(1);
                        std::string text(sv.begin(), sv.end());
                        utility::replaceAll(text, "_", "");
                        int res = std::stoi(text, nullptr, 2);
                        yylval->emplace<int>(res)
                        return token_type::INTEGER;
                      }
{OCT}                 { 
                        std::string_view sv(yytext, yylen);
                        sv.remove_prefix(2);
                        sv.remove_suffix(1);
                        std::string text(sv.begin(), sv.end());
                        utility::replaceAll(text, "_", "");
                        int res = std::stoi(text, nullptr, 8);
                        yylval->emplace<int>(res)
                        return token_type::INTEGER;
                      }
{HEX}                 { 
                        std::string_view sv(yytext, yylen);
                        sv.remove_prefix(3);
                        sv.remove_suffix(1);
                        std::string text(sv.begin(), sv.end());
                        utility::replaceAll(text, "_", "");
                        int res = std::stoi(text, nullptr, 16);
                        yylval->emplace<int>(res)
                        return token_type::INTEGER;
                      }
{FLOAT}               { 
                        std::string text(yytext, yylen);
                        utility::replaceAll(text, "_", "");
                        float res = std::stof(text);
                        yylval->emplace<float>(res)
                        return token_type::FLOAT;
                      }
{CHAR}                { 
                        yylval->emplace<char>(yytext[1])
                        return token_type::CHAR;
                      }
{STRING}              { 
                        std::string_view sv(yytext, yylen);
                        sv.remove_prefix(1);
                        sv.remove_suffix(1);
                        std::string res(sv.begin(), sv.end());
                        utility::replaceAll(res, "\"\"", "\"");
                        yylval->emplace<char>(res);
                        return token_type::STRING;
                      }
                      
"False"               { return token_type::FALSE; }
"True"                { return token_type::TRUE; }
"null"                { return token_type::NULL; }

"+"                   { return token_type::PLUS;} 
"-"                   { return token_type::MINUS;} 
"/"                   { return token_type::DIV;} 
"*"                   { return token_type::MUL;} 
"<"                   { return token_type::MORE;} 
">"                   { return token_type::LESS;} 

"or"                  { return token_type::OR; } 
"not"                 { return token_type::NOT; } 
"and"                 { return token_type::AND; } 

      
{NAME}                { 
                        std::string text(yytext, yylen);
                        yylval->emplace<std::string>(std::move(text));
                        return token_type::NAME;
                      }
{ATTRIBUTE_CALL}      { 
                        std::string text(yytext, yyleng);
                        std::regex re(R"(([a-zA-Z_][a-zA-Z0-9_]*)\s*'\s*([a-zA-Z_][a-zA-Z0-9_]*))");
                        std::smatch match;
                        std::regex_match(text, match, re);
                        std::pair<std::string, std::string> res{
                          match[1]; 
                          match[2];
                        };
                        yylval->
                          emplace<std::pair<std::string, std::string>>(
                            std::move(res));
                        return token_type::ATTRIBUTE_CALL;
                        
                      }

{BAD_CHAR_EMPTY}      { handleLexicalError("Empty char") }
{BAD_CHAR_LONG}       { handleLexicalError("Bad char long", std::string(yytext, yylen)); }
{BAD_CHAR_UNTERM}     { handleLexicalError("Bad char unterm", std::string(yytext, yylen)); }
{BAD_STRING_LINE_FEED} { handleLexicalError("Bad string with LF", std::string(yytext, yylen)); }
{BAD_STRING_NO_CLOSE_DOUBLE_QUOTE} { handleLexicalError(
                                      "Bad string no close double quote",
                                       std::string(yytext, yylen));
                                   }

.                     { handleLexicalError("Unknown input text", std::string(yytext, yylen)); }

%%
