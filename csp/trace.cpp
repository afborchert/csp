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
#include <cstring>
#include <ctime>
#include <fstream>
#include <iostream>
#include <unistd.h>
#include "parser.hpp"
#include "scanner.hpp"
#include "symtable.hpp"
#include "yytname.hpp"

using namespace CSP;

int main(int argc, char** argv) {
   char* cmdname = *argv++; --argc;

   // permit verbose process output to be suppressed
   bool opt_p = true;
   if (argc > 0 && strcmp(argv[0], "-p") == 0) {
      opt_p = false; --argc; ++argv;
   }

   if (argc != 1) {
      std::cerr << "Usage: " << cmdname << " [-p] filename" << std::endl;
      exit(1);
   }

   char* fname = *argv++; --argc;
   std::ifstream fin(fname);
   if (!fin) {
      std::cerr << cmdname << ": unable to open " << fname <<
	 " for reading" << std::endl;
      exit(1);
   }
   SymTable symtab;
   std::string filename(fname);
   Scanner scanner(fin, filename, symtab);

   /* initialize pseudo-randomgenerator */
   std::srand(time(0) ^ getpid());

   ProcessPtr process;
   parser p(scanner, symtab, process);
   if (p.parse() == 0) {
      if (opt_p) std::cout << "Tracing: " << process << std::endl;
      std::cout << "Alphabet: " << process->get_alphabet() << std::endl;
      std::cout << "Acceptable: " << process->acceptable() << std::endl;
      if (!process->accepts_success()) {
	 std::string event;
	 while (std::cin >> event) {
	    if (process->get_alphabet().is_member(event)) {
	       process = process->proceed(event);
	       if (!process) {
		  std::cerr << "cannot accept " << event << std::endl;
		  exit(1);
	       }
	       if (process->accepts_success()) break;
	       if (opt_p) std::cout << "Process: " << process << std::endl;
	       std::cout << "Acceptable: " << process->acceptable() <<
		  std::endl;
	    } else {
	       std::cout << "Not in alphabet: " << event << std::endl;
	    }
	 }
      }
      std::cout << "OK" << std::endl;
   }
}
