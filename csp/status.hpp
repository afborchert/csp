/* 
   Copyright (c) 2011-2023 Andreas F. Borchert
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
	 ScopePtr scope;
	 std::shared_ptr<UniformIntDistribution> prg;
   };

   template<typename T, typename... Args>
   std::shared_ptr<T> get_status(StatusPtr status, Args&&... args) {
      std::shared_ptr<T> s = std::dynamic_pointer_cast<T>(status);
      if (s) return s;
      return std::make_shared<T>(status, std::forward<Args>(args)...);
   }

} // namespace CSP

#endif
