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

#ifndef CSP_ALPHABET_HPP
#define CSP_ALPHABET_HPP

#include <algorithm>
#include <iostream>
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
	    return events.find(event) != events.end();
	 }

	 int cardinality() const {
	    return events.size();
	 }

	 bool operator>=(const Alphabet& other) const {
	    return std::includes(events.begin(), events.end(),
	       other.events.begin(), other.events.end());
	 }

	 bool operator<=(const Alphabet& other) const {
	    return other <= *this;
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

	 /* union */
	 Alphabet operator+(const Alphabet& other) const {
	    Set result;
	    std::insert_iterator<Set> inserter =
	       std::inserter(result, result.end());
	    std::set_union(events.begin(), events.end(),
	       other.events.begin(), other.events.end(),
	       inserter);
	    return Alphabet(result);
	 }

	 /* difference */
	 Alphabet operator-(const Alphabet& other) const {
	    Set result;
	    std::insert_iterator<Set> inserter =
	       std::inserter(result, result.end());
	    std::set_difference(events.begin(), events.end(),
	       other.events.begin(), other.events.end(),
	       inserter);
	    return Alphabet(result);
	 }

	 /* intersection */
	 Alphabet operator*(const Alphabet& other) const {
	    Set result;
	    std::insert_iterator<Set> inserter =
	       std::inserter(result, result.end());
	    std::set_intersection(events.begin(), events.end(),
	       other.events.begin(), other.events.end(),
	       inserter);
	    return Alphabet(result);
	 }

	 /* symmetric difference */
	 Alphabet operator/(const Alphabet& other) const {
	    Set result;
	    std::insert_iterator<Set> inserter =
	       std::inserter(result, result.end());
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
