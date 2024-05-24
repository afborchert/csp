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
	 void print(std::ostream& out) const override {
	    process1->print(out); out << "; "; process2->print(out);
	 }
	 Alphabet acceptable(StatusPtr status) const final {
	    if (process1->accepts_success(status)) {
	       return process2->acceptable(status);
	    } else {
	       return process1->acceptable(status);
	    }
	 }

      private:
	 ProcessPtr process1;
	 ProcessPtr process2;

	 ActiveProcess internal_proceed(const std::string& event,
	       StatusPtr status) final {
	    if (process1->accepts_success(status)) {
	       return process2->proceed(event, status);
	    } else {
	       auto [p, s] = process1->proceed(event, status);
	       return {std::make_shared<ProcessSequence>(p, process2), s};
	    }
	 }
	 Alphabet internal_get_alphabet() const final {
	    return process1->get_alphabet() + process2->get_alphabet();
	 }
	 void initialize_dependencies() const final {
	    auto me = shared_from_this();
	    process1->add_dependant(me);
	    process2->add_dependant(me);
	 }
   };

} // namespace CSP

#endif
