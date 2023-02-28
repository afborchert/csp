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

#include "object.hpp"
#include "process.hpp"
#include "process-definition.hpp"
#include "scope.hpp"
#include "symtable.hpp"

namespace CSP {

Scope::Scope() : outer(nullptr) {
}
Scope::Scope(ScopePtr outer) : outer(outer) {
}

template<typename Map, typename T>
static bool do_lookup(Map& map, ScopePtr outer,
      const std::string& name, T& object) {
   auto it = map.find(name);
   if (it == map.end()) {
      if (outer) {
	 return outer->lookup(name, object);
      } else {
	 return false;
      }
   } else {
      object = it->second;
      return true;
   }
}

// accessors
bool Scope::lookup(const std::string& name,
      ProcessDefinitionPtr& process) const {
   return do_lookup(processes, outer, name, process);
}

bool Scope::lookup(const std::string& name,
      FunctionDefinitionPtr& function) const {
   return do_lookup(functions, outer, name, function);
}

ScopePtr Scope::get_outer() const {
   return outer;
}

// mutators
bool Scope::insert(ProcessDefinitionPtr process) {
   auto [it, ok] = processes.insert(std::make_pair(process->get_name(),
      process));
   return ok;
}

bool Scope::insert(FunctionDefinitionPtr function) {
   auto [it, ok] = functions.insert(std::make_pair(function->get_name(),
      function));
   return ok;
}

} // namespace CSP
