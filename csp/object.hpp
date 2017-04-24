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

#ifndef CSP_OBJECT_HPP
#define CSP_OBJECT_HPP

#include <cassert>
#include <cstdlib>
#include <iostream>
#include <memory>
#include "alphabet.hpp"
#include "symtable.hpp"
#include "types.hpp"
#include "uniformint.hpp"

namespace CSP {

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

   class Event: public Object {
      public:
	 Event(const std::string& name) : name(name) {
	 }
	 const std::string& get_name() const {
	    return name;
	 }
	 virtual void print(std::ostream& out) const {
	    out << name;
	 }
      private:
	 const std::string name;
   };

   class EventSet: public Object {
      public:
	 EventSet() {
	 }
	 EventSet(const Alphabet& alphabet) : alphabet(alphabet) {
	 }
	 EventSet(const std::string& name) {
	    alphabet.add(name);
	 }
	 virtual void print(std::ostream& out) const {
	    out << alphabet;
	 }
	 const Alphabet& get_alphabet() const {
	    return alphabet;
	 }
      private:
	 Alphabet alphabet;
   };

   /*
      Instances of this class represent CSP processes.
   */
   class Process: public Object {
      public:
	 // default constructor
	 Process() :
	    alphabet_fixed(false), alphabet_initialized(false),
	    dependencies_initialized(false) {
	 }

	 /* attempts to engage in the given event and
	    returns a process which accepted that event;
	    null is returned if the event was not accepted;
	    the same process is returned if the event
	    does not belong to the alphabet of this process */
	 ProcessPtr proceed(std::string& event) {
	    if (get_alphabet().is_member(event)) {
	       /* use internal polymorphic function
	          to process this event */
	       return internal_proceed(event);
	    } else {
	       /* if it is not in our alphabet
		  we are not interested in it */
	       return std::dynamic_pointer_cast<Process>(shared_from_this());
	    }
	 }

	 /* retrieve the set of symbols which would be
	    accepted next by this process;
	    the empty set is returned in case of STOP */
	 virtual Alphabet acceptable() const = 0;

	 /* returns true iff success is accepted,
	    i.e. in case of a SKIP process */
	 bool accepts_success() const {
	    return acceptable().is_member("_success_");
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
	       /* do not propagate implicitly success as member
		  of the alphabet */
	       propagate_alphabet(internal_get_alphabet() -
		  Alphabet("_success_"));
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

	 /* set implicitly the alphabet of this process;
	    this is suppressed if the alphabet was explicitly
	    set before */
	 void propagate_alphabet(const Alphabet& new_alphabet) const {
	    if (!alphabet_fixed) {
	       if (new_alphabet - alphabet) {
		  alphabet = alphabet + new_alphabet;
		  for (ProcessList::iterator it = dependants.begin();
			it != dependants.end(); ++ it) {
		     (*it)->propagate_alphabet(alphabet);
		  }
	       }
	    }
	 }

	 /* add a process to the list of dependants whose
	    alphabet depends on this process */
	 void add_dependant(ConstProcessPtr p) {
	    dependants.push_back(p);
	 }

      protected:
	 /* internal implementation of proceed
	    which no longer needs to check if event belongs to
	    our alphabet and that depends on the actual process */
	 virtual ProcessPtr internal_proceed(std::string& event) = 0;

	 /* construct initial alphabet */
	 virtual Alphabet internal_get_alphabet() const = 0;

      private:
	 mutable Alphabet alphabet;
	 bool alphabet_fixed; // do not change it
	 mutable bool alphabet_initialized;
	 mutable bool dependencies_initialized;
	 typedef std::list<ConstProcessPtr> ProcessList;
	 mutable ProcessList dependants;
	 virtual void initialize_dependencies() const {
	 }
   };

   inline std::ostream& operator<<(std::ostream& out,
	 const ProcessPtr& p) {
      ObjectPtr object(p);
      object->expanded_print(out); return out;
   }

