%option c++
%option yylineno

%{
  #include <string>    
  #include <string_view>
  #include <regex>
  #include <sstream>

  #include "string_utility.hpp"
  #include "helper.hpp"
  #include "parser.hpp"
                                    
  #define YY_USER_ACTION  helper::last_line = helper::first_line = yylineno; \
                          helper::first_column = helper::last_column; \
                          helper::last_column += yyleng;

                          

  using helper::yylval;

  void handleLexicalError(std::string msg, std::string err = "") {
      std::stringstream ss;
      ss << helper::curModuleFileName
         << ':' << helper::first_line 
         << ':' << helper::first_column
         << ':';
      ss << " lexical error: ";
      ss << std::move(msg);
      ss << ": ";
      ss << std::move(err);
      helper::errs.push_back(ss.str());
  }
%}

NAME          (?i:[a-zA-Z_][a-zA-Z0-9_]*)
DIGIT         [0-9]
HEXDIGIT      [0-9A-Fa-f]
INTEGER       {DIGIT}(_?{DIGIT})*
BINARY        2#[01](_?[01])*#
OCT           8#[0-7](_?[0-7])*#
HEX           16#{HEXDIGIT}(_?{HEXDIGIT})*#
FLOAT         {INTEGER}\.{INTEGER}([Ee][+-]?{INTEGER})?


CHAR          \'([^\'\n])\'
STRING        \"([^\"\n]|\"\")*\"

GETTING_ATTRIBUTE    {NAME}\s*\'\s*{NAME}
BAD_CHAR_EMPTY     \'\'
BAD_CHAR_LONG      \'([^\'\n]{2,})\'
BAD_CHAR_UNTERM    \'([^\'\n]+)
BAD_STRING_NO_CLOSE_DOUBLE_QUOTE \"([^\"\n]|\"\")*\n?
BAD_STRING_LINE_FEED \"(([^\"]|\"\")*\n+([^\"]|\"\")*)*\"


%x COMMENT


%%

"--"                  { BEGIN(COMMENT); }   /* вошли в со стояние COMMENT */
 
<COMMENT>.*           { BEGIN(INITIAL); }

[ \t\r\v]+            /* игнорируем пробелы и переводы строк */
"\n"                  { helper::first_column = helper::last_column = 1; }

";"                   { return yy::parser::token_type::SC; }
":"                   { return yy::parser::token_type::COLON; }
".."                  { return yy::parser::token_type::DOT_DOT; }
":="                  { return yy::parser::token_type::ASG; }
"("                   { return yy::parser::token_type::LPAR; }
")"                   { return yy::parser::token_type::RPAR;}
"."                   { return yy::parser::token_type::DOT; }
"&"                   { return yy::parser::token_type::AMPER; }
"'"                   { return yy::parser::token_type::APOSTR; }
","                   { return yy::parser::token_type::COMMA; }

(?i:"if")             { return yy::parser::token_type::IF; }
(?i:"then")           { return yy::parser::token_type::THEN; }
(?i:"else")           { return yy::parser::token_type::ELSE; }
(?i:"elsif")          { return yy::parser::token_type::ELSIF; }

(?i:"array")          { return yy::parser::token_type::ARRAY; } 
(?i:"in")             { return yy::parser::token_type::IN; } 
(?i:"out")            { return yy::parser::token_type::OUT; } 
(?i:"of")             { return yy::parser::token_type::OF; } 
(?i:"for")            { return yy::parser::token_type::FOR; }
(?i:"loop")           { return yy::parser::token_type::LOOP; }
(?i:"exit")           { return yy::parser::token_type::EXIT; }
(?i:"when")           { return yy::parser::token_type::WHEN; }
(?i:"while")          { return yy::parser::token_type::WHILE; }

(?i:"with")           { return yy::parser::token_type::WITH; }
(?i:"use")            { return yy::parser::token_type::USE; }

(?i:"procedure")      { return yy::parser::token_type::PROCEDURE; }
(?i:"function")       { return yy::parser::token_type::FUNCTION; }
(?i:"begin")          { return yy::parser::token_type::BEGIN_KW; }
(?i:"private")        { return yy::parser::token_type::PRIVATE; }
(?i:"is")             { return yy::parser::token_type::IS; }
(?i:"end")            { return yy::parser::token_type::END; }
(?i:"return")         { return yy::parser::token_type::RETURN; }

