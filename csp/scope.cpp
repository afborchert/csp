/* 
   Copyright (c) 2011-2017 Andreas F. Borchert
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
#include "scope.hpp"
#include "symtable.hpp"

namespace CSP {

Scope::Scope() : outer(nullptr) {
}
Scope::Scope(ScopePtr outer) : outer(outer) {
}

// accessors
bool Scope::lookup(const std::string& name,
      ProcessDefinitionPtr& process) const {
   ProcMap::const_iterator it = processes.find(name);
   if (it == processes.end()) {
      if (outer) {
	 return outer->lookup(name, process);
      } else {
	 return false;
      }
   } else {
      process = it->second;
      return true;
   }
}

ScopePtr Scope::get_outer() const {
   return outer;
}

// mutators
bool Scope::insert(const ProcessDefinitionPtr& process) {
   std::pair<ProcMap::iterator, bool> result =
      processes.insert(ProcMap::value_type(process->get_name(),
	 process));
   return result.second;
}

} // namespace CSP
