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
   NONINFRINGEMENT. IN NO IDENTIFIER SHALL THE AUTHORS OR COPYRIGHT HOLDERS
   BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
   ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
   CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
   SOFTWARE.
*/

#ifndef CSP_IDENTIFIER_HPP
#define CSP_IDENTIFIER_HPP

#include <iostream>
#include <string>

#include "object.hpp"

namespace CSP {

   class Identifier;
   using IdentifierPtr = std::shared_ptr<Identifier>;

   class Identifier: public Object {
      public:
	 Identifier(const std::string& name) : name(name) {
	 }
	 Identifier(IdentifierPtr qualifier, const std::string& name) :
	       name(qualifier->get_name() + "." + name) {
	 }
	 Identifier(IdentifierPtr qualifier, IdentifierPtr name) :
	       name(qualifier->get_name() + "." + name->get_name()) {
	 }

	 const std::string& get_name() const {
	    return name;
	 }
	 void print(std::ostream& out) const override {
	    out << name;
	 }
      private:
	 const std::string name;
   };

} // namespace CSP

#endif
