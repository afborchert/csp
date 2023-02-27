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
   Extension of Process for selecting processes, i.e. process with
   the form P1 | P2
*/

#ifndef CSP_SELECTING_PROCESS_HPP
#define CSP_SELECTING_PROCESS_HPP

#include <cassert>
#include <deque>
#include <iostream>
#include <memory>
#include <string>
#include "alphabet.hpp"
#include "process.hpp"

namespace CSP {

   class SelectingProcess: public Process {
      public:
	 SelectingProcess(ProcessPtr choice) {
	    assert(choice);
	    choices.push_back(choice);
	 }
	 void add_choice(ProcessPtr choice) {
	    assert(choice);
	    choices.push_back(choice);
	 }
	 void print(std::ostream& out) const override {
	    bool first = true;
	    out << "(";
	    for (auto choice: choices) {
	       if (first) {
		  first = false;
	       } else {
		  out << " | ";
	       }
	       choice->print(out);
	    }
	    out << ")";
	 }
	 Alphabet acceptable() const final {
	    Alphabet set;
	    for (auto choice: choices) {
	       set = set + choice->acceptable();
	    }
	    return set;
	 }
      protected:
	 ProcessPtr internal_proceed(std::string& event) final {
	    for (auto choice: choices) {
	       auto p = choice->proceed(event);
	       if (p) {
		  /* if we have a chain of prefixed processes,
		     we return a SelectingProcess instead as
		     PrefixedProcess objects are always enclosed
		     by a SelectingProcess object;
		     this helps to see parentheses when it is printed */
		  auto pp = std::dynamic_pointer_cast<PrefixedProcess>(p);
		  if (pp) {
		     return std::make_shared<SelectingProcess>(pp);
		  }
		  return p;
	       }
	    }
	    return nullptr;
	 }
	 Alphabet internal_get_alphabet() const final {
	    Alphabet set;
	    for (auto choice: choices) {
	       set = set + choice->get_alphabet();
	    }
	    return set;
	 }
      private:
	 std::deque<ProcessPtr> choices;
	 void initialize_dependencies() const final {
	    for (auto choice: choices) {
	       choice->add_dependant(std::dynamic_pointer_cast<const Process>(
		  shared_from_this()));
	    }
	 }
   };

} // namespace CSP

#endif
