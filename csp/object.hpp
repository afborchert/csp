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

/*
   We need a unified type for all possible values returned
   by a production of the parser. This is ObjectPtr, i.e.
   a std::shared_ptr to the polymorphic base type Object.

   Derivations include:
      - events and event sets
      - processes

   All objects support a print method which comes in two
   variants:
      - print            brief output with an implementation that
                         guards itself against endless recursion
      - expanded_print   more elaborate output but the implementation
                         shall invoke print for descendent objects
			 to avoid an endless recursion

   By default, expanded_print is implemented as print. The only
   exception is currently ProcessReference which gives the process
   name only in case of print and delivers a full expansion in
   case of expanded_print.
*/

#ifndef CSP_OBJECT_HPP
#define CSP_OBJECT_HPP

#include <iostream>
#include <memory>

namespace CSP {

   class Object;
   using ObjectPtr = std::shared_ptr<Object>;

   class Object: public std::enable_shared_from_this<Object> {
      friend std::ostream& operator<<(std::ostream& out,
	 const ObjectPtr& object);
      public:
	 /* print method that is safe in regard to recursion */
	 virtual void print(std::ostream& out) const = 0;

	 /* expanded print method that must not be used recursively;
	    by default identical to the regular print method
	 */
	 virtual void expanded_print(std::ostream& out) const {
	    print(out);
	 }
   };

   inline std::ostream& operator<<(std::ostream& out,
	 const ObjectPtr& object) {
      object->expanded_print(out); return out;
   }

} // namespace CSP

#endif
