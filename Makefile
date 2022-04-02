EXEC := \
	aibe \
	param-generator \


LD_LIBS := -lpbc -lgmp
BIN := bin

OBJ := $(addsuffix .o,$(EXEC))

all: bin aibe param-generator merklecpp-test

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

merklecpp-test.o: merklecpp-test.cpp bin
	$(CXX) -c -o $(BIN)/$@ $<

merklecpp-test: merklecpp-test.o
	$(CXX) -o $(BIN)/$@ $(BIN)/$< -L. -lssl -lcrypto

.PHONY: clean

clean:
	-rm -rf bin/