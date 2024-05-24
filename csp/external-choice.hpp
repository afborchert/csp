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
   Extension of Process for an external choice the form P1 [] P2
*/

#ifndef CSP_EXTERNAL_CHOICE_HPP
#define CSP_EXTERNAL_CHOICE_HPP

#include <cassert>
#include <iostream>
#include <memory>
#include <string>

#include "alphabet.hpp"
#include "process.hpp"
#include "status.hpp"
#include "uniformint.hpp"

namespace CSP {

   class ExternalChoice: public Process {
      public:
	 ExternalChoice(ProcessPtr p, ProcessPtr q) :
	       process1(p), process2(q) {
	    assert(process1);
	    assert(process2);
	 }
	 void print(std::ostream& out) const override {
	    process1->print(out); out << " [] "; process2->print(out);
	 }
	 Alphabet acceptable(StatusPtr status) const final {
	    /* if we get asked, we make up our mind */
	    auto s = get_status<InternalStatus>(status);
	    return process1->acceptable(s->s1) +
	       process2->acceptable(s->s2);
	 }

      private:
	 struct InternalStatus: public Status {
	    StatusPtr s1;
	    StatusPtr s2;

	    InternalStatus(StatusPtr status) :
	       Status(status),
	       s1(std::make_shared<Status>(status)),
	       s2(std::make_shared<Status>(status)) {
	    }
	 };

	 ProcessPtr process1;
	 ProcessPtr process2;

	 ActiveProcess internal_proceed(const std::string& event,
	       StatusPtr status) final {
	    auto s = get_status<InternalStatus>(status);
	    Alphabet a1 = process1->acceptable(s->s1);
	    Alphabet a2 = process2->acceptable(s->s2);
	    bool ok1 = a1.is_member(event);
	    bool ok2 = a2.is_member(event);
	    if (ok1 && ok2) {
	       if (status->flip()) {
		  ok1 = false;
	       } else {
		  ok2 = false;
	       }
	    }
	    if (ok1) {
	       return process1->proceed(event, s->s1);
	    } else {
	       return process2->proceed(event, s->s2);
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
