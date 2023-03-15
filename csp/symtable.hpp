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

#ifndef CSP_SYMTABLE_HPP
#define CSP_SYMTABLE_HPP

#include <cassert>
#include <cstdlib>
#include <deque>
#include <functional>
#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <utility>

#include "scope.hpp"
#include "symbol-changer.hpp"

namespace CSP {

   class SymTable {
      private:
	 ScopePtr scope;
	 ScopePtr global;
	 struct Reference {
	    Reference(std::string name, std::function<bool()> resolve) :
		  name(std::move(name)), resolve(std::move(resolve)) {
	    }
	    const std::string name;
	    std::function<bool()> resolve;
	 };
	 std::deque<Reference> unresolved;

      public:
	 // constructors
	 SymTable() = default;

	 // accessors
	 template <typename T>
	 std::shared_ptr<T> lookup(const std::string& name) const {
	    if (scope) {
	       return scope->lookup<T>(name);
	    } else {
	       return global->lookup<T>(name);
	    }
	 }

	 // mutators
	 void open() {
	    auto inner = std::make_shared<Scope>(scope);
	    if (!scope) global = inner;
	    scope = inner;
	 }
	 void close() {
	    assert(scope);
	    /* resolve all references if we are closing
	       the out-most scope, this is required in cases
	       of mutual recursion */
	    std::deque<Reference> survivors;
	    for (auto ref: unresolved) {
	       if (!ref.resolve()) {
		  survivors.push_back(ref);
	       }
	    }
	    std::swap(unresolved, survivors);
	    ScopePtr outer = scope->get_outer();
	    if (!outer) {
	       /* give error messages for all unresolved names */
	       unsigned errors = 0;
	       for (auto ref: unresolved) {
		  std::cerr << "unable to resolve " <<
		     ref.name << std::endl;
		  ++errors;
	       }
	       if (errors) {
		  std::exit(1);
	       }
	    }
	    scope = outer;
	 }

	 bool insert(const std::string& name, ObjectPtr object) {
	    assert(scope);
	    return scope->insert(name, object);
	 }
	 bool global_insert(const std::string& name, ObjectPtr object) {
	    assert(global);
	    return global->insert(name, object);
	 }
	 void add_unresolved(std::string name, std::function<bool()> resolve) {
	    unresolved.emplace_back(std::move(name), std::move(resolve));
	 }
   };

} // namespace CSP

#endif
