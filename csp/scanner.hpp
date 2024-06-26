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

#ifndef CSP_SCANNER_HPP
#define CSP_SCANNER_HPP

#include <iostream>
#include <memory>

#include "context.hpp"
#include "parser.hpp"

namespace CSP {

   typedef parser::semantic_type semantic_type;

   class Scanner {
      public:
	 Scanner(Context& context,
	       std::istream& in, const std::string& input_name);

	 // mutators
	 int get_token(semantic_type& yylval, location& yylloc);
	 bool at_eof() const;

	 const std::string& get_line(std::size_t ln) const {
	    if (ln >= 1 && ln <= lines.size()) {
	       return lines[ln-1];
	    } else {
	       return line;
	    }
	 }

      private:
	 Context& context;
	 std::istream& in;
	 std::string input_name;
	 unsigned char ch = 0;
	 bool eof = false;
	 position oldpos, pos;
	 location tokenloc;
	 std::unique_ptr<std::string> tokenstr;
	 std::vector<std::string> lines; // input lines
	 std::string line; // current line
	 std::size_t line_index = 1;

	 // private mutators
	 void nextch();
	 void error(char const* msg);
   };

   inline int yylex(semantic_type* yylval, location* yylloc, Context& context) {
      return context.scanner().get_token(*yylval, *yylloc);
   }

} // namespace CSP

#endif
