EXEC := \
	aibe \
	param-generator \


LD_LIBS := -lpbc -lgmp
BIN := bin

OBJ := $(addsuffix .o,$(EXEC))

all: bin aibe param-generator

bin:
	mkdir -p $(BIN)

aibe.o: aibe.c bin
	$(CC) -c -o $(BIN)/$@ $<

aibe: aibe.o bin
	$(CC) -o $(BIN)/$@ $(BIN)/$< -L. $(LD_LIBS)

param-generator.o: param-generator.cpp bin
	$(CXX) -c -o $(BIN)/$@ $<

param-generator: param-generator.o bin
	$(CXX) -o $(BIN)/$@ $(BIN)/$< -L. $(LD_LIBS)

.PHONY: clean

clean:
	-rm -rf bin/