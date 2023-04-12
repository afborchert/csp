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

#ifndef CSP_ALPHABET_HPP
#define CSP_ALPHABET_HPP

#include <algorithm>
#include <cassert>
#include <cctype>
#include <iostream>
#include <regex>
#include <set>
#include <string>

namespace CSP {

   class Alphabet {
      public:
	 using Set = std::set<std::string>;
	 using Iterator = Set::const_iterator;

	 Alphabet() {
	 }

	 Alphabet(const std::string& event) : events{event} {
	 }

	 Alphabet(const Set& set) : events(set) {
	 }

	 void add(const std::string& event) {
	    events.insert(event);
	 }

	 Iterator begin() const {
	    return events.begin();
	 }

	 Iterator end() const {
	    return events.end();
	 }

	 bool is_member(const std::string& event) const {
	    auto it = events.find(event);
	    if (it != events.end()) return true;

	    std::regex string_regex("^(.*?)\"[^\"]*\"$");
	    std::smatch match;
	    if (std::regex_match(event, match, string_regex) &&
		  match.size() == 2) {
	       auto key = match[1].str() + "*string*";
	       it = events.find(key);
	       return it != events.end();
	    }

	    std::regex int_regex("^(.*?\\.)\\d+$");
	    if (std::regex_match(event, match, int_regex) &&
		  match.size() == 2) {
	       auto key = match[1].str() + "*integer*";
	       it = events.find(key);
	       return it != events.end();
	    }
	    return false;
	 }

	 int cardinality() const {
	    return events.size();
	 }

	 bool operator>=(const Alphabet& other) const {
	    return std::includes(events.begin(), events.end(),
	       other.events.begin(), other.events.end());
	 }

	 bool operator<=(const Alphabet& other) const {
	    return other >= *this;
	 }

	 bool operator==(const Alphabet& other) const {
	    return cardinality() == other.cardinality() &&
	       std::equal(events.begin(), events.end(),
		  other.events.begin());
	 }

	 bool operator!=(const Alphabet& other) const {
	    return !(*this == other);
	 }

	 operator bool() const {
	    return events.size() > 0;
	 }

	 /* inclusion */
	 Alphabet& operator+=(const std::string& event) {
	    add(event);
	    return *this;
	 }
	 Alphabet& operator+=(const Alphabet& a) {
	    events.insert(a.begin(), a.end());
	    return *this;
	 }

	 /* union */
	 Alphabet operator+(const Alphabet& other) const {
	    Set result;
	    auto inserter = std::inserter(result, result.end());
	    std::set_union(events.begin(), events.end(),
	       other.events.begin(), other.events.end(),
	       inserter);
	    return Alphabet(result);
	 }

	 /* difference */
	 Alphabet operator-(const Alphabet& other) const {
	    Set result;
	    auto inserter = std::inserter(result, result.end());
	    std::set_difference(events.begin(), events.end(),
	       other.events.begin(), other.events.end(),
	       inserter);
	    return Alphabet(result);
	 }

	 /* intersection */
	 Alphabet operator*(const Alphabet& other) const {
	    Set result;
	    /* this cannot be done through std::set_intersection
	       in cases where we operate with integers or strings
	       as alphabets */
	    for (auto event: events) {
	       if (other.is_member(event)) {
		  result.insert(event);
	       }
	    }
	    for (auto event: other.events) {
	       if (is_member(event)) {
		  result.insert(event);
	       }
	    }
	    return Alphabet(result);
	 }

	 /* symmetric difference */
	 Alphabet operator/(const Alphabet& other) const {
	    Set result;
	    auto inserter = std::inserter(result, result.end());
	    std::set_symmetric_difference(events.begin(), events.end(),
	       other.events.begin(), other.events.end(),
	       inserter);
	    return Alphabet(result);
	 }
      private:
	 Set events;
   };

   inline std::ostream& operator<<(std::ostream& out,
	 const Alphabet& alphabet) {
      out << '{';
      bool first = true;
      for (auto& event: alphabet) {
	 if (first) {
	    first = false;
	 } else {
	    out << ", ";
	 }
	 out << event;
      }
      out << '}';
      return out;
   }

} // namespace CSP

#endif
