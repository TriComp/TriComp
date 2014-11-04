
OBJS            = parser.o lexer.o main.o representation.o

BIN             = bin/parser

FLAGS           = -std=c++11

DEBUG		= -g -DDEBUG

all: $(OBJS)
	g++ $(FLAGS) $(DEBUG) $(OBJS) -lfl -lm -o $(BIN)

%.o: %.cpp
	g++ $(FLAGS) $(DEBUG) -c $<

clean:
	-rm -f $(OBJS) parser.cc parser.h lexer.cc representation.cpp *~ \#*

realclean:
	$(MAKE) clean
	-rm -f $(BIN)

parser.cc: parser.ypp
	bison --defines=parser.h --report=lookahead --report-file=bison_report -o parser.cc parser.ypp

lexer.cc: lexer.l
	flex -o lexer.cc lexer.l
