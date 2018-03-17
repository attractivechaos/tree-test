CXXFLAGS=-g -O3 -Wall

test-insert:test.cc avl.hh rb.hh
	$(CXX) $(CXXFLAGS) $< -o $@

clean:
	rm -fr test-insert *.o *.dSYM
