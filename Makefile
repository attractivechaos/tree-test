CXXFLAGS=-g -O3 -Wall

test-insert:test.cc avl.hh rb.hh kavl.h
	$(CXX) $(CXXFLAGS) $< -o $@

clean:
	rm -fr test-insert *.o *.dSYM
