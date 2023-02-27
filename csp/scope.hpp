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

#ifndef CSP_SCOPE_HPP
#define CSP_SCOPE_HPP

#include <map>
#include <memory>
#include <string>
#include "process-definition.hpp"

namespace CSP {

   class Scope;
   using ScopePtr = std::shared_ptr<Scope>;

   class Scope {
      private:
	 using ProcMap = std::map<std::string, ProcessDefinitionPtr>;
	 ProcMap processes;
	 ScopePtr outer;
      public:
	 // constructors
	 Scope();
	 Scope(ScopePtr outer);

	 // accessors
	 bool lookup(const std::string& name,
	       ProcessDefinitionPtr& process) const;
	 ScopePtr get_outer() const;

	 // mutators
	 bool insert(const ProcessDefinitionPtr& process);
   };

} // namespace CSP

#endif
