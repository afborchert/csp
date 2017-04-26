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
   Extension of Process for prefixed processes, i.e. processes of
   the form

      event -> P
*/

#ifndef CSP_PREFIXED_PROCESS_HPP
#define CSP_PREFIXED_PROCESS_HPP

#include <cassert>
#include <iostream>
#include <memory>
#include <string>
#include "alphabet.hpp"
#include "process.hpp"

namespace CSP {

   class PrefixedProcess: public Process {
      public:
	 PrefixedProcess(const std::string& event, ProcessPtr process) :
	       event(event), process(process) {
	    assert(process);
	 }
	 const std::string& get_event() {
	    return event;
	 }
	 virtual void print(std::ostream& out) const {
	    out << event << " -> "; process->print(out);
	 }
	 virtual void expanded_print(std::ostream& out) const {
	    /* add parentheses if we are at top-level,
	       otherwise this is done by SelectingProcess */
	    out << "("; print(out); out << ")";
	 }
	 virtual Alphabet acceptable() const {
	    return Alphabet(event);
	 }
      protected:
	 virtual ProcessPtr internal_proceed(std::string& next_event) {
	    if (event == next_event) {
	       return process;
	    } else {
	       return nullptr;
	    }
	 }
	 virtual Alphabet internal_get_alphabet() const {
	    return Alphabet(event) + process->get_alphabet();
	 }
      private:
	 const std::string event;
	 ProcessPtr process;
	 virtual void initialize_dependencies() const {
	    process->add_dependant(std::dynamic_pointer_cast<const Process>(
	       shared_from_this()));
	 }
   };

} // namespace CSP

#endif
