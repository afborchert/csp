/* 
   Copyright (c) 2011-2022 Andreas F. Borchert
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

#ifndef CSP_RECURSIVE_PROCESS_HPP
#define CSP_RECURSIVE_PROCESS_HPP

#include <iostream>
#include <memory>
#include <string>

#include "alphabet.hpp"
#include "named-process.hpp"

namespace CSP {

   class RecursiveProcess;
   using RecursiveProcessPtr = std::shared_ptr<RecursiveProcess>;

   class RecursiveProcess: public NamedProcess {
      public:
	 RecursiveProcess(const std::string& name) :
	       NamedProcess(name) {
	 }
	 RecursiveProcess(const std::string& name, Alphabet alphabet) :
	       NamedProcess(name), explicit_alphabet(true) {
	    set_alphabet(alphabet);
	 }
	 RecursiveProcess(const std::string& name, ProcessPtr p_alphabet) :
	       NamedProcess(name), p_alphabet(p_alphabet) {
	 }

	 void print(std::ostream& out) const override {
	    out << "mu " << get_name();
	    if (explicit_alphabet) {
	       out << ":" << get_alphabet();
	    }
	    if (process) {
	       out << "."; process->print(out);
	    }
	 }

	 void set_process(ProcessPtr p) {
	    assert(!process); assert(p);
	    process = p;
	 }

	 Alphabet acceptable(Bindings& bindings) const final {
	    return process->acceptable(bindings);
	 }

      private:
	 ProcessPtr internal_proceed(const std::string& event,
	       Bindings& bindings) final {
	    return process->proceed(event, bindings);
	 }
	 Alphabet internal_get_alphabet() const final {
	    if (p_alphabet) {
	       return p_alphabet->get_alphabet();
	    } else {
	       return process->get_alphabet();
	    }
	 }

	 void initialize_dependencies() const final {
	    if (p_alphabet) {
	       p_alphabet->add_dependant(
		  std::dynamic_pointer_cast<const Process>(shared_from_this()));
	    }
	 }

	 bool explicit_alphabet = false;
	 ProcessPtr p_alphabet;
	 ProcessPtr process;
   };

} // namespace CSP

#endif
