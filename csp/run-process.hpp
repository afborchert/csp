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
   RUN process
*/

#ifndef CSP_RUN_PROCESS_HPP
#define CSP_RUN_PROCESS_HPP

#include <cassert>
#include <iostream>
#include <memory>
#include <string>
#include "alphabet.hpp"
#include "process.hpp"

namespace CSP {

   class RunProcess: public Process {
      public:
	 RunProcess(const Alphabet& alphabet) :
	       run_alphabet(alphabet) {
	 }
	 RunProcess(ProcessPtr p_alphabet) :
	       p_alphabet(p_alphabet) {
	 }
	 void print(std::ostream& out) const override {
	    out << "RUN " << get_alphabet();
	 }
	 Alphabet acceptable() const final {
	    return get_alphabet();
	 }
      protected:
	 ProcessPtr internal_proceed(const std::string& next_event) final {
	    return std::dynamic_pointer_cast<Process>(shared_from_this());
	 }
	 Alphabet internal_get_alphabet() const final {
	    if (p_alphabet) {
	       return p_alphabet->get_alphabet();
	    } else {
	       return run_alphabet;
	    }
	 }
      private:
	 const Alphabet run_alphabet;
	 ProcessPtr p_alphabet; // process from which we take its alphabet

	 void initialize_dependencies() const final {
	    if (p_alphabet) {
	       p_alphabet->add_dependant(
		  std::dynamic_pointer_cast<const Process>
		     (shared_from_this()));
	    }
	 }
   };

} // namespace CSP

#endif
