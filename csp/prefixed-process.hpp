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
   Extension of Process for prefixed processes, i.e. processes of
   the form

      event -> P
*/

#ifndef CSP_PREFIXED_PROCESS_HPP
#define CSP_PREFIXED_PROCESS_HPP

#include <cassert>
#include <iostream>
#include <memory>
#include <string>

#include "alphabet.hpp"
#include "process.hpp"
#include "symtable.hpp"

namespace CSP {

   class PrefixedProcess: public Process {
      public:
	 PrefixedProcess(const std::string& event, ProcessPtr process,
		  SymTable& symtab) :
	       event(event), process(process), symtab(symtab) {
	    assert(process);
	 }
	 const std::string& get_event() {
	    return event;
	 }
	 void print(std::ostream& out) const override {
	    out << event << " -> "; process->print(out);
	 }
	 void expanded_print(std::ostream& out) const override {
	    /* add parentheses if we are at top-level,
	       otherwise this is done by SelectingProcess */
	    out << "("; print(out); out << ")";
	 }
	 Alphabet acceptable(StatusPtr status) const final {
	    return Alphabet(event);
	 }

      private:
	 const std::string event;
	 ProcessPtr process;
	 SymTable& symtab;

	 ActiveProcess internal_proceed(const std::string& next_event,
	       StatusPtr status) final {
	    ProcessPtr p;
	    if (event == next_event) {
	       p = process;
	    } else {
	       p = nullptr;
	    }
	    return {p, status};
	 }
	 Alphabet internal_get_alphabet() const final {
	    return Alphabet(event) + process->get_alphabet();
	 }
	 void initialize_dependencies() const final {
	    /* check if the prefix includes channel names;
	       in this case we need to include this in our alphabet */
	    auto pos = event.find(".");
	    while (pos > 0 && pos < event.size()) {
	       auto name = event.substr(0, pos);
	       auto channel = symtab.lookup<Channel>(name);
	       if (channel) {
		  add_channel(channel);
		  break;
	       }
	       pos = event.find(".", pos + 1);
	    }
	    add_dependant(process);
	    process->add_dependant(std::dynamic_pointer_cast<const Process>(
	       shared_from_this()));
	 }
   };

} // namespace CSP

#endif
