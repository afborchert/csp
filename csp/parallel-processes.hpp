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
	 Alphabet acceptable() const final {
	    /* events are acceptable either
	         if they are accepted by both, or
		 if they do belong to the alphabet of one of the processes only
		    and are acceptable by the corresponding process
	    */
	    Alphabet sd = process1->get_alphabet() / process2->get_alphabet();
	    Alphabet p1a = process1->acceptable();
	    Alphabet p2a = process2->acceptable();
	    Alphabet ex1 = sd * p1a;
	    Alphabet ex2 = sd * p2a;
	    return p1a * p2a + ex1 + ex2;
	 }
      protected:
	 ProcessPtr internal_proceed(const std::string& event) final {
	    ProcessPtr p1 = process1->proceed(event);
	    ProcessPtr p2 = process2->proceed(event);
	    if (p1 && p2) {
	       return std::make_shared<ParallelProcesses>(p1, p2);
	    } else {
	       return nullptr;
	    }
	 };
	 Alphabet internal_get_alphabet() const final {
	    return process1->get_alphabet() + process2->get_alphabet();
	 }
      private:
	 ProcessPtr process1;
	 ProcessPtr process2;
   };

} // namespace CSP

#endif
