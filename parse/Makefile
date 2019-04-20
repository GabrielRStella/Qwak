#C++ compiler options
CXXFLAGS += -std=c++11
LDFLAGS += -lcln -lginac

#Make it so we can always call "make <cmd>" and it will run
.PHONY: clean test cleansim sim

#Command to compile C++ executables, eg "prog: a.o b.o":
#$(CXX) $^ $(LDFLAGS) -o $@

#############################################################
#Run "make" to make the library and compile tests
#Run "make test" to do the above, then run the tests
#############################################################

all: qwak.a tests qwaket

test: tests
	./tests

tests: tests.o qwak.a
	(cd ../sim && make)
	$(CXX) $^ ../sim/quantum_simulator.a $(LDFLAGS) -o $@
	#(cd ../sim && make clean)

qwaket: qwaket.o qwak.a
	(cd ../sim && make)
	$(CXX) $^ ../sim/quantum_simulator.a $(LDFLAGS) -o $@
	#(cd ../sim && make clean)

#Generates a static library
#https://stackoverflow.com/a/32168987
#Consider using a .so (runtime library)?
qwak.a: env.o parse.o qwak.o
	ar rcs $@ $^

clean:
	rm -f *.o qwak.a tests qwaket
  
cleansim:
	(cd ../sim && make clean)
  
sim:
	(cd ../sim && make)