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
   CHAOS process
*/

#ifndef CSP_CHAOS_PROCESS_HPP
#define CSP_CHAOS_PROCESS_HPP

#include <cassert>
#include <iostream>
#include <memory>
#include <string>
#include "alphabet.hpp"
#include "process.hpp"

namespace CSP {

   class ChaosProcess: public Process {
      public:
	 ChaosProcess(const Alphabet& alphabet) :
	       chaos_alphabet(alphabet), state(undecided) {
	 }
	 ChaosProcess(ProcessPtr p_alphabet) :
	       p_alphabet(p_alphabet), state(undecided) {
	 }
	 virtual void print(std::ostream& out) const {
	    out << "CHAOS " << get_alphabet();
	 }
	 virtual Alphabet acceptable() const {
	    decide();
	    return accepting_next;
	 }
      protected:
	 virtual ProcessPtr internal_proceed(std::string& next_event) {
	    decide();
	    bool ok = accepting_next.is_member(next_event);
	    state = undecided;
	    if (ok) {
	       return std::dynamic_pointer_cast<Process>(shared_from_this());
	    } else {
	       /* STOP */
	       return nullptr;
	    }
	 }
	 virtual Alphabet internal_get_alphabet() const {
	    if (p_alphabet) {
	       return p_alphabet->get_alphabet();
	    } else {
	       return chaos_alphabet;
	    }
	 }
      private:
	 const Alphabet chaos_alphabet;
	 ProcessPtr p_alphabet; // process from which we take its alphabet
	 mutable enum {undecided, decided} state;
	 mutable UniformIntDistribution prg;
	 mutable Alphabet accepting_next; // defined if state == decided

	 void decide() const {
	    if (state == undecided) {
	       accepting_next = Alphabet();
	       for (const auto& event: get_alphabet()) {
		  if (prg.flip()) {
		     accepting_next.add(event);
		  }
	       }
	       state = decided;
	    }
	 }

	 virtual void initialize_dependencies() const {
	    if (p_alphabet) {
	       p_alphabet->add_dependant(
		  std::dynamic_pointer_cast<const Process>(shared_from_this()));
	    }
	 }
   };

} // namespace CSP

#endif
