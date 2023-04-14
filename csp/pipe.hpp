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

#ifndef CSP_PIPE_HPP
#define CSP_PIPE_HPP

#include <cassert>
#include <iostream>
#include <memory>
#include <string>

#include "alphabet.hpp"
#include "concealed-process.hpp"
#include "context.hpp"
#include "process.hpp"
#include "symbol-changer.hpp"
#include "symtable.hpp"

namespace CSP {

   class Pipe: public Process {
      public:
	 Pipe(Context& context, ProcessPtr p, ProcessPtr q) :
	       context(context), p(p), q(q) {
	    assert(p); assert(q);
	 }
	 void print(std::ostream& out) const override {
	    p->print(out); out << " >> "; q->print(out);
	 }
	 Alphabet acceptable(StatusPtr status) const final {
	    setup();
	    return pipe->acceptable(status);
	 }

      private:
	 Context& context;
	 ProcessPtr p;
	 ProcessPtr q;
	 mutable ProcessPtr pipe;

	 ProcessPtr conceal_except(ProcessPtr p,
	       const std::string& left,
	       const std::string& right) const {
	    auto p_alpha = p->get_alphabet();
	    auto conceal = exclude_prefix(exclude_prefix(p_alpha, left), right);
	    if (conceal.cardinality() > 0) {
	       return std::make_shared<ConcealedProcess>(p, conceal);
	    } else {
	       return p;
	    }
	 }

	 void setup() const {
	    if (!pipe) {
	       auto mid = context.symtab().get_unique_symbol();
	       auto map_right = std::make_shared<MapChannel>("right", mid);
	       auto map_left = std::make_shared<MapChannel>("left", mid);
	       auto p_mapped = conceal_except(
		     std::make_shared<MappedProcess>(p, map_right),
		     "left", mid);
	       auto q_mapped = conceal_except(
		     std::make_shared<MappedProcess>(q, map_left),
		     mid, "right");
	       auto pp = std::make_shared<ParallelProcesses>(p_mapped,
		  q_mapped);
	       auto conceal = select_prefix(pp->get_alphabet(), mid);
	       pipe = std::make_shared<ConcealedProcess>(pp, conceal);
	    }
	 }

	 ActiveProcess internal_proceed(const std::string& event,
	       StatusPtr status) final {
	    setup();
	    return pipe->proceed(event, status);
	 }
	 Alphabet internal_get_alphabet() const final {
	    auto p_alpha = select_prefix(p->get_alphabet(), "left");
	    auto q_alpha = select_prefix(q->get_alphabet(), "right");
	    return p_alpha + q_alpha;
	 }

	 void initialize_dependencies() const final {
	 }
   };

} // namespace CSP

#endif

