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
CXXSTD :=	-std=c++14
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
		$(MAKEDEPEND) $(CPPFLAGS) $(CPPSources) $(MainCPPSources)
# DO NOT DELETE
parser.tab.o: parser.tab.cpp yystype.hpp object.hpp alphabet.hpp \
 symtable.hpp types.hpp uniformint.hpp scanner.hpp parser.hpp location.hh \
 position.hh parser.tab.hpp stack.hh yytname.hpp
yytname.o: yytname.cpp
error.o: error.cpp error.hpp parser.hpp location.hh position.hh \
 yystype.hpp object.hpp alphabet.hpp symtable.hpp types.hpp \
 uniformint.hpp parser.tab.hpp stack.hh
scanner.o: scanner.cpp error.hpp parser.hpp location.hh position.hh \
 yystype.hpp object.hpp alphabet.hpp symtable.hpp types.hpp \
 uniformint.hpp parser.tab.hpp stack.hh scanner.hpp
scope.o: scope.cpp object.hpp alphabet.hpp symtable.hpp types.hpp \
 uniformint.hpp scope.hpp
symtable.o: symtable.cpp object.hpp alphabet.hpp symtable.hpp types.hpp \
 uniformint.hpp scope.hpp
testlex.o: testlex.cpp parser.hpp location.hh position.hh yystype.hpp \
 object.hpp alphabet.hpp symtable.hpp types.hpp uniformint.hpp \
 parser.tab.hpp stack.hh scanner.hpp yytname.hpp
testparser.o: testparser.cpp parser.hpp location.hh position.hh \
 yystype.hpp object.hpp alphabet.hpp symtable.hpp types.hpp \
 uniformint.hpp parser.tab.hpp stack.hh scanner.hpp yytname.hpp
trace.o: trace.cpp parser.hpp location.hh position.hh yystype.hpp \
 object.hpp alphabet.hpp symtable.hpp types.hpp uniformint.hpp \
 parser.tab.hpp stack.hh scanner.hpp yytname.hpp
testlex.o: testlex.cpp parser.hpp location.hh position.hh yystype.hpp \
 object.hpp alphabet.hpp symtable.hpp types.hpp uniformint.hpp \
 parser.tab.hpp stack.hh scanner.hpp yytname.hpp
testparser.o: testparser.cpp parser.hpp location.hh position.hh \
 yystype.hpp object.hpp alphabet.hpp symtable.hpp types.hpp \
 uniformint.hpp parser.tab.hpp stack.hh scanner.hpp yytname.hpp
trace.o: trace.cpp parser.hpp location.hh position.hh yystype.hpp \
 object.hpp alphabet.hpp symtable.hpp types.hpp uniformint.hpp \
 parser.tab.hpp stack.hh scanner.hpp yytname.hpp
