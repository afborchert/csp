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

#ifndef CSP_BINDINGS_HPP
#define CSP_BINDINGS_HPP

#include <cassert>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <unordered_map>

#include "object.hpp"

namespace CSP {
   /* we do not include "process.hpp" to avoid a reference cycle */
   class Process;
   using ProcessPtr = std::shared_ptr<Process>;

   struct BindingsKey: public Object {
      BindingsKey(ProcessPtr p, std::string name) :
	    p(p), name(name) {
      }
      ProcessPtr p;
      std::string name;
      void print(std::ostream& out) const override {
	 out << name;
      }
   };
   using BindingsKeyPtr = std::shared_ptr<BindingsKey>;

   class Bindings {
      public:
	 void set(BindingsKeyPtr key, std::string message) {
	    assert(key);
	    b.insert_or_assign(key, std::move(message));
	 }
	 std::string get(BindingsKeyPtr key) const {
	    auto it = b.find(key);
	    assert(it != b.end());
	    return it->second;
	 }
      private:
	 std::unordered_map<BindingsKeyPtr, std::string> b;
   };

} // namespace CSP

#endif
