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
#include "named-process.hpp"
#include "symtable.hpp"

namespace CSP {

   class ProcessReference: public NamedProcess {
      public:
	 ProcessReference(const std::string& name, SymTable& symtab) :
	       NamedProcess(name), symtab(symtab) {
	 }

	 void register_ref() {
	    if (!resolve()) {
	       symtab.add_unresolved(get_name(),
		  [this]() {
		     return this->resolve();
		  });
	    }
	 }
	 bool resolve() const {
	    if (p) return true;
	    p = symtab.lookup<NamedProcess>(get_name());
	    if (!p) return false;
	    for (auto c: channels) {
	       p->add_channel(c);
	       channels.clear();
	    }
	    return true;
	 }
	 void print(std::ostream& out) const override {
	    out << get_name();
	 }
	 void expanded_print(std::ostream& out) const override {
	    if (p) {
	       p->print(out);
	    } else {
	       out << get_name();
	    }
	 }
	 Alphabet acceptable() const final {
	    if (!get_alphabet()) {
	       /* if our alphabet is empty, we simply return
		  the empty set; this test avoids an endless
		  recursion in case of constructs of the type
		     P = P
	       */
	       return Alphabet();
	    }
	    if (!p) resolve();
	    if (p) {
	       return p->acceptable();
	    } else {
	       return Alphabet();
	    }
	 }

	 void add_channel(ChannelPtr c) override {
	    if (p || resolve()) {
	       p->add_channel(c);
	    } else {
	       channels.push_back(c);
	    }
	 }

      private:
	 SymTable& symtab;
	 mutable NamedProcessPtr p;
	 mutable std::deque<ChannelPtr> channels;

	 ProcessPtr internal_proceed(const std::string& event) final {
	    if (p) {
	       return p->proceed(event);
	    } else {
	       return nullptr;
	    }
	 }
	 Alphabet internal_get_alphabet() const final {
	    if (!p) resolve();
	    if (p) {
	       return p->get_alphabet();
	    } else {
	       return Alphabet();
	    }
	 }
	 void initialize_dependencies() const final {
	    if (!p) {
	       resolve();
	       if (!p) return;
	    }
	    p->add_dependant(std::dynamic_pointer_cast<const Process>(
	       Process::shared_from_this()));
	 }
   };

} // namespace CSP

#endif
