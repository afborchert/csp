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

#ifndef CSP_SYMBOL_CHANGER_HPP
#define CSP_SYMBOL_CHANGER_HPP

#include <functional>
#include <memory>
#include <string>

#include "alphabet.hpp"

/* function that maps symbols, see CSP 2.6 */

namespace CSP {

   class SymbolChanger;
   using SymbolChangerPtr = std::shared_ptr<SymbolChanger>;

   class SymbolChanger {
      public:
	 std::string map(std::string event) {
	    if (event[0] == '_') {
	       return event;
	    } else {
	       return internal_map(event);
	    }
	 }
	 Alphabet map(Alphabet a) {
	    Alphabet changed_a;
	    for (auto symbol: a) {
	       changed_a.add(map(symbol));
	    }
	    return changed_a;
	 }

	 std::string reverse_map(std::string event) {
	    if (event[0] == '_') {
	       return event;
	    } else {
	       return internal_reverse_map(event);
	    }
	 }

	 virtual std::string get_name(std::string name) = 0;

      private:
	 virtual std::string internal_map(std::string) = 0;
	 virtual std::string internal_reverse_map(std::string) = 0;
   };

   class Identity: public SymbolChanger {
      private:
	 std::string get_name(std::string name) final {
	    return name;
	 }

	 std::string internal_map(std::string event) final {
	    return event;
	 }

	 std::string internal_reverse_map(std::string event) final {
	    return event;
	 }
   };

   /* see CSP 2.6.2 */
   class Qualifier: public SymbolChanger {
      public:
	 Qualifier(std::string label) : label(label) {
	 }
      private:
	 std::string label;

	 std::string get_name(std::string name) final {
	    return label + ":" + name;
	 }

	 std::string internal_map(std::string event) final {
	    return label + "." + event;
	 }

	 std::string internal_reverse_map(std::string event) final {
	    return event.substr(label.size() + 1);
	 }
   };

} // namespace CSP

#endif
