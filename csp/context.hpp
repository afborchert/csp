/* 
   Copyright (c) 2011-2023 Andreas F. Borchert
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

#ifndef CSP_CONTEXT_HPP
#define CSP_CONTEXT_HPP

#include <cassert>

namespace CSP {

   class Scanner;
   class SymTable;

   class Context {
      public:
	 void set_scanner(Scanner& scanner) {
	    assert(!scanner_ptr);
	    scanner_ptr = &scanner;
	 }
	 void set_symtab(SymTable& symtab) {
	    assert(!symtab_ptr);
	    symtab_ptr = &symtab;
	 }
	 Scanner& scanner() {
	    return *scanner_ptr;
	 }
	 SymTable& symtab() {
	    return *symtab_ptr;
	 }

	 unsigned get_error_count() const {
	    return error_count;
	 }
	 void increase_error_count() {
	    ++error_count;
	 }

      private:
	 Scanner* scanner_ptr = nullptr;
	 SymTable* symtab_ptr = nullptr;
	 unsigned error_count = 0;
   };

} // namespace CSP

#endif
