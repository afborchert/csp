/* 
   Copyright (c) 2011-2024 Andreas F. Borchert
   All rights reserved.

   Permission is hereby granted, free of charge, to any person obtaining
   a copy of this software and associated documentation files (the
   "Software"), to deal in the Software without restriction, including
   without limitation the rights to use, copy, modify, merge, publish,
   distribute, sublicense, and/or sell copies of the Software, and to
   permit persons to whom the Software is furnished to do so, subject to
   the following conditions:

   The above copyright notice and this permission notice shall be
   included in all copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY
   KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
   WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
   NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
   BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
   ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
   CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
   SOFTWARE.
*/

#include "error.hpp"
#include "identifier.hpp"
#include "object.hpp"
#include "process.hpp"
#include "process-reference.hpp"
#include "scanner.hpp"

namespace CSP {

// private functions =========================================================

bool is_letter(char ch) {
   return ((ch >= 'a') && (ch <= 'z')) ||
      ((ch >= 'A') && (ch <= 'Z')) || ch == '_';
}

bool is_lower(char ch) {
   return (ch >= 'a') && (ch <= 'z');
}

bool is_upper(char ch) {
   return (ch >= 'A') && (ch <= 'Z');
}

bool is_digit(char ch) {
   return (ch >= '0') && (ch <= '9');
}

bool is_whitespace(char ch) {
   return ch == ' ' || ch == '\t' || ch == '\r' || ch == '\n' ||
      ch == '\f' || ch == '\v';
}

// constructor ===============================================================

Scanner::Scanner(Context& context, std::istream& in,
	 const std::string& input_name) :
      context(context), in(in), input_name(input_name) {
   context.set_scanner(*this);
   // unfortunately pos insists on a non-const pointer to std::string
   pos.initialize(&this->input_name);
   nextch();
}

// mutator ===================================================================

int Scanner::get_token(semantic_type& yylval, location& yylloc) {
   int token = 0;
   yylval = ObjectPtr();

restart:
   for(;;) {
      if (eof) {
	 break;
      } else if (is_whitespace(ch)) {
	 nextch();
      } else {
	 break;
      }
   }
   tokenloc.begin = oldpos;
   if (is_letter(ch) || is_digit(ch)) {
      tokenstr = std::make_unique<std::string>();
      if (is_lower(ch) || is_digit(ch)) {
	 token = parser::token::LCIDENT;
      } else {
	 token = parser::token::UCIDENT;
      }
      while (is_letter(ch) || is_digit(ch)) {
	 nextch();
      }
      if (*tokenstr == "CHAOS") {
	 token = parser::token::CHAOS;
      } else if (*tokenstr == "RUN") {
	 token = parser::token::RUN;
      } else if (*tokenstr == "SKIP") {
	 token = parser::token::SKIP;
      } else if (*tokenstr == "STOP") {
	 token = parser::token::STOP;
      } else if (*tokenstr == "alpha") {
	 token = parser::token::ALPHA;
      } else if (*tokenstr == "mu") {
	 token = parser::token::MU;
      } else if (*tokenstr == "string") {
	 token = parser::token::STRING;
      } else if (*tokenstr == "integer") {
	 token = parser::token::INTEGER;
      } else if (*tokenstr == "div") {
	 token = parser::token::DIV;
      } else if (*tokenstr == "mod") {
	 token = parser::token::MOD;
      } else {
	 yylval = std::make_shared<Identifier>(*tokenstr);
      }
      tokenstr = nullptr;
   } else if (ch == '"') {
      tokenstr = std::make_unique<std::string>();
      nextch();
      while (ch != '"') {
	 nextch();
      }
      yylval = std::make_shared<Identifier>(*tokenstr);
      tokenstr = nullptr;
      nextch();
   } else {
      switch (ch) {
	 case 0:
	    /* eof */
	    break;
	 case '/':
	    nextch();
	    if (ch == '/') {
	       nextch();
	       token = parser::token::SUBORDINATION;
	       break;
	    } else if (ch == '*') {
	       /* delimited comment */
	       nextch();
	       bool star = false;
	       while (!eof && (!star || ch != '/')) {
		  star = ch == '*';
		  nextch();
	       }
	       if (eof) {
		  error("unexpected eof in delimited comment");
	       } else {
		  nextch();
	       }
	    } else {
	       error("invalid token");
	    }
	    goto restart;
	 // punctuators
	 case '(':
	    nextch(); token = parser::token::LPAREN; break;
	 case ')':
	    nextch(); token = parser::token::RPAREN; break;
	 case '{':
	    nextch(); token = parser::token::LBRACE; break;
	 case '}':
	    nextch(); token = parser::token::RBRACE; break;
	 case '=':
	    nextch(); token = parser::token::EQUAL; break;
	 case ',':
	    nextch(); token = parser::token::COMMA; break;
	 case ';':
	    nextch(); token = parser::token::SEMICOLON; break;
	 case ':':
	    nextch(); token = parser::token::COLON; break;
	 case '.':
	    nextch(); token = parser::token::PERIOD; break;
	 case '\\':
	    nextch(); token = parser::token::CONCEAL; break;
	 case '?':
	    nextch(); token = parser::token::QM; break;
	 case '!':
	    nextch(); token = parser::token::EM; break;
	 // operators in expressions
	 case '+':
	    nextch(); token = parser::token::PLUS; break;
	 case '*':
	    nextch(); token = parser::token::TIMES; break;
	 // composite punctuators
	 case '-':
	    nextch();
	    if (ch == '>') {
	       nextch(); token = parser::token::ARROW;
	    } else if (ch == '-') {
	       /* Ada-style single-line comment */
	       while (!eof && ch != '\n') {
		  nextch();
	       }
	       if (eof) {
		  error("unexpected eof in single-line comment");
	       }
	       goto restart;
	    } else {
	       nextch(); token = parser::token::MINUS;
	    }
	    break;
	 case '|':
	    nextch();
	    if (ch == '|') {
	       nextch();
	       if (ch == '|') {
		  nextch();
		  token = parser::token::INTERLEAVES;
	       } else {
		  token = parser::token::PARALLEL;
	       }
	    } else if (ch == '~') {
	       nextch();
	       if (ch == '|') {
		  nextch();
		  token = parser::token::INTERNAL_CHOICE;
	       } else {
		  error("invalid token");
	       }
	    } else {
	       token = parser::token::OR;
	    }
	    break;
	 case '[':
	    nextch();
	    if (ch == ']') {
	       nextch();
	       token = parser::token::EXTERNAL_CHOICE;
	    } else {
	       error("invalid token");
	    }
	    break;
	 case '>':
	    nextch();
	    if (ch == '>') {
	       nextch();
	       token = parser::token::PIPE;
	    } else {
	       error("invalid token");
	    }
	    break;
	 default:
	    nextch();
	    error("invalid token");
	    goto restart;
      }
   }

   yylloc = tokenloc;
   return token;
}

bool Scanner::at_eof() const {
   return eof;
}

// private methods ===========================================================

/*
 * get next character from the input stream, if available:
 *  - pos gets updated (
 */
void Scanner::nextch() {
   tokenloc.end = pos;
   oldpos = pos;
   if (eof) {
      ch = 0; return;
   }
   if (tokenstr) {
      *tokenstr += ch;
   }

   if (line_index > line.length()) {
      if (!std::getline(in, line)) {
	 eof = true; ch = 0; return;
      }
      line_index = 0;
   }
   if (line_index == line.length()) {
      ch = '\n';
   } else {
      ch = line[line_index];
   }
   ++line_index;

   if (ch == '\n') {
      pos.lines();
      lines.emplace_back(std::move(line));
      line.clear();
   } else if (ch == '\t') {
      unsigned blanks = 8 - (pos.column - 1) % 8;
      pos.columns(blanks);
   } else {
      pos.columns();
   }
}

void Scanner::error(char const* msg) {
   yyerror(tokenloc, context, msg);
}

} // namespace CSP
