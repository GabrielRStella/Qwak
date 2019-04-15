#C++ compiler options
CXXFLAGS += -std=c++11
LDFLAGS += -lcln -lginac

#Make it so we can always call "make <cmd>" and it will run
.PHONY: clean test

#Command to compile C++ executables, eg "prog: a.o b.o":
#$(CXX) $^ $(LDFLAGS) -o $@

#############################################################
#Run "make" to make the library and compile tests
#Run "make test" to do the above, then run the tests
#############################################################

all: quantum_simulator.a tests

test: tests
	./tests

tests: tests.o quantum_simulator.a
	$(CXX) $^ $(LDFLAGS) -o $@

gate_tests: quantum_gate_test.o quantum_simulator.a
	$(CXX) $^ $(LDFLAGS) -o $@

phase_test: phase_test.o quantum_simulator.a
	$(CXX) $^ $(LDFLAGS) -o $@


#Generates a static library
#https://stackoverflow.com/a/32168987
#Consider using a .so (runtime library)?
quantum_simulator.a: quantum_state.o quantum_gate.o util.o
	ar rcs $@ $^

clean:
	rm -f *.o quantum_simulator.a tests gate_tests phase_test