(?i:"package")        { return yy::parser::token_type::PACKAGE; } 
(?i:"body")           { return yy::parser::token_type::BODY; } 
(?i:"type")           { return yy::parser::token_type::TYPE; } 
(?i:"tagged")         { return yy::parser::token_type::TAGGED; } 
(?i:"record")         { return yy::parser::token_type::RECORD; } 
(?i:"overriding")     { return yy::parser::token_type::OVERRIDING; } 
(?i:"new")            { return yy::parser::token_type::NEW; } 

(?i:"Integer")        { return yy::parser::token_type::INTEGERTY; } 
(?i:"String")         { return yy::parser::token_type::STRINGTY; } 
(?i:"Character")      { return yy::parser::token_type::CHARACTERTY; }
(?i:"Float")          { return yy::parser::token_type::FLOATTY; } 
(?i:"Boolean")        { return yy::parser::token_type::BOOLTY; } 
    
{INTEGER}             { 
                        std::string text(yytext, yyleng);
                        utility::toLower(text);
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
                        utility::toLower(text);
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
                        utility::toLower(text);
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
                        utility::toLower(text);
                        int res = std::stoi(text, nullptr, 16);
                        yylval->emplace<int>(res);
                        return yy::parser::token_type::INTEGER;
                      }
{FLOAT}               { 
                        std::string text(yytext, yyleng);
                        utility::replaceAll(text, "_", "");
                        utility::toLower(text);
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
                      
(?i:"False")          { 
                        yylval->emplace<bool>(false); 
                        return yy::parser::token_type::BOOL; 
                      }
(?i:"True")           { 
                        yylval->emplace<bool>(true); 
                        return yy::parser::token_type::BOOL;
                      }
(?i:"null")           { return yy::parser::token_type::NULL_KW; }

"+"                   { return yy::parser::token_type::PLUS;} 
"-"                   { return yy::parser::token_type::MINUS;} 
"/"                   { return yy::parser::token_type::DIV;} 
"*"                   { return yy::parser::token_type::MUL;} 
"<"                   { return yy::parser::token_type::LESS;} 
">"                   { return yy::parser::token_type::MORE;}
"="                   { return yy::parser::token_type::EQ; }
"/="                  { return yy::parser::token_type::NEQ; }
">="                  { return yy::parser::token_type::GTE; }
"<="                  { return yy::parser::token_type::LTE; }


(?i:"or")             { return yy::parser::token_type::OR; } 
(?i:"not")            { return yy::parser::token_type::NOT; } 
(?i:"and")            { return yy::parser::token_type::AND; } 
(?i:"xor")            { return yy::parser::token_type::XOR; } 

      
{NAME}                { 
                        std::string text(yytext, yyleng);
                        utility::toLower(text);
                        yylval->emplace<std::string>(std::move(text));
                        return yy::parser::token_type::NAME;
                      }
{GETTING_ATTRIBUTE}      { 
                        std::string text(yytext, yyleng);
                        std::regex re(R"(([a-zA-Z_][a-zA-Z0-9_]*)\s*'\s*([a-zA-Z_][a-zA-Z0-9_]*))");
                        std::smatch match;
                        std::regex_match(text, match, re);
                        std::pair<std::string, std::string> res(match[1], match[2]);
                        utility::toLower(res.first);
                        utility::toLower(res.second);
                        yylval->
                          emplace<std::pair<std::string, std::string>>(std::move(res));
                        return yy::parser::token_type::GETTING_ATTRIBUTE;
                      }

{BAD_CHAR_EMPTY}      { handleLexicalError("Empty char"); return yy::parser::token_type::ERR; }
{BAD_CHAR_LONG}       { handleLexicalError("Bad char long", std::string(yytext, yyleng)); return yy::parser::token_type::ERR; }
{BAD_CHAR_UNTERM}     { handleLexicalError("Bad char unterm", std::string(yytext, yyleng)); return yy::parser::token_type::ERR; }
{BAD_STRING_LINE_FEED} { handleLexicalError("Bad string with LF", std::string(yytext, yyleng)); return yy::parser::token_type::ERR; }
{BAD_STRING_NO_CLOSE_DOUBLE_QUOTE} { handleLexicalError(
                                      "Bad string no close double quote",
                                       std::string(yytext, yyleng)); 
                                       return yy::parser::token_type::ERR; 
                                   }

.                     { handleLexicalError("Unknown input text", std::string(yytext, yyleng)); return yy::parser::token_type::ERR; }

%%
