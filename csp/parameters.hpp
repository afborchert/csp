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

#ifndef CSP_PARAMETERS_HPP
#define CSP_PARAMETERS_HPP

#include <cstdlib>
#include <deque>
#include <iostream>
#include <string>

#include "alphabet.hpp"
#include "object.hpp"

namespace CSP {

   /* list of actual parameters */
   class Parameters: public Object {
      public:
	 void print(std::ostream& out) const final {
	    out << "("; bool first = true;
	    for (auto event: events) {
	       if (first) {
		  first = false;
	       } else {
		  out << ", ";
	       }
	       out << event;
	    }
	    out << ")";
	 }
	 void add(std::string event) {
	    events.emplace_back(std::move(event));
	 }
	 auto size() const {
	    return events.size();
	 }
	 const std::string& at(std::size_t index) const {
	    return events.at(index);
	 }

      private:
	 std::deque<std::string> events;
   };

   using ParametersPtr = std::shared_ptr<Parameters>;
   using ConstParametersPtr = std::shared_ptr<const Parameters>;

   inline std::ostream& operator<<(std::ostream& out, ParametersPtr p) {
      p->print(out); return out;
   }

   inline std::ostream& operator<<(std::ostream& out, ConstParametersPtr p) {
      p->print(out); return out;
   }

} // namespace CSP

#endif
