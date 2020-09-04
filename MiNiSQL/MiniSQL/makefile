LEX=lex
YACC=yacc
CC=g++
OBJECT=./runner

$(OBJECT): lex.yy.c  y.tab.c  y.tab.h
	$(CC)   lex.yy.c    y.tab.c API.cpp CatalogManager.cpp CatalogPage.cpp BufferManager.cpp BPTreeEntry.cpp BPTreeKey.cpp RecordManager.cpp RecordPage.cpp IndexCatalogPage.cpp main.cpp -o runner -std=c++11 -Wno-unused-value -Wno-deprecated-register -pthread

lex.yy.c: interpreter.lpp
	$(LEX) interpreter.lpp

y.tab.c  y.tab.h: interpreter.ypp
	$(YACC) -d interpreter.ypp

clean:
	rm -f $(OBJECT)  *.o
