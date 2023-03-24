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

#ifndef CSP_NAMED_PROCESS_HPP
#define CSP_NAMED_PROCESS_HPP

#include <memory>
#include <string>

#include "channel.hpp"
#include "process.hpp"

namespace CSP {

   class NamedProcess;
   using ConstNamedProcessPtr = std::shared_ptr<const NamedProcess>;
   using NamedProcessPtr = std::shared_ptr<NamedProcess>;

   class NamedProcess: public Process {
      public:
	 NamedProcess(const std::string& name) : name(name) {
	 }
	 const std::string& get_name() const {
	    return name;
	 }

      private:
	 const std::string name;

	 Alphabet get_channel_alphabet(ChannelPtr c) const override {
	    return c->get_alphabet(name);
	 }
   };

} // namespace CSP

#endif
