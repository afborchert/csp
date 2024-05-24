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

#ifndef CSP_EXPRESSION_HPP
#define CSP_EXPRESSION_HPP

#include <cassert>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include <utility>

#include "context.hpp"
#include "error.hpp"
#include "location.hh"
#include "object.hpp"
#include "status.hpp"

namespace CSP {

   class Expression;
   using ExpressionPtr = std::shared_ptr<Expression>;
   using Value = unsigned long;

   class Expression: public Object {
      public:
	 virtual Value eval(StatusPtr status) const = 0;
   };

   inline std::ostream& operator<<(std::ostream& out,
	 const ExpressionPtr& expr) {
      expr->print(out); return out;
   }

   class Variable: public Expression {
      public:
	 Variable(const location& loc, Context& context,
		  const std::string& varname) :
	       loc(loc), context(context), varname(varname) {
	 }
	 Value eval(StatusPtr status) const override {
	    auto sval = status->lookup<Identifier>(varname)->get_name();
	    const char* s = sval.c_str();
	    char* endptr;
	    auto val = std::strtoul(s, &endptr, 10);
	    if (*endptr) {
	       yyerror(loc, context,
		  "bound variable %s is not of integer type");
	       return 0;
	    }
	    return val;
	 }
	 void print(std::ostream& out) const override {
	    out << varname;
	 }
      private:
	 const location loc;
	 Context& context;
	 std::string varname;
   };

   class Integer: public Expression {
      public:
	 Integer(Value value) : value(value) {
	 }
	 Value eval(StatusPtr status) const override {
	    return value;
	 }
	 void print(std::ostream& out) const override {
	    out << value;
	 }

      private:
	 Value value;
   };

   class Binary: public Expression {
      public:
	 template<typename F>
	 Binary(ExpressionPtr left, ExpressionPtr right,
	       std::string opsym, F&& f) :
	       left(left), right(right), opsym(opsym), f(std::forward<F>(f)) {
	 }
	 Value eval(StatusPtr status) const override {
	    auto v1 = left->eval(status);
	    auto v2 = right->eval(status);
	    return f(v1, v2);
	 }
	 void print(std::ostream& out) const override {
	    out << "(" << left << " " << opsym << " " << right << ")";
	 }

      private:
	 ExpressionPtr left;
	 ExpressionPtr right;
	 std::string opsym;
	 std::function<Value(Value, Value)> f;
   };

} // namespace CSP

#endif
