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

#include <cstdlib>
#include <iostream>
#include "error.hpp"

namespace CSP {

void yyerror(location const* loc, char const* msg) {
   /* we have to output locations ourselves as the corresponding
      output operator as provided by bison is broken */
   auto filename = loc->begin.filename;
   if (!filename) {
      filename = loc->end.filename;
   }
   if (filename) {
      std::cerr << *filename << ":";
   }
   auto end_col = loc->end.column > 0? loc->end.column - 1: 0;
   if (loc->begin.line < loc->end.line) {
      std::cerr << loc->begin.line << ':' << loc->begin.column <<
	 '-' << loc->end.line << ':' << loc->end.column;
   } else if (loc->begin.line < end_col) {
      std::cerr << loc->begin.line << ':' << loc->begin.column <<
	 '-' << end_col;
   } else {
      std::cerr << loc->begin.line << ':' << loc->begin.column;
   }
   std::cerr << ": " << msg << std::endl;
   exit(1);
}

void parser::error(const location_type& loc, const std::string& msg) {
   yyerror(&loc, msg.c_str());
}

} // namespace CSP
