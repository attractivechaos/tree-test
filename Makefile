CXXFLAGS=-g -O3 -Wall

test-insert:test.cc rb.hh kavl.h kavl.hpp rc-avl-c1.hpp
	$(CXX) $(CXXFLAGS) $< -o $@

clean:
	rm -fr test-insert *.o *.dSYM