   class ProcessDefinition: public Process {
      public:
	 ProcessDefinition(const std::string& name, ProcessPtr process) :
	       name(name), process(process) {
	    assert(process);
	 }
	 const std::string& get_name() const {
	    return name;
	 }
	 virtual void print(std::ostream& out) const {
	    out << name << " = "; process->print(out);
	 }
	 virtual Alphabet acceptable() const {
	    return process->acceptable();
	 }
      protected:
	 virtual ProcessPtr internal_proceed(std::string& event) {
	    return process->proceed(event);
	 }
	 virtual Alphabet internal_get_alphabet() const {
	    return process->get_alphabet();
	 }
      private:
	 const std::string name;
	 ProcessPtr process;
	 virtual void initialize_dependencies() const {
	    process->add_dependant(std::dynamic_pointer_cast<const Process>(
	       shared_from_this()));
	 }
   };

   class ProcessReference: public Process {
      public:
	 ProcessReference(const std::string& name, SymTable& symtab) :
	       name(name), symtab(symtab) {
	 }
	 const std::string& get_name() const {
	    return name;
	 }
	 void register_ref() {
	    if (!resolve()) {
	       symtab.add_unresolved(
		  std::dynamic_pointer_cast<ProcessReference>(
		     shared_from_this()));
	    }
	 }
	 bool resolve() const {
	    if (p) return true;
	    if (!symtab.lookup(name, p)) return false;
	    return true;
	 }
	 virtual void print(std::ostream& out) const {
	    out << name;
	 }
	 virtual void expanded_print(std::ostream& out) const {
	    if (!p) resolve();
	    if (p) {
	       p->print(out);
	    } else {
	       out << name;
	    }
	 }
	 virtual Alphabet acceptable() const {
	    if (!p) resolve();
	    if (p) {
	       return p->acceptable();
	    } else {
	       return Alphabet();
	    }
	 }
      protected:
	 virtual ProcessPtr internal_proceed(std::string& event) {
	    if (p) {
	       return p->proceed(event);
	    } else {
	       return nullptr;
	    }
	 }
	 virtual Alphabet internal_get_alphabet() const {
	    if (!p) resolve();
	    if (p) {
	       return p->get_alphabet();
	    } else {
	       return Alphabet();
	    }
	 }
      private:
	 const std::string name;
	 SymTable& symtab;
	 mutable ProcessPtr p;
	 virtual void initialize_dependencies() const {
	    if (!p) {
	       resolve();
	       if (!p) return;
	    }
	    p->add_dependant(std::dynamic_pointer_cast<const Process>(
	       shared_from_this()));
	 }
   };

   class PrefixedProcess: public Process {
      public:
	 PrefixedProcess(const std::string& event, ProcessPtr process) :
	       event(event), process(process) {
	    assert(process);
	 }
	 const std::string& get_event() {
	    return event;
	 }
	 virtual void print(std::ostream& out) const {
	    out << event << " -> "; process->print(out);
	 }
	 virtual Alphabet acceptable() const {
	    return Alphabet(event);
	 }
      protected:
	 virtual ProcessPtr internal_proceed(std::string& next_event) {
	    if (event == next_event) {
	       return process;
	    } else {
	       return nullptr;
	    }
	 }
	 virtual Alphabet internal_get_alphabet() const {
	    return Alphabet(event) + process->get_alphabet();
	 }
      private:
	 const std::string event;
	 ProcessPtr process;
	 virtual void initialize_dependencies() const {
	    process->add_dependant(std::dynamic_pointer_cast<const Process>(
	       shared_from_this()));
	 }
   };

