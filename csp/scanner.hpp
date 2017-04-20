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

#ifndef CSP_SCANNER_HPP
#define CSP_SCANNER_HPP

#include <iostream>
#include <memory>
#include "parser.hpp"
#include "symtable.hpp"

namespace CSP {

   typedef parser::semantic_type semantic_type;

   class Scanner {
      public:
	 Scanner(std::istream& in, const std::string& input_name,
	    SymTable& symtab);

	 // mutators
	 int get_token(semantic_type& yylval, location& yylloc);

      private:
	 SymTable& symtab;
	 std::istream& in;
	 std::string input_name;
	 unsigned char ch;
	 bool eof;
	 int lasttoken; // last token returned by get_token()
	 position oldpos, pos;
	 location tokenloc;
	 std::unique_ptr<std::string> tokenstr;

	 // private mutators
	 void nextch();
	 void error(char const* msg);
   };

   inline int yylex(semantic_type* yylval, location* yylloc, Scanner& scanner) {
      return scanner.get_token(*yylval, *yylloc);
   }

} // namespace CSP

#endif
