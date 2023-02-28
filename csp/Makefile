BisonSources :=	$(wildcard *.ypp)
GeneratedCPPSourcesFromBison := $(patsubst %.ypp,%.tab.cpp,$(BisonSources))
GeneratedCPPSources := $(GeneratedCPPSourcesFromBison) \
   yytname.cpp
GeneratedHPPSources := $(patsubst %.ypp,%.tab.hpp,$(BisonSources)) \
   $(wildcard *.hh)
CPPSources := $(GeneratedCPPSources) \
   error.cpp scanner.cpp scope.cpp symtable.cpp \
   testlex.cpp testparser.cpp trace.cpp
Objects := $(patsubst %.cpp,%.o,$(CPPSources))
MainCPPSources := testlex.cpp testparser.cpp trace.cpp
MainObjects := $(patsubst %.cpp,%.o,$(MainCPPSources))
#BISON := bison
core_objs := error.o parser.tab.o scanner.o yytname.o \
   scope.o symtable.o
testparser_objs := $(core_objs) testparser.o
testlex_objs := $(core_objs) testlex.o
trace_objs := $(core_objs) trace.o
Utils := ../bison-scripts
Binaries := testlex testparser trace
MAKEDEPEND := perl ../gcc-makedepend/gcc-makedepend.pl

CXX :=		g++
CXXSTD :=	-std=c++17
CXXFLAGS :=	-Wall -g
LDFLAGS :=
CPPFLAGS +=	-I. $(CXXSTD)
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

yytname.cpp:	parser.tab.cpp
		perl $(Utils)/extract_yytname.pl CSP parser.tab.cpp >$@

$(GeneratedCPPSourcesFromBison): %.tab.cpp: %.ypp
	$(BISON) -d $<

depend:		$(CPPSources)
		$(MAKEDEPEND) $(CPPFLAGS) $(CPPSources)
# DO NOT DELETE
parser.tab.o: parser.tab.cpp chaos-process.hpp alphabet.hpp process.hpp \
 object.hpp uniformint.hpp concealed-process.hpp error.hpp parser.hpp \
 location.hh symtable.hpp process-definition.hpp scope.hpp \
 symbol-changer.hpp identifier.hpp parser.tab.hpp external-choice.hpp \
 event-set.hpp interleaving-processes.hpp internal-choice.hpp \
 mapped-process.hpp parallel-processes.hpp prefixed-process.hpp \
 process-reference.hpp process-sequence.hpp run-process.hpp scanner.hpp \
 selecting-process.hpp skip-process.hpp stop-process.hpp yytname.hpp
yytname.o: yytname.cpp
error.o: error.cpp error.hpp parser.hpp location.hh symtable.hpp \
 process.hpp alphabet.hpp object.hpp uniformint.hpp \
 process-definition.hpp scope.hpp symbol-changer.hpp identifier.hpp \
 parser.tab.hpp
scanner.o: scanner.cpp error.hpp parser.hpp location.hh symtable.hpp \
 process.hpp alphabet.hpp object.hpp uniformint.hpp \
 process-definition.hpp scope.hpp symbol-changer.hpp identifier.hpp \
 parser.tab.hpp process-reference.hpp scanner.hpp
scope.o: scope.cpp object.hpp process.hpp alphabet.hpp uniformint.hpp \
 process-definition.hpp scope.hpp symbol-changer.hpp identifier.hpp \
 symtable.hpp
symtable.o: symtable.cpp object.hpp process.hpp alphabet.hpp \
 uniformint.hpp process-definition.hpp process-reference.hpp symtable.hpp \
 scope.hpp symbol-changer.hpp identifier.hpp
testlex.o: testlex.cpp parser.hpp location.hh symtable.hpp process.hpp \
 alphabet.hpp object.hpp uniformint.hpp process-definition.hpp scope.hpp \
 symbol-changer.hpp identifier.hpp parser.tab.hpp scanner.hpp yytname.hpp
testparser.o: testparser.cpp parser.hpp location.hh symtable.hpp \
 process.hpp alphabet.hpp object.hpp uniformint.hpp \
 process-definition.hpp scope.hpp symbol-changer.hpp identifier.hpp \
 parser.tab.hpp scanner.hpp yytname.hpp
trace.o: trace.cpp parser.hpp location.hh symtable.hpp process.hpp \
 alphabet.hpp object.hpp uniformint.hpp process-definition.hpp scope.hpp \
 symbol-changer.hpp identifier.hpp parser.tab.hpp scanner.hpp yytname.hpp