   class SelectingProcess: public Process {
      public:
	 SelectingProcess(ProcessPtr process, ProcessPtr pp) :
	       choice(pp), other_choices(process) {
	    assert(other_choices);
	    assert(choice);
	 }
	 virtual void print(std::ostream& out) const {
	    other_choices->print(out); out << " | "; choice->print(out);
	 }
	 virtual Alphabet acceptable() const {
	    return choice->acceptable() + other_choices->acceptable();
	 }
      protected:
	 virtual ProcessPtr internal_proceed(std::string& event) {
	    ProcessPtr p = other_choices->proceed(event);
	    if (!p) {
	       p = choice->proceed(event);
	    }
	    return p;
	 }
	 virtual Alphabet internal_get_alphabet() const {
	    return choice->get_alphabet() + other_choices->get_alphabet();
	 }
      private:
	 ProcessPtr choice;
	 ProcessPtr other_choices;
	 virtual void initialize_dependencies() const {
	    choice->add_dependant(std::dynamic_pointer_cast<const Process>(
	       shared_from_this()));
	    other_choices->add_dependant(
	       std::dynamic_pointer_cast<const Process>(shared_from_this()));
	 }
   };

   class ParallelProcesses: public Process {
      public:
	 ParallelProcesses(ProcessPtr p1, ProcessPtr p2) :
	       process1(p1), process2(p2) {
	    assert(process1); assert(process2);
	 }
	 virtual void print(std::ostream& out) const {
	    process1->print(out); out << " || "; process2->print(out);
	 }
	 virtual Alphabet acceptable() const {
	    /* events are acceptable either
	         if they are accepted by both, or
		 if they do belong to the alphabet of one process only
		    and are acceptable by the corresponding process
	    */
	    Alphabet sd = process1->get_alphabet() / process2->get_alphabet();
	    Alphabet p1a = process1->acceptable();
	    Alphabet p2a = process2->acceptable();
	    Alphabet ex1 = sd * p1a;
	    Alphabet ex2 = sd * p2a;
	    return p1a * p2a + ex1 + ex2;
	 }
      protected:
	 virtual ProcessPtr internal_proceed(std::string& event) {
	    ProcessPtr p1 = process1->proceed(event);
	    ProcessPtr p2 = process2->proceed(event);
	    if (p1 && p2) {
	       return std::make_shared<ParallelProcesses>(p1, p2);
	    } else {
	       return nullptr;
	    }
	 };
	 virtual Alphabet internal_get_alphabet() const {
	    return process1->get_alphabet() + process2->get_alphabet();
	 }
      private:
	 ProcessPtr process1;
	 ProcessPtr process2;
   };

   class InterleavingProcesses: public Process {
      public:
	 InterleavingProcesses(ProcessPtr p, ProcessPtr q) :
	       process1(p), process2(q) {
	    assert(process1);
	    assert(process2);
	 }
	 virtual void print(std::ostream& out) const {
	    process1->print(out); out << " ||| "; process2->print(out);
	 }
	 virtual Alphabet acceptable() const {
	    return process1->acceptable() + process2->acceptable();
	 }
      protected:
	 virtual ProcessPtr internal_proceed(std::string& event) {
	    Alphabet a1 = process1->acceptable();
	    Alphabet a2 = process2->acceptable();
	    bool ok1 = a1.is_member(event);
	    bool ok2 = a2.is_member(event);
	    if (ok1 && ok2) {
	       if (prg.flip()) {
		  ok1 = false;
	       } else {
		  ok2 = false;
	       }
	    }
	    if (ok1) {
	       return std::make_shared<InterleavingProcesses>(
			process1->proceed(event), process2);
	    } else if (ok2) {
	       return std::make_shared<InterleavingProcesses>(
			process1, process2->proceed(event));
	    } else {
	       return nullptr;
	    }
	 }
	 virtual Alphabet internal_get_alphabet() const {
	    return process1->get_alphabet() + process2->get_alphabet();
	 }
      private:
	 ProcessPtr process1;
	 ProcessPtr process2;
	 UniformIntDistribution prg;
	 virtual void initialize_dependencies() const {
	    process1->add_dependant(std::dynamic_pointer_cast<const Process>(
	       shared_from_this()));
	    process2->add_dependant(
	       std::dynamic_pointer_cast<const Process>(shared_from_this()));
	 }
   };

