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

#ifndef CSP_EVENT_SET_HPP
#define CSP_EVENT_SET_HPP

#include <iostream>
#include <string>

#include "alphabet.hpp"
#include "object.hpp"

namespace CSP {

   class EventSet: public Object {
      public:
	 EventSet() {
	 }
	 EventSet(const Alphabet& alphabet) : alphabet(alphabet) {
	 }
	 EventSet(const std::string& name) {
	    alphabet.add(name);
	 }
	 void print(std::ostream& out) const override {
	    out << alphabet;
	 }
	 const Alphabet& get_alphabet() const {
	    return alphabet;
	 }
      private:
	 Alphabet alphabet;
   };

   using EventSetPtr = std::shared_ptr<EventSet>;

} // namespace CSP

#endif
