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

#ifndef CSP_MAPPED_PROCESS_HPP
#define CSP_MAPPED_PROCESS_HPP

#include <iostream>
#include <memory>
#include <sstream>
#include <string>

#include "alphabet.hpp"
#include "process.hpp"
#include "symbol-changer.hpp"

namespace CSP {

   class MappedProcess;
   using MappedProcessPtr = std::shared_ptr<MappedProcess>;

   class MappedProcess: public Process {
      public:
	 MappedProcess(ProcessPtr process, SymbolChangerPtr f) :
	       f(f), process(process) {
	    assert(process);
	 }
	 void print(std::ostream& out) const override {
	    std::ostringstream os; process->print(os);
	    out << f->get_name(os.str());
	 }
	 Alphabet acceptable() const final {
	    return f->map(process->acceptable());
	 }
      protected:
	 ProcessPtr internal_proceed(const std::string& event) final {
	    auto p = process->proceed(f->reverse_map(event));
	    return std::make_shared<MappedProcess>(p, f);
	 }
	 Alphabet internal_get_alphabet() const final {
	    /* mapping is done through map_alphabet below */
	    return process->get_alphabet();
	 }
      private:
	 SymbolChangerPtr f;
	 ProcessPtr process;

	 Alphabet map_alphabet(const Alphabet& alphabet) const final {
	    return f->map(alphabet);
	 }

	 void initialize_dependencies() const final {
	    process->add_dependant(std::dynamic_pointer_cast<const Process>(
	       shared_from_this()));
	 }
   };

} // namespace CSP

#endif
