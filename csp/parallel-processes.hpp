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

/*
   Extension of Process for parallel processes, i.e. processes of
   the form P1 || P2
*/

#ifndef CSP_PARALLEL_PROCESSES_HPP
#define CSP_PARALLEL_PROCESSES_HPP

#include <cassert>
#include <iostream>
#include <memory>
#include <string>

#include "alphabet.hpp"
#include "process.hpp"

namespace CSP {

   class ParallelProcesses: public Process {
      public:
	 ParallelProcesses(ProcessPtr p1, ProcessPtr p2) :
	       process1(p1), process2(p2) {
	    assert(process1); assert(process2);
	 }
	 void print(std::ostream& out) const override {
	    process1->print(out); out << " || "; process2->print(out);
	 }
	 void expanded_print(std::ostream& out) const override {
	    process1->expanded_print(out);
	    out << " || ";
	    process2->expanded_print(out);
	 }
	 Alphabet acceptable(StatusPtr status) const final {
	    /* events are acceptable either
	         if they are accepted by both, or
		 if they do belong to the alphabet of one of the processes only
		    and are acceptable by the corresponding process
	    */
	    Alphabet sd = process1->get_alphabet() / process2->get_alphabet();
	    auto s = get_status<InternalStatus>(status);
	    Alphabet p1a = process1->acceptable(s->s1);
	    Alphabet p2a = process2->acceptable(s->s2);
	    Alphabet ex1 = sd * p1a;
	    Alphabet ex2 = sd * p2a;
	    return p1a * p2a + ex1 + ex2;
	 }

      private:
	 struct InternalStatus: public Status {
	    StatusPtr s1;
	    StatusPtr s2;
	    InternalStatus(StatusPtr status) :
	       Status(status), s1(status), s2(status) {
	    }
	 };

	 ProcessPtr process1;
	 ProcessPtr process2;

	 ActiveProcess internal_proceed(const std::string& event,
	       StatusPtr status) final {
	    auto s = get_status<InternalStatus>(status);
	    auto [p1, s1] = process1->proceed(event, s->s1);
	    auto [p2, s2] = process2->proceed(event, s->s2);
	    if (p1 && p2) {
	       s->s1 = s1; s->s2 = s2;
	       return {std::make_shared<ParallelProcesses>(p1, p2), s};
	    } else {
	       return {nullptr, s};
	    }
	 };
	 Alphabet internal_get_alphabet() const final {
	    return process1->get_alphabet() + process2->get_alphabet();
	 }
   };

} // namespace CSP

#endif
