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
#include <cstring>
#include <ctime>
#include <fstream>
#include <iostream>
#include <tuple>
#include <unistd.h>

#include "context.hpp"
#include "parser.hpp"
#include "process.hpp"
#include "scanner.hpp"
#include "status.hpp"
#include "symtable.hpp"

using namespace CSP;

void usage(const char* cmdname) {
   std::cerr << "Usage: " << cmdname << " [-Aaepv] [-P n] source.csp" <<
      std::endl;
   std::cerr << "Options:" << std::endl;
   std::cerr << " -A   print alphabet, one symbol per line, and exit" <<
      std::endl;
   std::cerr << " -a   do not print the alphabet at the beginning" << std::endl;
   std::cerr << " -e   print events, if accepted" << std::endl;
   std::cerr << " -p   do not print current process after each event" <<
      std::endl;
   std::cerr << " -P n chose event by random and stop after n events" <<
      std::endl;
   std::cerr << " -v   do not print the set of acceptable events" <<
      std::endl;
   std::exit(1);
}

int main(int argc, char** argv) {
   const char* cmdname = *argv++; --argc;
   if (argc == 0) usage(cmdname);

   // permit verbose process output to be suppressed
   bool opt_A = false; // print alphabet and exit
   bool opt_a = true;  // print alphabet at the beginning
   bool opt_e = false; // print events if accepted
   bool opt_p = true;  // print current process after each event
   bool opt_P = false; // chose event by random and stop after n events
   unsigned int event_count = 0; // parameter of -P
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
	    case 'P':
	       {
		  opt_P = true;
		  char* arg = cp+1;
		  if (!*arg) {
		     --argc; ++argv;
		     if (argc == 0) usage(cmdname);
		     arg = *argv;
		  }
		  char* endptr;
		  event_count = std::strtoul(arg, &endptr, 10);
		  if (*endptr) usage(cmdname);
		  cp = endptr-1;
	       }
	       break;
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
      std::exit(1);
   }
   std::string filename(fname);
   Context context;
   Scanner scanner(context, fin, filename);
   SymTable symtab(context);

   ProcessPtr process;
   parser p(context, process);
   if (p.parse() == 0 && context.get_error_count() == 0) {
      if (opt_A) {
	 auto alphabet = process->get_alphabet();
	 for (auto event: alphabet) {
	    std::cout << event << std::endl;
	 }
	 std::exit(0);
      }
      auto status = std::make_shared<Status>();
      if (opt_p) {
	 std::cout << "Tracing: " << process << std::endl;
      }
      if (opt_a) {
	 std::cout << "Alphabet: " << process->get_alphabet() << std::endl;
      }
      if (opt_v) {
	 std::cout << "Acceptable: " <<
	    process->acceptable(status) << std::endl;
      }
      if (!process->accepts_success(status)) {
	 std::string event;
	 auto fetch_event = [&]() -> bool {
	    if (opt_P) {
	       if (event_count == 0) return false;
	       --event_count;
	       auto acceptable = process->acceptable(status);
	       if (acceptable.cardinality() == 0) return false;
	       auto chose = status->draw(acceptable.cardinality());
	       event = *std::next(acceptable.begin(), chose);
	       return true;
	    } else {
	       return !!(std::cin >> event);
	    }
	 };
	 while (fetch_event()) {
	    if (process->get_alphabet().is_member(event)) {
	       std::tie(process, status) = process->proceed(event, status);
	       if (!process) {
		  std::cerr << "cannot accept " << event << std::endl;
		  std::exit(1);
	       }
	       if (process->accepts_success(status)) break;
	       if (opt_e) {
		  std::cout << event << std::endl;
	       }
	       if (opt_p) {
		  std::cout << "Process: " << process << std::endl;
	       }
	       if (opt_v) {
		  std::cout << "Acceptable: " <<
		     process->acceptable(status) << std::endl;
	       }
	    } else {
	       std::cout << "Not in alphabet: " << event << std::endl;
	    }
	 }
      }
      std::cout << "OK" << std::endl;
   } else {
      std::exit(1);
   }
}
