EXEC := \
	pbc-test \
	aibe \
	param-generator \


LD_LIBS := -lpbc -lgmp
BIN := bin

OBJ := $(addsuffix .o,$(EXEC))

all: bin $(OBJ) $(EXEC)

bin:
	mkdir -p $(BIN)

$(OBJ): %.o: %.cpp bin
	$(CXX) -c -o $(BIN)/$@ $<

$(EXEC): %: %.o bin
	$(CXX) -o $(BIN)/$@ $(BIN)/$< -L. $(LD_LIBS)

.PHONY: clean

clean:
	-rm -rf bin/