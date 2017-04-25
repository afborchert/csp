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
   Extension of Process for selecting processes, i.e. process with
   the form P1 | P2
*/

#ifndef CSP_SELECTING_PROCESS_HPP
#define CSP_SELECTING_PROCESS_HPP

#include <cassert>
#include <iostream>
#include <memory>
#include <string>
#include "alphabet.hpp"
#include "process.hpp"

namespace CSP {

   class SelectingProcess: public Process {
      public:
	 SelectingProcess(ProcessPtr process, ProcessPtr pp) :
	       choice(pp), other_choices(process) {
	    assert(other_choices);
	    assert(choice);
	 }
	 virtual void print(std::ostream& out) const {
	    other_choices->print(out); out << " | "; choice->print(out);
	 }
	 virtual Alphabet acceptable() const {
	    return choice->acceptable() + other_choices->acceptable();
	 }
      protected:
	 virtual ProcessPtr internal_proceed(std::string& event) {
	    ProcessPtr p = other_choices->proceed(event);
	    if (!p) {
	       p = choice->proceed(event);
	    }
	    return p;
	 }
	 virtual Alphabet internal_get_alphabet() const {
	    return choice->get_alphabet() + other_choices->get_alphabet();
	 }
      private:
	 ProcessPtr choice;
	 ProcessPtr other_choices;
	 virtual void initialize_dependencies() const {
	    choice->add_dependant(std::dynamic_pointer_cast<const Process>(
	       shared_from_this()));
	    other_choices->add_dependant(
	       std::dynamic_pointer_cast<const Process>(shared_from_this()));
	 }
   };

} // namespace CSP

#endif
