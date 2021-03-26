CXXFLAGS=-g -O3 -Wall

test-insert:test.cc rb.hh kavl-lite.h kavl.hpp kavl-no-cnt.hpp rc-avl-c1.hpp rc-avl-cpp1.hpp
	$(CXX) $(CXXFLAGS) $< -o $@

clean:
	rm -fr test-insert *.o *.dSYM
