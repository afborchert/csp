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
   Extension of Process for processes that are prefixed with
   an input operation of the form

      channel?variable -> P
*/

#ifndef CSP_READING_PROCESS_HPP
#define CSP_READING_PROCESS_HPP

#include <cassert>
#include <iostream>
#include <memory>
#include <string>

#include "alphabet.hpp"
#include "channel.hpp"
#include "process.hpp"

namespace CSP {

   class ReadingProcess: public Process {
      public:
	 ReadingProcess(ChannelPtr channel,
	       const std::string& varname) :
	       channel(channel), varname(varname) {
	 }
	 void set_process(ProcessPtr p) {
	    assert(p && !process);
	    process = p;
	 }
	 const ChannelPtr get_channel() {
	    return channel;
	 }
	 void print(std::ostream& out) const override {
	    if (process) {
	       out << channel->get_name() <<
		  "?" << varname << " -> " << process;
	    } else {
	       out << channel->get_name() <<
		  "?" << varname << " -> ...";
	    }
	 }
	 void expanded_print(std::ostream& out) const override {
	    /* add parentheses if we are at top-level,
	       otherwise this is done by SelectingProcess */
	    out << "("; print(out); out << ")";
	 }
	 Alphabet acceptable(StatusPtr status) const final {
	    std::string prefix = channel->get_name() + ".";
	    auto prefix_len = prefix.length();
	    for (auto event: get_alphabet()) {
	       if (event.substr(0, prefix_len) == prefix) {
		  a += event;
	       }
	    }
	    return a;
	 }

      private:
	 ChannelPtr channel;
	 const std::string varname;
	 ProcessPtr process;
	 mutable Alphabet a; // return value for acceptable

	 ActiveProcess internal_proceed(const std::string& next_event,
	       StatusPtr status) final {
	    std::string prefix = channel->get_name() + ".";
	    auto prefix_len = prefix.length();
	    if (next_event.substr(0, prefix_len) != prefix) {
	       return {nullptr, status};
	    }
	    auto message = next_event.substr(prefix_len);
	    status = std::make_shared<Status>(status);
	    status->set(varname, std::make_shared<Identifier>(message));
	    return {process, status};
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
