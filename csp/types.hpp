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

#ifndef CSP_TYPES_HPP
#define CSP_TYPES_HPP

#include <memory>

namespace CSP {

   class Event;
   class EventSet;
   class Object;
   class PrefixedProcess;
   class Process;
   class ProcessDefinition;
   class ProcessReference;
   class Scope;
   class SymTable;
   using ConstProcessPtr = std::shared_ptr<const Process>;
   using EventPtr = std::shared_ptr<Event>;
   using EventSetPtr = std::shared_ptr<EventSet>;
   using ObjectPtr = std::shared_ptr<Object>;
   using PrefixedProcessPtr = std::shared_ptr<PrefixedProcess>;
   using ProcessDefinitionPtr = std::shared_ptr<ProcessDefinition>;
   using ProcessPtr = std::shared_ptr<Process>;
   using ProcessReferencePtr = std::shared_ptr<ProcessReference>;
   using ScopePtr = std::shared_ptr<Scope>;

} // namespace CSP

#endif
