
OBJS            = representation.o parser.o lexer.o main.o 

BIN             = bin/parser

FLAGOUS           = -std=c++11 -Wall -Wno-write-strings

DEBUG		= -g -DDEBUG

all: $(OBJS)
	g++ $(FLAGOUS) $(DEBUG) $(OBJS) -lfl -lm -o $(BIN)

representation.o: ../representation.cpp
	g++ $(FLAGOUS) -c ../representation.cpp

%.o: %.cc
	g++ $(FLAGOUS) $(DEBUG) -c $<

%.o: %.cpp
	g++ $(FLAGOUS) $(DEBUG) -c $<

clean:
	-rm -f $(OBJS) $(BIN) parser.cc parser.h lexer.cc *~ \#*

realclean:
	$(MAKE) clean
	-rm -f $(BIN)

parser.cc: parser.ypp
	bison  --defines=parser.h --report=lookahead --report-file=bison_report -o parser.cc parser.ypp

lexer.cc: lexer.l
	flex -o lexer.cc lexer.l
