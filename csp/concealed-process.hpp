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
   Extension of Process for a concealment of the form P \ A
   where A is an alphabet
*/

#ifndef CSP_CONCEALED_PROCESS_HPP
#define CSP_CONCEALED_PROCESS_HPP

#include <cassert>
#include <iostream>
#include <memory>
#include <string>
#include "alphabet.hpp"
#include "process.hpp"
#include "uniformint.hpp"

namespace CSP {

   class ConcealedProcess: public Process {
      public:
	 ConcealedProcess(ProcessPtr p, Alphabet concealed) :
	    process(p), concealed(concealed), state(undecided) {
	    assert(process);
	    assert(concealed.cardinality() > 0); // otherwise not useful
	 }
	 void print(std::ostream& out) const override {
	    process->print(out); out << " \\ " << concealed;
	 }
	 Alphabet acceptable() const final {
	    decide();
	    if (next) {
	       return next->acceptable() - concealed;
	    } else {
	       /* too bad -- we have turned into STOP */
	       return Alphabet();
	    }
	 }
      protected:
	 ProcessPtr internal_proceed(std::string& event) final {
	    decide();
	    ProcessPtr p = next;
	    state = undecided;

	    if (!p) {
	       return nullptr;
	    }
	    p = std::make_shared<ConcealedProcess>(p->proceed(event),
	       concealed);
	    p->set_alphabet(process->get_alphabet() - concealed);
	    return p;
	 }
	 Alphabet internal_get_alphabet() const final {
	    return process->get_alphabet() - concealed;
	 }
      private:
	 ProcessPtr process;
	 Alphabet concealed;
	 /* if asked, we move ahead */
	 mutable enum {undecided, decided} state;
	 mutable UniformIntDistribution prg;
	 mutable ProcessPtr next; // defined if state == decided
	 void decide() const {
	    if (state == undecided) {
	       /* as noted in 3.5.2 the implementation of this operator
		  is inherently non-deterministic and thereby possibly
		  divergent, i.e. this could be an endless loop if
		  we would not limit this to 1000 attempts;
		  this implementation choses one of the acceptable
		  events uniformely
	       */
	       unsigned int count = 0;
	       ProcessPtr p = process;
	       while (p && count++ < 1000) {
		  Alphabet acceptable = p->acceptable();
		  if (acceptable.cardinality() == 0) {
		     /* deadlock */
		     next = nullptr; state = decided; return;
		  }
		  /* we are chosing now the next event by random */
		  auto chose = prg.draw(acceptable.cardinality());
		  auto event = *std::next(acceptable.begin(), chose);
		  if (!concealed.is_member(event)) {
		     next = p; state = decided;
		     return;
		  }
		  p = p->proceed(event);
	       }
	       /* emergency break from a possibly otherwise endless loop;
	          the only option we have here is to turn into STOP */
	       next = nullptr;
	       state = decided;
	    }
	 }
	 /* no initialize_dependencies */
   };

} // namespace CSP

#endif
