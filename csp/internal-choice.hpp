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
   Extension of Process for an internal choice of the form P1 |~| P2
*/

#ifndef CSP_INTERNAL_CHOICE_HPP
#define CSP_INTERNAL_CHOICE_HPP

#include <cassert>
#include <iostream>
#include <memory>
#include <string>
#include "alphabet.hpp"
#include "process.hpp"
#include "uniformint.hpp"

namespace CSP {

   class InternalChoice: public Process {
      public:
	 InternalChoice(ProcessPtr p, ProcessPtr q) :
	       nextmove(undecided), process1(p), process2(q) {
	    assert(process1);
	    assert(process2);
	 }
	 void print(std::ostream& out) const override {
	    process1->print(out); out << " |~| "; process2->print(out);
	 }
	 Alphabet acceptable() const final {
	    /* if we get asked, we make up our mind */
	    decide();
	    if (nextmove == headforp1) {
	       return process1->acceptable();
	    } else {
	       return process2->acceptable();
	    }
	 }
      protected:
	 ProcessPtr internal_proceed(std::string& event) final {
	    decide();
	    if (nextmove == headforp1) {
	       nextmove = undecided;
	       return process1->proceed(event);
	    } else {
	       nextmove = undecided;
	       return process2->proceed(event);
	    }
	 }
	 Alphabet internal_get_alphabet() const final {
	    return process1->get_alphabet() + process2->get_alphabet();
	 }
      private:
	 /* if asked, we tell our next decision */
	 mutable enum {undecided, headforp1, headforp2} nextmove;
	 mutable UniformIntDistribution prg;
	 ProcessPtr process1;
	 ProcessPtr process2;
	 void decide() const {
	    if (nextmove == undecided) {
	       if (prg.flip()) {
		  nextmove = headforp1;
	       } else {
		  nextmove = headforp2;
	       }
	    }
	 }
	 void initialize_dependencies() const final {
	    process1->add_dependant(std::dynamic_pointer_cast<const Process>(
	       shared_from_this()));
	    process2->add_dependant(
	       std::dynamic_pointer_cast<const Process>(shared_from_this()));
	 }
   };

} // namespace CSP

#endif
