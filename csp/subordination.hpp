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
   Support of subordination operator P // Q,
   see CSP 4.5
*/

#ifndef CSP_SUBORDINATION_HPP
#define CSP_SUBORDINATION_HPP

#include <cassert>
#include <iostream>
#include <memory>
#include <string>

#include "alphabet.hpp"
#include "process.hpp"

namespace CSP {

   class Subordination: public Process {
      public:
	 Subordination(ProcessPtr p, ProcessPtr q) : p(p), q(q) {
	    assert(p); assert(q);
	 }
	 void print(std::ostream& out) const override {
	    p->print(out); out << " // "; q->print(out);
	 }
	 Alphabet acceptable(StatusPtr status) const final {
	    setup();
	    return pq->acceptable(status);
	 }

      private:
	 ProcessPtr p;
	 ProcessPtr q;
	 mutable ProcessPtr pq; // (P || Q) \ alpha P

	 void setup() const {
	    if (!pq) {
	       auto pp = std::make_shared<ParallelProcesses>(p, q);
	       auto p_alpha = p->get_alphabet();
	       auto q_alpha = q->get_alphabet();
	       auto conceal = p_alpha * q_alpha;
	       pq = std::make_shared<ConcealedProcess>(pp, conceal);
	    }
	 }

	 ActiveProcess internal_proceed(const std::string& event,
	       StatusPtr status) final {
	    setup();
	    return pq->proceed(event, status);
	 }
	 Alphabet internal_get_alphabet() const final {
	    auto p_alpha = p->get_alphabet();
	    auto q_alpha = q->get_alphabet();
	    return q_alpha - p_alpha;
	 }

	 void initialize_dependencies() const {
	    /* make sure that the alphabet of p includes that of q */
	    p->add_dependant(q);
	 }
   };

} // namespace CSP

#endif
