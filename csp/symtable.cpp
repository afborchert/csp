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

#include <cassert>
#include <cstdlib>
#include <memory>

#include "object.hpp"
#include "process.hpp"
#include "process-definition.hpp"
#include "process-reference.hpp"
#include "scope.hpp"
#include "symtable.hpp"

namespace CSP {

SymTable::SymTable() : scope(nullptr) {
}

// accessors
bool SymTable::lookup(const std::string name, ProcessPtr& process) const {
   ProcessDefinitionPtr pdef;
   if (scope) {
      bool found = scope->lookup(name, pdef);
      if (found) process = pdef;
      return found;
   } else {
      return false;
   }
}

bool SymTable::lookup(const std::string name, FunctionDefinitionPtr& f) {
   FunctionDefinitionPtr fdef;
   if (scope) {
      bool found = scope->lookup(name, fdef);
      if (!found) {
	 fdef = std::make_shared<FunctionDefinition>(name);
	 bool ok = insert(fdef); assert(ok);
      }
      f = fdef;
      return true;
   } else {
      return false;
   }
}

// mutators
void SymTable::open() {
   ScopePtr inner = std::make_shared<Scope>(scope);
   scope = inner;
}

void SymTable::close() {
   assert(scope);
   /* resolve all process references,
      this is required in cases of mutual recursion */
   for (auto pref: unresolved) {
      if (!pref->resolve()) {
	 std::cerr << "unable to resolve " << pref->get_name() << std::endl;
	 exit(1);
      }
   }
   ScopePtr outer = scope->get_outer();
   scope = outer;
}

bool SymTable::insert(ProcessDefinitionPtr process) {
   assert(scope);
   return scope->insert(process);
}
bool SymTable::insert(FunctionDefinitionPtr f) {
   assert(scope);
   return scope->insert(f);
}
void SymTable::add_unresolved(ProcessReferencePtr pref) {
   assert(pref);
   unresolved.push_back(pref);
}

} // namespace CSP