   class ExternalChoice: public Process {
      public:
	 ExternalChoice(ProcessPtr p, ProcessPtr q) :
	       process1(p), process2(q) {
	    assert(process1);
	    assert(process2);
	 }
	 virtual void print(std::ostream& out) const {
	    process1->print(out); out << " [] "; process2->print(out);
	 }
	 virtual Alphabet acceptable() const {
	    /* if we get asked, we make up our mind */
	    return process1->acceptable() + process2->acceptable();
	 }
      protected:
	 virtual ProcessPtr internal_proceed(std::string& event) {
	    Alphabet a1 = process1->acceptable();
	    Alphabet a2 = process2->acceptable();
	    bool ok1 = a1.is_member(event);
	    bool ok2 = a2.is_member(event);
	    if (ok1 && ok2) {
	       if (prg.flip()) {
		  ok1 = false;
	       } else {
		  ok2 = false;
	       }
	    }
	    if (ok1) {
	       return process1->proceed(event);
	    } else {
	       return process2->proceed(event);
	    }
	 }
	 virtual Alphabet internal_get_alphabet() const {
	    return process1->get_alphabet() + process2->get_alphabet();
	 }
      private:
	 /* if asked, we tell our next decision */
	 ProcessPtr process1;
	 ProcessPtr process2;
	 UniformIntDistribution prg;
	 virtual void initialize_dependencies() const {
	    process1->add_dependant(std::dynamic_pointer_cast<const Process>(
	       shared_from_this()));
	    process2->add_dependant(
	       std::dynamic_pointer_cast<const Process>(shared_from_this()));
	 }
   };

   class InternalChoice: public Process {
      public:
	 InternalChoice(ProcessPtr p, ProcessPtr q) :
	       nextmove(undecided), process1(p), process2(q) {
	    assert(process1);
	    assert(process2);
	 }
	 virtual void print(std::ostream& out) const {
	    process1->print(out); out << " |~| "; process2->print(out);
	 }
	 virtual Alphabet acceptable() const {
	    /* if we get asked, we make up our mind */
	    decide();
	    if (nextmove == headforp1) {
	       return process1->acceptable();
	    } else {
	       return process2->acceptable();
	    }
	 }
      protected:
	 virtual ProcessPtr internal_proceed(std::string& event) {
	    decide();
	    if (nextmove == headforp1) {
	       nextmove = undecided;
	       return process1->proceed(event);
	    } else {
	       nextmove = undecided;
	       return process2->proceed(event);
	    }
	 }
	 virtual Alphabet internal_get_alphabet() const {
	    return process1->get_alphabet() + process2->get_alphabet();
	 }
      private:
	 /* if asked, we tell our next decision */
	 mutable enum {undecided, headforp1, headforp2} nextmove;
	 mutable UniformIntDistribution prg;
	 ProcessPtr process1;
	 ProcessPtr process2;
	 void decide() const {
	    if (nextmove == undecided) {
	       if (prg.flip()) {
		  nextmove = headforp1;
	       } else {
		  nextmove = headforp2;
	       }
	    }
	 }
	 virtual void initialize_dependencies() const {
	    process1->add_dependant(std::dynamic_pointer_cast<const Process>(
	       shared_from_this()));
	    process2->add_dependant(
	       std::dynamic_pointer_cast<const Process>(shared_from_this()));
	 }
   };

   class ProcessSequence: public Process {
      public:
	 ProcessSequence(ProcessPtr p, ProcessPtr q) :
	       process1(p), process2(q) {
	    assert(process1);
	    assert(process2);
	 }
	 virtual void print(std::ostream& out) const {
	    process1->print(out); out << "; "; process2->print(out);
	 }
	 virtual Alphabet acceptable() const {
	    if (process1->accepts_success()) {
	       return process2->acceptable();
	    } else {
	       return process1->acceptable();
	    }
	 }
      protected:
	 virtual ProcessPtr internal_proceed(std::string& event) {
	    if (process1->accepts_success()) {
	       return process2->proceed(event);
	    } else {
	       return std::make_shared<ProcessSequence>(
		  process1->proceed(event), process2);
	    }
	 }
	 virtual Alphabet internal_get_alphabet() const {
	    return process1->get_alphabet() + process2->get_alphabet();
	 }
      private:
	 ProcessPtr process1;
	 ProcessPtr process2;
	 virtual void initialize_dependencies() const {
	    process1->add_dependant(std::dynamic_pointer_cast<const Process>(
	       shared_from_this()));
	    process2->add_dependant(
	       std::dynamic_pointer_cast<const Process>(shared_from_this()));
	 }
   };

