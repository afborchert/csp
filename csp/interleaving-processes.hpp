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
   Extension of Process for interleaving processes, i.e. processes of
   the form P1 ||| P2
*/

#ifndef CSP_INTERLEAVING_PROCESSES_HPP
#define CSP_INTERLEAVING_PROCESSES_HPP

#include <cassert>
#include <iostream>
#include <memory>
#include <string>
#include "alphabet.hpp"
#include "process.hpp"
#include "uniformint.hpp"

namespace CSP {

   class InterleavingProcesses: public Process {
      public:
	 InterleavingProcesses(ProcessPtr p, ProcessPtr q) :
	       process1(p), process2(q) {
	    assert(process1);
	    assert(process2);
	 }
	 void print(std::ostream& out) const override {
	    process1->print(out); out << " ||| "; process2->print(out);
	 }
	 Alphabet acceptable() const final {
	    return process1->acceptable() + process2->acceptable();
	 }
      protected:
	 ProcessPtr internal_proceed(std::string& event) final {
	    Alphabet a1 = process1->acceptable();
	    Alphabet a2 = process2->acceptable();
	    bool ok1 = a1.is_member(event);
	    bool ok2 = a2.is_member(event);
	    if (ok1 && ok2) {
	       if (prg.flip()) {
		  ok1 = false;
	       } else {
		  ok2 = false;
	       }
	    }
	    if (ok1) {
	       return std::make_shared<InterleavingProcesses>(
			process1->proceed(event), process2);
	    } else if (ok2) {
	       return std::make_shared<InterleavingProcesses>(
			process1, process2->proceed(event));
	    } else {
	       return nullptr;
	    }
	 }
	 Alphabet internal_get_alphabet() const final {
	    return process1->get_alphabet() + process2->get_alphabet();
	 }
      private:
	 ProcessPtr process1;
	 ProcessPtr process2;
	 UniformIntDistribution prg;
	 void initialize_dependencies() const final {
	    process1->add_dependant(std::dynamic_pointer_cast<const Process>(
	       shared_from_this()));
	    process2->add_dependant(
	       std::dynamic_pointer_cast<const Process>(shared_from_this()));
	 }
   };

} // namespace CSP

#endif
