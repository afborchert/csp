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
   A process definition object behaves like its right-hand-side process
   but it remembers its left-hand-side name for printing
*/

#ifndef CSP_PROCESS_REFERENCE_HPP
#define CSP_PROCESS_REFERENCE_HPP

#include <cstdlib>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

namespace CSP {
   class ProcessReference;
   using ProcessReferencePtr = std::shared_ptr<ProcessReference>;
}

#include "alphabet.hpp"
#include "context.hpp"
#include "error.hpp"
#include "identifier.hpp"
#include "parameters.hpp"
#include "parser.hpp"
#include "process-definition.hpp"
#include "named-process.hpp"
#include "scanner.hpp"
#include "symtable.hpp"

namespace CSP {

   class ProcessReference: public NamedProcess {
      public:
	 ProcessReference(const location& loc,
		  const std::string& name,
		  Context& context) :
	       NamedProcess(name), loc(loc), context(context) {
	 }
	 ProcessReference(const location& loc,
		  const std::string& name,
		  ParametersPtr params,
		  Context& context) :
	       NamedProcess(name), loc(loc), context(context),
	       actual(params), bound(params->size(), false) {
	    for (std::size_t i = 0; i < actual->size(); ++i) {
	       if (context.symtab().defined(actual->at(i))) {
		  bound[i] = true;
	       }
	    }
	 }

	 void register_ref() {
	    if (!resolve()) {
	       auto me = std::dynamic_pointer_cast<ProcessReference>(
		  shared_from_this());
	       context.symtab().add_unresolved(loc, get_name(),
		  [me]() {
		     return me->resolve();
		  });
	    }
	 }

	 /* suppress type checks */
	 void set_refonly() {
	    just_reference = true;
	 }

	 bool resolve() const {
	    if (p) return true;
	    auto pdef = context.symtab().lookup<ProcessDefinition>(get_name());
	    if (!pdef) {
	       p = context.symtab().lookup<NamedProcess>(get_name());
	       if (!p) return false;
	       if (actual) {
		  yyerror(loc, context, "reference of process '%s' "
		     "does not match its definition", get_name());
	       }
	    } else if (!just_reference) {
	       formal = pdef->get_params();
	       if (!formal != !actual ||
		     (formal && formal->size() != actual->size())) {
		  yyerror(loc, context, "reference of process '%s' "
		     "does not match its definition", get_name());
	       }
	       p = pdef;
	    }
	    for (auto c: channels) {
	       p->add_channel(c);
	       channels.clear();
	    }
	    return true;
	 }

	 void print(std::ostream& out) const override {
	    if (actual) {
	       out << get_name() << actual;
	    } else {
	       out << get_name();
	    }
	 }

	 Alphabet acceptable(StatusPtr status) const final {
	    if (!get_alphabet()) {
	       /* if our alphabet is empty, we simply return
		  the empty set; this test avoids an endless
		  recursion in case of constructs of the type
		     P = P
	       */
	       return Alphabet();
	    }
	    if (!p) resolve();
	    if (p) {
	       auto s = std::make_shared<Status>(status);
	       setup_bindings(s);
	       return p->acceptable(s);
	    } else {
	       return Alphabet();
	    }
	 }

	 void add_channel(ChannelPtr c) const override {
	    if (p || resolve()) {
	       p->add_channel(c);
	    } else {
	       channels.push_back(c);
	    }
	 }

      private:
	 const location loc;
	 Context& context;
	 mutable ProcessPtr p;
	 ParametersPtr actual;
	 std::vector<bool> bound;
	 mutable ConstParametersPtr formal;
	 mutable std::deque<ChannelPtr> channels;
	 bool just_reference = false; // just referencing, not executing

	 void setup_bindings(StatusPtr status) const {
	    if (actual) {
	       for (std::size_t i = 0; i < actual->size(); ++i) {
		  auto param = actual->at(i);
		  if (bound[i]) {
		     auto id = status->lookup<Identifier>(param);
		     param = id->get_name();
		  }
		  status->set(formal->at(i),
		     std::make_shared<Identifier>(param));
	       }
	    }
	 }

	 ActiveProcess internal_proceed(const std::string& event,
	       StatusPtr status) final {
	    if (p) {
	       auto s = std::make_shared<Status>(status);
	       setup_bindings(s);
	       return p->proceed(event, s);
	    } else {
	       return {nullptr, status};
	    }
	 }
	 Alphabet internal_get_alphabet() const final {
	    if (!p) resolve();
	    if (p) {
	       return p->get_alphabet();
	    } else {
	       return Alphabet();
	    }
	 }
	 void initialize_dependencies() const final {
	    if (!p) {
	       resolve();
	       if (!p) return;
	    }
	    p->add_dependant(std::dynamic_pointer_cast<const Process>(
	       Process::shared_from_this()));
	    add_dependant(p);
	 }
   };

} // namespace CSP

#endif