   class StopProcess: public Process {
      public:
	 StopProcess(const Alphabet& alphabet) :
	       stop_alphabet(alphabet) {
	 }
	 StopProcess(ProcessPtr p_alphabet) :
	       p_alphabet(p_alphabet) {
	 }
	 virtual void print(std::ostream& out) const {
	    out << "STOP " << get_alphabet();
	 }
	 virtual Alphabet acceptable() const {
	    return Alphabet();
	 }
      protected:
	 virtual ProcessPtr internal_proceed(std::string& next_event) {
	    return nullptr;
	 }
	 virtual Alphabet internal_get_alphabet() const {
	    if (p_alphabet) {
	       return p_alphabet->get_alphabet();
	    } else {
	       return stop_alphabet;
	    }
	 }
      private:
	 const Alphabet stop_alphabet;
	 ProcessPtr p_alphabet; // process from which we take its alphabet

	 virtual void initialize_dependencies() const {
	    p_alphabet->add_dependant(
	       std::dynamic_pointer_cast<const Process>(shared_from_this()));
	 }
   };

   class RunProcess: public Process {
      public:
	 RunProcess(const Alphabet& alphabet) :
	       run_alphabet(alphabet) {
	 }
	 RunProcess(ProcessPtr p_alphabet) :
	       p_alphabet(p_alphabet) {
	 }
	 virtual void print(std::ostream& out) const {
	    out << "RUN " << get_alphabet();
	 }
	 virtual Alphabet acceptable() const {
	    return get_alphabet();
	 }
      protected:
	 virtual ProcessPtr internal_proceed(std::string& next_event) {
	    return std::dynamic_pointer_cast<Process>(shared_from_this());
	 }
	 virtual Alphabet internal_get_alphabet() const {
	    if (p_alphabet) {
	       return p_alphabet->get_alphabet();
	    } else {
	       return run_alphabet;
	    }
	 }
      private:
	 const Alphabet run_alphabet;
	 ProcessPtr p_alphabet; // process from which we take its alphabet

	 virtual void initialize_dependencies() const {
	    if (p_alphabet) {
	       p_alphabet->add_dependant(
		  std::dynamic_pointer_cast<const Process>
		     (shared_from_this()));
	    }
	 }
   };

   class SkipProcess: public Process {
      public:
	 SkipProcess(const Alphabet& alphabet) : skip_alphabet(alphabet) {
	 }
	 SkipProcess(ProcessPtr p_alphabet) : p_alphabet(p_alphabet) {
	 }
	 virtual void print(std::ostream& out) const {
	    out << "SKIP " << get_alphabet();
	 }
	 virtual Alphabet acceptable() const {
	    return Alphabet("_success_");
	 }
      protected:
	 virtual ProcessPtr internal_proceed(std::string& next_event) {
	    /* should usually not be used */
	    if (next_event == "_success_") {
	       return std::make_shared<StopProcess>(skip_alphabet);
	    } else {
	       return nullptr;
	    }
	 }
	 virtual Alphabet internal_get_alphabet() const {
	    if (p_alphabet) {
	       return p_alphabet->get_alphabet() + Alphabet("_success_");
	    } else {
	       return skip_alphabet + Alphabet("_success_");
	    }
	 }
      private:
	 const Alphabet skip_alphabet;
	 ProcessPtr p_alphabet; // process from which we take its alphabet

	 virtual void initialize_dependencies() const {
	    if (p_alphabet) {
	       p_alphabet->add_dependant(
		  std::dynamic_pointer_cast<const Process>(shared_from_this()));
	    }
	 }
   };
} // namespace CSP

#endif
