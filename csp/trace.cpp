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
#include "process.hpp"
#include "scanner.hpp"
#include "symtable.hpp"
#include "yytname.hpp"

using namespace CSP;

void usage(const char* cmdname) {
   std::cerr << "Usage: " << cmdname << " [-Aaepv] source.csp" << std::endl;
   std::cerr << "Options:" << std::endl;
   std::cerr << " -A   print alphabet, one symbol per line, and exit" <<
      std::endl;
   std::cerr << " -a   do not print the alphabet at the beginning" << std::endl;
   std::cerr << " -e   print events, if accepted" << std::endl;
   std::cerr << " -p   do not print current process after each event" <<
      std::endl;
   std::cerr << " -v   do not print the set of acceptable events" <<
      std::endl;
   exit(1);
}

int main(int argc, char** argv) {
   const char* cmdname = *argv++; --argc;
   if (argc == 0) usage(cmdname);

   // permit verbose process output to be suppressed
   bool opt_A = false; // print alphabet and exit
   bool opt_a = true;  // print alphabet at the beginning
   bool opt_e = false; // print events if accepted
   bool opt_p = true;  // print current process after each event
   bool opt_v = true;  // print current set of acceptable events
   while (argc > 0 && **argv == '-') {
      for (char* cp = *argv + 1; *cp; ++cp) {
	 switch (*cp) {
	    case 'A':
	       opt_A = true; break;
	    case 'a':
	       opt_a = false; break;
	    case 'e':
	       opt_e = true; break;
	    case 'p':
	       opt_p = false; break;
	    case 'v':
	       opt_v = false; break;
	    default:
	       usage(cmdname); break;
	 }
      }
      --argc; ++argv;
   }
   if (argc != 1) usage(cmdname);

   const char* fname = *argv++; --argc;
   std::ifstream fin(fname);
   if (!fin) {
      std::cerr << cmdname << ": unable to open " << fname <<
	 " for reading" << std::endl;
      exit(1);
   }
   SymTable symtab;
   std::string filename(fname);
   Scanner scanner(fin, filename, symtab);

   ProcessPtr process;
   parser p(scanner, symtab, process);
   if (p.parse() == 0) {
      if (opt_A) {
	 auto alphabet = process->get_alphabet();
	 for (auto event: alphabet) {
	    std::cout << event << std::endl;
	 }
	 exit(0);
      }
      if (opt_p) {
	 std::cout << "Tracing: " << process << std::endl;
      }
      if (opt_a) {
	 std::cout << "Alphabet: " << process->get_alphabet() << std::endl;
      }
      if (opt_v) {
	 std::cout << "Acceptable: " << process->acceptable() << std::endl;
      }
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
	       if (opt_e) {
		  std::cout << event << std::endl;
	       }
	       if (opt_p) {
		  std::cout << "Process: " << process << std::endl;
	       }
	       if (opt_v) {
		  std::cout << "Acceptable: " << process->acceptable() <<
		     std::endl;
	       }
	    } else {
	       std::cout << "Not in alphabet: " << event << std::endl;
	    }
	 }
      }
      std::cout << "OK" << std::endl;
   }
}
