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

#ifndef CSP_PROCESS_HPP
#define CSP_PROCESS_HPP

#include <cassert>
#include <deque>
#include <iostream>
#include <memory>
#include <string>
#include <utility>

/* we need to declare these first
   as these types are required by some of the headers below */

namespace CSP {
   class Process;
   using ConstProcessPtr = std::shared_ptr<const Process>;
   using ProcessPtr = std::shared_ptr<Process>;
}

#include "alphabet.hpp"
#include "channel.hpp"
#include "object.hpp"
#include "status.hpp"
#include "uniformint.hpp"

namespace CSP {

   using ActiveProcess = std::pair<ProcessPtr, StatusPtr>;

   /*
      Instances of this class represent CSP processes.
   */
   class Process: public Object, public std::enable_shared_from_this<Process> {
      public:
	 // default constructor
	 Process() {}

	 /* attempts to engage in the given event and
	    returns a process which accepted that event;
	    null is returned if the event was not accepted;
	    the same process is returned if the event
	    does not belong to the alphabet of this process */
	 ActiveProcess proceed(const std::string& event, StatusPtr status) {
	    if (get_alphabet().is_member(event)) {
	       /* use internal polymorphic function
	          to process this event */
	       return internal_proceed(event, status);
	    } else {
	       /* if it is not in our alphabet
		  we are not interested in it */
	       return {shared_from_this(), status};
	    }
	 }

	 /* retrieve the set of symbols which would be
	    accepted next by this process;
	    the empty set is returned in case of STOP */
	 virtual Alphabet acceptable(StatusPtr status) const = 0;

	 /* returns true iff success is accepted,
	    i.e. in case of a SKIP process */
	 bool accepts_success(StatusPtr status) const {
	    return acceptable(status).is_member("_success_");
	 }

	 /* retrieve the alphabet of this process;
	    the alphabet, if not explicitly set, is implicitly
	    derived from the set of mutual recursive equations
	    defining it */
	 const Alphabet& get_alphabet() const {
	    if (!dependencies_initialized) {
	       dependencies_initialized = true;
	       initialize_dependencies();
	    }
	    if (!alphabet_initialized) {
	       alphabet_initialized = true;
	       for (auto c: channels) {
		  alphabet += get_channel_alphabet(c);
	       }
	       /* do not propagate implicitly success as member
		  of the alphabet */
	       propagate_alphabet(internal_get_alphabet() -
		  Alphabet("_success_"));
	       alphabet = map_alphabet(alphabet);
	    }
	    return alphabet;
	 }

	 /* set explicitly the alphabet of this process */
	 void set_alphabet(const Alphabet& new_alphabet) {
	    /* do not synchronize this yet as this
	       function is called by the parser at
	       a time when forward references have
	       not been resolved yet */
	    assert(new_alphabet.cardinality() > 0);
	    alphabet = new_alphabet;
	    alphabet_fixed = true;
	    alphabet_initialized = true;
	 }

	 /* add a process to the list of dependants whose
	    alphabet depends on this process */
	 void add_dependant(ConstProcessPtr p) const {
	    dependants.push_back(p);
	 }

	 virtual void add_channel(ChannelPtr c) const {
	    channels.push_back(c);
	 }

      private:
	 /* internal implementation of proceed
	    which no longer needs to check if event belongs to
	    our alphabet and that depends on the actual process */
	 virtual ActiveProcess internal_proceed(const std::string& event,
	    StatusPtr status) = 0;

	 /* construct initial alphabet */
	 virtual Alphabet internal_get_alphabet() const = 0;

	 mutable Alphabet alphabet;
	 bool alphabet_fixed = false; // changed only by set_alphabet
	 mutable bool alphabet_initialized = false;
	 mutable bool dependencies_initialized = false;
	 mutable std::deque<ConstProcessPtr> dependants;
	 // channels this process depends on
	 mutable std::deque<ChannelPtr> channels;

	 virtual Alphabet get_channel_alphabet(ChannelPtr c) const {
	    return c->get_alphabet();
	 }

	 /* set implicitly the alphabet of this process;
	    this is suppressed if the alphabet was explicitly
	    set before */
	 void propagate_alphabet(const Alphabet& new_alphabet) const {
	    if (!alphabet_fixed) {
	       if (new_alphabet - alphabet) {
		  alphabet = alphabet + new_alphabet;
		  for (auto& d: dependants) {
		     d->propagate_alphabet(map_alphabet(alphabet));
		  }
	       }
	    }
	 }

	 virtual Alphabet map_alphabet(const Alphabet& alphabet) const {
	    return alphabet;
	 }

	 virtual void initialize_dependencies() const {
	 }
   };

   inline std::ostream& operator<<(std::ostream& out, ProcessPtr p) {
      p->expanded_print(out); return out;
   }

   inline std::ostream& operator<<(std::ostream& out, ConstProcessPtr p) {
      p->expanded_print(out); return out;
   }

} // namespace CSP

#endif
