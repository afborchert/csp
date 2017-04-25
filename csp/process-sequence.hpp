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
   Sequence of processes, i.e. processes of the form P1; P2
*/

#ifndef CSP_PROCESS_SEQUENCE_HPP
#define CSP_PROCESS_SEQUENCE_HPP

#include <cassert>
#include <iostream>
#include <memory>
#include <string>
#include "alphabet.hpp"
#include "process.hpp"

namespace CSP {

   class ProcessSequence: public Process {
      public:
	 ProcessSequence(ProcessPtr p, ProcessPtr q) :
	       process1(p), process2(q) {
	    assert(process1);
	    assert(process2);
	 }
	 virtual void print(std::ostream& out) const {
	    process1->print(out); out << "; "; process2->print(out);
	 }
	 virtual Alphabet acceptable() const {
	    if (process1->accepts_success()) {
	       return process2->acceptable();
	    } else {
	       return process1->acceptable();
	    }
	 }
      protected:
	 virtual ProcessPtr internal_proceed(std::string& event) {
	    if (process1->accepts_success()) {
	       return process2->proceed(event);
	    } else {
	       return std::make_shared<ProcessSequence>(
		  process1->proceed(event), process2);
	    }
	 }
	 virtual Alphabet internal_get_alphabet() const {
	    return process1->get_alphabet() + process2->get_alphabet();
	 }
      private:
	 ProcessPtr process1;
	 ProcessPtr process2;
	 virtual void initialize_dependencies() const {
	    process1->add_dependant(std::dynamic_pointer_cast<const Process>(
	       shared_from_this()));
	    process2->add_dependant(
	       std::dynamic_pointer_cast<const Process>(shared_from_this()));
	 }
   };

} // namespace CSP

#endif
