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

/*
   A process definition object behaves like its right-hand-side process
   but it remembers its left-hand-side name for printing
*/

#ifndef CSP_PROCESS_REFERENCE_HPP
#define CSP_PROCESS_REFERENCE_HPP

#include <iostream>
#include <memory>
#include <string>

namespace CSP {
   class ProcessReference;
   using ProcessReferencePtr = std::shared_ptr<ProcessReference>;
}

#include "alphabet.hpp"
#include "process.hpp"
#include "symtable.hpp"

namespace CSP {

   class ProcessReference: public Process {
      public:
	 ProcessReference(const std::string& name, SymTable& symtab) :
	       name(name), symtab(symtab) {
	 }
	 const std::string& get_name() const {
	    return name;
	 }
	 void register_ref() {
	    if (!resolve()) {
	       symtab.add_unresolved(
		  std::dynamic_pointer_cast<ProcessReference>(
		     shared_from_this()));
	    }
	 }
	 bool resolve() const {
	    if (p) return true;
	    if (!symtab.lookup(name, p)) return false;
	    return true;
	 }
	 virtual void print(std::ostream& out) const {
	    out << name;
	 }
	 virtual void expanded_print(std::ostream& out) const {
	    if (!p) resolve();
	    if (p) {
	       p->print(out);
	    } else {
	       out << name;
	    }
	 }
	 virtual Alphabet acceptable() const {
	    if (!p) resolve();
	    if (p) {
	       return p->acceptable();
	    } else {
	       return Alphabet();
	    }
	 }
      protected:
	 virtual ProcessPtr internal_proceed(std::string& event) {
	    if (p) {
	       return p->proceed(event);
	    } else {
	       return nullptr;
	    }
	 }
	 virtual Alphabet internal_get_alphabet() const {
	    if (!p) resolve();
	    if (p) {
	       return p->get_alphabet();
	    } else {
	       return Alphabet();
	    }
	 }
      private:
	 const std::string name;
	 SymTable& symtab;
	 mutable ProcessPtr p;
	 virtual void initialize_dependencies() const {
	    if (!p) {
	       resolve();
	       if (!p) return;
	    }
	    p->add_dependant(std::dynamic_pointer_cast<const Process>(
	       shared_from_this()));
	 }
   };

} // namespace CSP

#endif
