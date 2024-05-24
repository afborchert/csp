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

#ifndef CSP_STATUS_HPP
#define CSP_STATUS_HPP

#include <cassert>
#include <cstdlib>
#include <memory>
#include <vector>
#include <utility>

#include "scope.hpp"
#include "uniformint.hpp"

/*
   status objects maintain runtime information which include
    - bound variables and processes
    - storage of decisions made by non-deterministic processes
*/

namespace CSP {

   class Status;
   using StatusPtr = std::shared_ptr<Status>;

   class Status {
      public:
	 Status() :
	       scope(std::make_shared<Scope>()),
	       prg(std::make_shared<UniformIntDistribution>()) {
	 }
	 Status(StatusPtr status) :
	       scope(std::make_shared<Scope>(status->scope)),
	       extended(status->extended),
	       prg(status->prg) {
	 }

	 virtual ~Status() {}

	 template <typename T>
	 auto lookup(const std::string& name) const {
	    auto object = scope->lookup<T>(name);
	    assert(object);
	    return object;
	 }

	 void set(const std::string& name, ObjectPtr object) {
	    bool ok = scope->insert(name, object);
	    assert(ok);
	 }

	 auto draw(unsigned int upper_limit) {
	    return prg->draw(upper_limit);
	 }
	 bool flip() {
	    return prg->flip();
	 }

      private:
	 template<typename T, typename... Args>
	 friend std::shared_ptr<T> get_status(StatusPtr status, Args&&... args);

	 ScopePtr scope; // for bound variables and processes
	 StatusPtr extended; // managed by get_status
	 std::shared_ptr<UniformIntDistribution> prg;
   };

   /* access extended status and create it for status, if it
      does not exist yet */
   template<typename T, typename... Args>
   std::shared_ptr<T> get_status(StatusPtr status, Args&&... args) {
      std::shared_ptr<T> s = std::dynamic_pointer_cast<T>(status);
      if (s) return s;
      if (status->extended) {
	 std::shared_ptr<T> s = std::dynamic_pointer_cast<T>(status->extended);
	 if (s) return s;
      }
      auto extended = std::make_shared<T>(status, std::forward<Args>(args)...);
      status->extended = extended;
      return extended;
   }

} // namespace CSP

#endif
