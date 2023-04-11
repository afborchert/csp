BisonSources :=	$(wildcard *.ypp)
GeneratedCPPSourcesFromBison := $(patsubst %.ypp,%.tab.cpp,$(BisonSources))
GeneratedCPPSources := $(GeneratedCPPSourcesFromBison)
GeneratedHPPSources := $(patsubst %.ypp,%.tab.hpp,$(BisonSources)) \
   $(wildcard *.hh)
CPPSources := $(GeneratedCPPSources) \
   error.cpp scanner.cpp \
   testlex.cpp testparser.cpp trace.cpp
Objects := $(patsubst %.cpp,%.o,$(CPPSources))
MainCPPSources := testlex.cpp testparser.cpp trace.cpp
MainObjects := $(patsubst %.cpp,%.o,$(MainCPPSources))
#BISON := bison
core_objs := error.o parser.tab.o scanner.o
testparser_objs := $(core_objs) testparser.o
testlex_objs := $(core_objs) testlex.o
trace_objs := $(core_objs) trace.o
Binaries := testlex testparser trace
MAKEDEPEND := perl ../gcc-makedepend/gcc-makedepend.pl

CXX :=		g++
CXXSTD :=	-std=c++17
CXXFLAGS :=	-Wall -g
LDFLAGS :=
CPPFLAGS +=	-I. -I../fmt $(CXXSTD)
LDLIBS :=
BISON :=	bison

.PHONY:		all clean depend
all:		$(GeneratedCPPSourcesFromBison) $(Objects) $(Binaries)
clean:		; rm -f $(Objects) $(GeneratedCPPSources) parser.output \
		   $(MainObjects)
realclean:	clean
		rm -f $(GeneratedCPPSources) $(GeneratedHPPSources) \
		   $(Binaries)

testlex:	$(testlex_objs)
		$(CXX) $(LDFLAGS) -o $@ $(testlex_objs) $(LDLIBS)

testparser:	$(testparser_objs)
		$(CXX) $(LDFLAGS) -o $@ $(testparser_objs) $(LDLIBS)

trace:		$(trace_objs)
		$(CXX) $(LDFLAGS) -o $@ $(trace_objs) $(LDLIBS)

$(GeneratedCPPSourcesFromBison): %.tab.cpp: %.ypp
	$(BISON) -d $<

depend:		$(CPPSources)
		$(MAKEDEPEND) $(CPPFLAGS) $(CPPSources)
# DO NOT DELETE
parser.tab.o: parser.tab.cpp context.hpp channel.hpp alphabet.hpp \
 object.hpp chaos-process.hpp process.hpp status.hpp scope.hpp \
 uniformint.hpp concealed-process.hpp error.hpp ../fmt/printf.hpp \
 location.hh external-choice.hpp event-set.hpp identifier.hpp \
 interleaving-processes.hpp internal-choice.hpp mapped-process.hpp \
 symbol-changer.hpp parallel-processes.hpp parameters.hpp \
 prefixed-process.hpp process-definition.hpp named-process.hpp \
 symtable.hpp process-reference.hpp parser.hpp parser.tab.hpp scanner.hpp \
 process-sequence.hpp reading-process.hpp recursive-process.hpp \
 run-process.hpp selecting-process.hpp writing-process.hpp \
 skip-process.hpp stop-process.hpp subordination.hpp
error.o: error.cpp context.hpp error.hpp ../fmt/printf.hpp location.hh \
 parser.hpp process.hpp alphabet.hpp channel.hpp object.hpp status.hpp \
 scope.hpp uniformint.hpp symtable.hpp symbol-changer.hpp identifier.hpp \
 parser.tab.hpp scanner.hpp
scanner.o: scanner.cpp error.hpp ../fmt/printf.hpp context.hpp \
 location.hh identifier.hpp object.hpp process.hpp alphabet.hpp \
 channel.hpp status.hpp scope.hpp uniformint.hpp process-reference.hpp \
 parameters.hpp parser.hpp symtable.hpp symbol-changer.hpp parser.tab.hpp \
 process-definition.hpp named-process.hpp scanner.hpp
testlex.o: testlex.cpp context.hpp parser.hpp location.hh process.hpp \
 alphabet.hpp channel.hpp object.hpp status.hpp scope.hpp uniformint.hpp \
 symtable.hpp error.hpp ../fmt/printf.hpp symbol-changer.hpp \
 identifier.hpp parser.tab.hpp scanner.hpp
testparser.o: testparser.cpp context.hpp parser.hpp location.hh \
 process.hpp alphabet.hpp channel.hpp object.hpp status.hpp scope.hpp \
 uniformint.hpp symtable.hpp error.hpp ../fmt/printf.hpp \
 symbol-changer.hpp identifier.hpp parser.tab.hpp scanner.hpp
trace.o: trace.cpp context.hpp parser.hpp location.hh process.hpp \
 alphabet.hpp channel.hpp object.hpp status.hpp scope.hpp uniformint.hpp \
 symtable.hpp error.hpp ../fmt/printf.hpp symbol-changer.hpp \
 identifier.hpp parser.tab.hpp scanner.hpp
