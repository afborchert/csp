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

#ifndef CSP_SCOPE_HPP
#define CSP_SCOPE_HPP

#include <map>
#include <memory>
#include <string>

#include "object.hpp"

namespace CSP {

   class Scope;
   using ScopePtr = std::shared_ptr<Scope>;

   class Scope {
      public:
	 // constructors
	 Scope() = default;
	 Scope(ScopePtr outer) : outer(outer) {}

	 // accessors
	 template <typename T>
	 std::shared_ptr<T> lookup(const std::string& name) const {
	    auto [ok, it] = find(name);
	    if (!ok) return nullptr;
	    return std::dynamic_pointer_cast<T>(it->second);
	 }

	 bool defined(const std::string& name) const {
	    auto [ok, it] = find(name);
	    return ok;
	 }

	 ScopePtr get_outer() const {
	    return outer;
	 }

	 // mutators
	 bool insert(const std::string& name, ObjectPtr object) {
	    auto [it, ok] = objects.insert(std::make_pair(name, object));
	    return ok;
	 }

      private:
	 ScopePtr outer;
	 std::map<std::string, ObjectPtr> objects;
	 using Iterator = std::map<std::string, ObjectPtr>::const_iterator;

	 std::pair<bool, Iterator> find(const std::string& name) const {
	    auto it = objects.find(name);
	    if (it == objects.end()) {
	       if (outer) {
		  return outer->find(name);
	       }
	       return {false, it};
	    }
	    return {true, it};
	 }
   };

} // namespace CSP

#endif
