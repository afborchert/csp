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

#ifndef CSP_CHANNEL_HPP
#define CSP_CHANNEL_HPP

#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <utility>

#include "alphabet.hpp"
#include "object.hpp"

namespace CSP {

   class Channel;
   using ChannelPtr = std::shared_ptr<Channel>;

   class Channel: public Object {
      public:
	 Channel(std::string name) : name(std::move(name)) {}

	 const std::string& get_name() const {
	    return name;
	 }

	 void add_symbol(std::string symbol) {
	    alphabet += message_to_event(symbol);
	 }
	 bool set_alphabet(Alphabet a) {
	    if (alphabet_fixed) return false;
	    std::swap(a, alphabet);
	    alphabet = message_to_event(alphabet);
	    alphabet_fixed = true;
	    return true;
	 }
	 bool set_alphabet(std::string name, Alphabet a) {
	    a = message_to_event(a);
	    auto [it, ok] = alphabets.insert(std::make_pair(name, a));
	    if (ok && !alphabet_fixed) alphabet += a;
	    return ok;
	 }
	 const Alphabet& get_alphabet() const {
	    return alphabet;
	 }
	 Alphabet get_alphabet(std::string name) const {
	    auto it = alphabets.find(name);
	    if (it != alphabets.end()) {
	       return it->second;
	    }
	    return alphabet;
	 }

      private:
	 const std::string name;
	 Alphabet alphabet; // default alphabet
	 bool alphabet_fixed = false; // set_alphabet has been called
	 std::map<std::string, Alphabet> alphabets; // per-process alphabets

	 std::string message_to_event(std::string message) {
	    return name + "." + message;
	 }

	 Alphabet message_to_event(Alphabet alphabet) {
	    Alphabet prefixed;
	    for (auto message: alphabet) {
	       prefixed += message_to_event(message);
	    }
	    return prefixed;
	 }

	 void print(std::ostream& out) const override {
	    out << name;
	 }
   };

} // namespace CSP

#endif

