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
   Extension of Process for processes that are prefixed with
   an output operation of the form

      channel!variable -> P
*/

#ifndef CSP_WRITING_PROCESS_HPP
#define CSP_WRITING_PROCESS_HPP

#include <cassert>
#include <iostream>
#include <memory>
#include <string>

#include "alphabet.hpp"
#include "channel.hpp"
#include "identifier.hpp"
#include "process.hpp"

namespace CSP {

   class WritingProcess: public Process {
      public:
	 WritingProcess(ChannelPtr channel, const std::string& varname,
		  ProcessPtr process) :
	       channel(channel), varname(varname), process(process) {
	    assert(process);
	 }
	 const ChannelPtr get_channel() {
	    return channel;
	 }
	 void print(std::ostream& out) const override {
	    out << channel->get_name() << "!" << varname << " -> ";
	    process->print(out);
	 }
	 void expanded_print(std::ostream& out) const override {
	    /* add parentheses if we are at top-level,
	       otherwise this is done by SelectingProcess */
	    out << "("; print(out); out << ")";
	 }
	 Alphabet acceptable(StatusPtr status) const final {
	    auto message = status->lookup<Identifier>(varname);
	    auto event = channel->get_name() + "." + message->get_name();
	    return Alphabet(event);
	 }

      private:
	 ChannelPtr channel;
	 std::string varname;
	 ProcessPtr process;

	 ActiveProcess internal_proceed(const std::string& next_event,
	       StatusPtr status) final {
	    auto message = status->lookup<Identifier>(varname);
	    auto event = channel->get_name() + "." + message->get_name();
	    ProcessPtr p;
	    if (next_event == event) {
	       p = process;
	    } else {
	       p = nullptr;
	    }
	    return {p, status};
	 }
	 Alphabet internal_get_alphabet() const final {
	    /* the rest of the alphabet is constructed through
	       output operations and channel declarations */
	    assert(process);
	    return process->get_alphabet();
	 }
	 void initialize_dependencies() const final {
	    add_channel(channel);
	    add_dependant(process);
	    process->add_dependant(std::dynamic_pointer_cast<const Process>(
	       shared_from_this()));
	 }
   };

} // namespace CSP

#endif
