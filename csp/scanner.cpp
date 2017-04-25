/* 
   Copyright (c) 2011-2017 Andreas F. Borchert
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
#include "event.hpp"
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

Scanner::Scanner(std::istream& in, const std::string& input_name,
	 SymTable& symtab) :
      symtab(symtab), in(in), input_name(input_name),
      ch(0), eof(false), tokenstr(nullptr) {
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
   if (is_letter(ch)) {
      tokenstr = std::make_unique<std::string>();
      if (is_lower(ch)) {
	 token = parser::token::EVENT;
      } else {
	 token = parser::token::PROCESS;
      }
      while (is_letter(ch) || is_digit(ch)) {
	 nextch();
      }
      if (*tokenstr == "STOP") {
	 token = parser::token::STOP;
      } else if (*tokenstr == "RUN") {
	 token = parser::token::RUN;
      } else if (*tokenstr == "SKIP") {
	 token = parser::token::SKIP;
      } else if (*tokenstr == "alpha") {
	 token = parser::token::ALPHA;
      } else if (token == parser::token::EVENT) {
	 yylval = std::make_shared<Event>(*tokenstr);
      } else {
	 auto p = std::make_shared<ProcessReference>(*tokenstr, symtab);
	 p->register_ref();
	 yylval = p;
      }
      tokenstr = nullptr;
   } else {
      switch (ch) {
	 case 0:
	    /* eof */
	    break;
	 case '/':
	    nextch();
	    if (ch == '/') {
	       /* single-line comment */
	       while (!eof && ch != '\n') {
		  nextch();
	       }
	       if (eof) {
		  error("unexpected eof in single-line comment");
	       }
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
	       error("invalid token");
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
	 default:
	    nextch();
	    error("invalid token");
	    goto restart;
      }
   }

   yylloc = tokenloc;
   return token;
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
   char c;
   if (!in.get(c)) {
      eof = true; ch = 0; return;
   }
   ch = c;
   if (ch == '\n') {
      pos.lines();
   } else {
      pos.columns();
   }
}

void Scanner::error(char const* msg) {
   yyerror(&tokenloc, msg);
}

} // namespace CSP
