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

#include <cstdlib>
#include <fstream>
#include <iostream>
#include <memory>

#include "context.hpp"
#include "parser.hpp"
#include "scanner.hpp"

using namespace CSP;

int main(int argc, char** argv) {
   char* cmdname = *argv++; --argc;
   if (argc > 1) {
      std::cerr << "Usage: " << cmdname << " [filename]" << std::endl;
      std::exit(1);
   }

   std::unique_ptr<Scanner> scanner;
   std::unique_ptr<std::ifstream> fin = nullptr;
   Context context;
   if (argc > 0) {
      char* fname = *argv++; --argc;
      fin = std::make_unique<std::ifstream>(fname);
      std::string filename(fname);
      if (!*fin) {
	 std::cerr << cmdname << ": unable to open " << fname <<
	    " for reading" << std::endl;
	 std::exit(1);
      }
      scanner = std::make_unique<Scanner>(context, *fin, filename);
   } else {
      scanner = std::make_unique<Scanner>(context, std::cin, "stdin");
   }
   semantic_type yylval;
   int token;
   location loc;
   while ((token = scanner->get_token(yylval, loc)) != 0) {
      std::cout << token;
      if (yylval) {
	 std::cout << " \"" << yylval << '"';
	 yylval = ObjectPtr();
      }
      std::cout << std::endl;
   }
}
