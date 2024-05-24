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
	 ChaosProcess(const Alphabet& alphabet) : chaos_alphabet(alphabet) {
	 }
	 ChaosProcess(ProcessPtr p_alphabet) : p_alphabet(p_alphabet) {
	 }
	 void print(std::ostream& out) const override {
	    out << "CHAOS " << get_alphabet();
	 }
	 Alphabet acceptable(StatusPtr status) const final {
	    auto s = get_status<InternalStatus>(status);
	    decide(s);
	    return s->accepting_next;
	 }

      private:
	 struct InternalStatus: public Status {
	    mutable enum {undecided, decided} state;
	    mutable Alphabet accepting_next; // defined if state == decided

	    InternalStatus(StatusPtr status) :
	       Status(status), state(undecided) {
	    }
	 };
	 using InternalStatusPtr = std::shared_ptr<InternalStatus>;

	 const Alphabet chaos_alphabet;
	 ProcessPtr p_alphabet; // process from which we take its alphabet

	 ActiveProcess internal_proceed(const std::string& next_event,
	       StatusPtr status) final {
	    auto s = get_status<InternalStatus>(status);
	    decide(s);
	    bool ok = s->accepting_next.is_member(next_event);
	    s->state = InternalStatus::undecided;
	    if (ok) {
	       return {shared_from_this(), s};
	    } else {
	       /* STOP */
	       return {nullptr, s};
	    }
	 }
	 Alphabet internal_get_alphabet() const final {
	    if (p_alphabet) {
	       return p_alphabet->get_alphabet();
	    } else {
	       return chaos_alphabet;
	    }
	 }
	 void decide(InternalStatusPtr s) const {
	    if (s->state == InternalStatus::undecided) {
	       s->accepting_next = Alphabet();
	       for (const auto& event: get_alphabet()) {
		  if (s->flip()) {
		     s->accepting_next.add(event);
		  }
	       }
	       s->state = InternalStatus::decided;
	    }
	 }
	 void initialize_dependencies() const final {
	    if (p_alphabet) {
	       p_alphabet->add_dependant(shared_from_this());
	    }
	 }
   };

} // namespace CSP

#endif
