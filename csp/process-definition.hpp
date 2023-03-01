/* 
   Copyright (c) 2011-2022 Andreas F. Borchert
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

#ifndef CSP_PROCESS_DEFINITION_HPP
#define CSP_PROCESS_DEFINITION_HPP

#include <iostream>
#include <memory>
#include <string>
#include "alphabet.hpp"
#include "process.hpp"

namespace CSP {

   class ProcessDefinition;
   using ProcessDefinitionPtr = std::shared_ptr<ProcessDefinition>;

   class ProcessDefinition: public Process {
      public:
	 ProcessDefinition(const std::string& name, ProcessPtr process) :
	       name(name), process(process) {
	    assert(process);
	 }
	 const std::string& get_name() const {
	    return name;
	 }
	 void print(std::ostream& out) const override {
	    out << name << " = "; process->print(out);
	 }
	 Alphabet acceptable() const final {
	    return process->acceptable();
	 }
      private:
	 ProcessPtr internal_proceed(const std::string& event) final {
	    return process->proceed(event);
	 }
	 Alphabet internal_get_alphabet() const final {
	    return process->get_alphabet();
	 }
	 const std::string name;
	 ProcessPtr process;
	 void initialize_dependencies() const final {
	    process->add_dependant(std::dynamic_pointer_cast<const Process>(
	       shared_from_this()));
	 }
   };

} // namespace CSP

#endif
