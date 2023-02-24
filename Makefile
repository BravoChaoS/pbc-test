EXEC := \
	pbc-test \
	param-generator \

BLS_DIR := /home/jojjiw/workspace/libBLS
BLS_LIB := $(BLS_DIR)/build
DEP_DIR := $(BLS_DIR)/deps/deps_inst/x86_or_x64/include
DEP_LIB := $(BLS_DIR)/deps/deps_inst/x86_or_x64/lib
LD_LIBS := -lpbc -lgmp
BLS_LIBS := $(LD_LIBS) -llibbls -llibff
BIN := bin

OBJ := $(addsuffix .o,$(EXEC))

all: bin aibe pbc-test param-generator merklecpp-test bls-test Keccak256Test

bin:
	mkdir -p $(BIN)

pbc-test.o: pbc-test.cpp bin
	$(CC) -c -o $(BIN)/$@ $<

pbc-test: pbc-test.o bin
	$(CXX) -o $(BIN)/$@ $(BIN)/$< -L. $(LD_LIBS)


aibe.o: aibe.cpp bin
	$(CC) -c -o $(BIN)/$@ $<

aibe: aibe.o bin
	$(CXX) -o $(BIN)/$@ $(BIN)/$< -L. $(LD_LIBS)

param-generator.o: param-generator.cpp bin
	$(CXX) -c -o $(BIN)/$@ $<

param-generator: param-generator.o bin
	$(CXX) -o $(BIN)/$@ $(BIN)/$< -L. $(LD_LIBS)

merklecpp-test.o: merklecpp-test.cpp bin
	$(CXX) -c -o $(BIN)/$@ $<

merklecpp-test: merklecpp-test.o
	$(CXX) -o $(BIN)/$@ $(BIN)/$< -L. -lssl -lcrypto

bls-test.o: bls-test.cpp bin
	$(CXX) -c -o $(BIN)/$@ $< -I$(BLS_DIR) -I$(DEP_DIR)

bls-test: bls-test.o bin
	$(CXX) -o $(BIN)/$@ $(BIN)/$< -L$(BLS_LIB) -L$(DEP_LIB)

Keccak256Test.o: Keccak256Test.cpp bin
	$(CXX) -c -o $(BIN)/$@ $<

Keccak256Test: Keccak256Test.o
	$(CXX) -o $(BIN)/$@ $(BIN)/$<



.PHONY: clean

clean:
	-rm -rf bin/