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

#ifndef CSP_SYMBOL_CHANGER_HPP
#define CSP_SYMBOL_CHANGER_HPP

#include <cassert>
#include <map>
#include <memory>
#include <sstream>
#include <string>

#include "alphabet.hpp"
#include "identifier.hpp"
#include "object.hpp"

/* function that maps symbols, see CSP 2.6 */

namespace CSP {

   class SymbolChanger;
   using SymbolChangerPtr = std::shared_ptr<SymbolChanger>;
   class FunctionDefinition;
   using FunctionDefinitionPtr = std::shared_ptr<FunctionDefinition>;

   class SymbolChanger: public Object {
      public:
	 std::string map(std::string event) {
	    if (event[0] == '_') {
	       return event;
	    } else {
	       return internal_map(event);
	    }
	 }
	 Alphabet map(Alphabet a) {
	    Alphabet changed_a;
	    for (auto symbol: a) {
	       changed_a.add(map(symbol));
	    }
	    return changed_a;
	 }

	 std::string reverse_map(std::string event) {
	    if (event[0] == '_') {
	       return event;
	    } else {
	       return internal_reverse_map(event);
	    }
	 }

	 virtual std::string get_name(std::string name) = 0;

      private:
	 virtual std::string internal_map(std::string) = 0;
	 virtual std::string internal_reverse_map(std::string) = 0;
   };

   /* see CSP 2.6 */
   class FunctionDefinition: public SymbolChanger {
      public:
	 FunctionDefinition(std::string name) : name(name) {
	 }
	 bool add_mapping(std::string event1, std::string event2) {
	    auto [it1, ok1] = map.insert(std::make_pair(event1, event2));
	    if (!ok1) return false;
	    auto [it2, ok2] = reversed_map.insert(std::make_pair(event2,
	       event1));
	    if (!ok2) {
	       map.erase(it1); return false;
	    }
	    return true;
	 }
	 bool add_mapping(IdentifierPtr event1, IdentifierPtr event2) {
	    return add_mapping(event1->get_name(), event2->get_name());
	 }

	 std::string get_name() {
	    return name;
	 }

	 void print(std::ostream& out) const final {
	    for (auto [event1, event2]: map) {
	       out << name << "(" << event1 << ") = " << event2 << std::endl;
	    }
	 }

      private:
	 std::string name;
	 std::map<std::string, std::string> map;
	 std::map<std::string, std::string> reversed_map;

	 std::string get_name(std::string name) final {
	    return this->name + "(" + name + ")";
	 }

	 std::string internal_map(std::string event) final {
	    auto it = map.find(event);
	    if (it != map.end()) {
	       return it->second;
	    } else {
	       return event;
	    }
	 }

	 std::string internal_reverse_map(std::string event) final {
	    auto it = reversed_map.find(event);
	    if (it != reversed_map.end()) {
	       return it->second;
	    } else {
	       return event;
	    }
	 }
   };

   /* see CSP 2.6.1 */
   class Inverse: public SymbolChanger {
      public:
	 Inverse(SymbolChangerPtr f) : f(f) {
	    assert(f);
	    auto f_ = std::dynamic_pointer_cast<Inverse>(f);
	    if (f_) {
	       f = f_->f;
	    }
	 }

	 void print(std::ostream& out) const final {
	    f->print(out); out << "^-1";
	 }

      private:
	 SymbolChangerPtr f;

	 std::string get_name(std::string name) final {
	    return f->get_name(name) + "^-1";
	 }

	 std::string internal_map(std::string event) final {
	    return f->reverse_map(event);
	 }

	 std::string internal_reverse_map(std::string event) final {
	    return f->map(event);
	 }
   };

   /* see CSP 2.6.2 */
   class Qualifier: public SymbolChanger {
      public:
	 Qualifier(std::string label) : label(label) {
	 }

	 void print(std::ostream& out) const final {
	    out << "f_" << label;
	 }

      private:
	 std::string label;

	 std::string get_name(std::string name) final {
	    return label + ":" + name;
	 }

	 std::string internal_map(std::string event) final {
	    return label + "." + event;
	 }

	 std::string internal_reverse_map(std::string event) final {
	    return event.substr(label.size() + 1);
	 }
   };

   /* see CSP 4.4 where we need to map right and left for pipe
      to an internal hidden name */
   class MapChannel: public SymbolChanger {
      public:
	 MapChannel(std::string channel, std::string newname) :
	       channel(channel), newname(newname) {
	 }
	 void print(std::ostream& out) const final {
	    out << "mapping " << channel << " -> " << newname;
	 }

      private:
	 std::string channel;
	 std::string newname;

	 std::string get_name(std::string name) final {
	    return name;
	 }

	 std::string map_channel(const std::string& event,
	       const std::string& channel,
	       const std::string& newname) {
	    auto channel_len = channel.size();
	    auto event_len = event.size();
	    if (event_len <= channel_len + 1) return event;
	    if (event.substr(0, channel_len) == channel &&
		  event[channel_len] == '.') {
	       return newname + event.substr(channel_len);
	    }
	    return event;
	 }

	 std::string internal_map(std::string event) final {
	    return map_channel(event, channel, newname);
	 }

	 std::string internal_reverse_map(std::string event) final {
	    return map_channel(event, newname, channel);
	 }
   };

} // namespace CSP

#endif
