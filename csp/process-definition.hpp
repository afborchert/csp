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
   A process definition object behaves like its right-hand-side process
   but it remembers its left-hand-side name for printing;
   processes can be parameterized with events
*/

#ifndef CSP_PROCESS_DEFINITION_HPP
#define CSP_PROCESS_DEFINITION_HPP

#include <cstdlib>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "alphabet.hpp"
#include "named-process.hpp"
#include "parameters.hpp"
#include "symtable.hpp"

namespace CSP {

   class ProcessDefinition;
   using ProcessDefinitionPtr = std::shared_ptr<ProcessDefinition>;

   class ProcessDefinition: public NamedProcess {
      public:
	 ProcessDefinition(const std::string& name) :
	       NamedProcess(name) {
	 }
	 ProcessDefinition(const std::string& name,
		  ParametersPtr params) :
	       NamedProcess(name), params(params) {
	 }

	 void set_process(ProcessPtr p) {
	    assert(p); assert(!process);
	    process = p;
	 }

	 void enter_parameters(SymTable& symtab) {
	    if (params) {
	       for (std::size_t i = 0; i < params->size(); ++i) {
		  symtab.define(params->at(i));
	       }
	    }
	 }
	 void print(std::ostream& out) const override {
	    if (params) {
	       out << get_name() << params << " = ";
	    } else {
	       out << get_name() << " = ";
	    }
	    if (process) {
	       process->print(out);
	    } else {
	       out << "*undefined*";
	    }
	 }
	 ConstParametersPtr get_params() const {
	    return params;
	 }

	 Alphabet acceptable(StatusPtr status) const final {
	    assert(process);
	    return process->acceptable(status);
	 }

	 void add_channel(ChannelPtr c) const override {
	    assert(process);
	    process->add_channel(c);
	 }

      private:
	 ParametersPtr params; // if any
	 ProcessPtr process;

	 ActiveProcess internal_proceed(const std::string& event,
	       StatusPtr status) final {
	    return process->proceed(event, status);
	 }
	 Alphabet internal_get_alphabet() const final {
	    return process->get_alphabet();
	 }
	 void initialize_dependencies() const final {
	    process->add_dependant(std::dynamic_pointer_cast<const Process>(
	       shared_from_this()));
	 }
   };

} // namespace CSP

#endif
