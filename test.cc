#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <set>
#include "avl.hh"
#include "rb.hh"

static double cputime(void)
{
	struct rusage r;
	getrusage(RUSAGE_SELF, &r);
	return r.ru_utime.tv_sec + r.ru_stime.tv_sec + 1e-6 * (r.ru_utime.tv_usec + r.ru_stime.tv_usec);
}

static inline uint32_t hash32(uint32_t key)
{
    key += ~(key << 15);
    key ^=  (key >> 10);
    key +=  (key << 3);
    key ^=  (key >> 6);
    key += ~(key << 11);
    key ^=  (key >> 16);
    return key;
}

struct cmp32 {
	inline int operator() (uint32_t a, uint32_t b) {
		return (a > b) - (a < b);
	}
};

typedef avlset_t<uint32_t, cmp32> avlset_int_t;
typedef rbset_t<uint32_t, cmp32> rbset_int_t;
typedef std::set<uint32_t> stlset_int_t;

uint32_t test_hash(int n, uint32_t x)
{
	for (int i = 0; i < n; ++i)
		x = hash32(x);
	return x;
}

void test_insert_stl(int n, uint32_t x)
{
	std::set<uint32_t> tree;
	for (int i = 0; i < n; ++i) {
		tree.insert(x);
		x = hash32(x);
	}
}

void test_insert_avl(int n, uint32_t x)
{
	avlset_int_t tree;
	for (int i = 0; i < n; ++i) {
		tree.insert(x);
		x = hash32(x);
	}
}

void test_insert_rb(int n, uint32_t x)
{
	rbset_int_t tree;
	for (int i = 0; i < n; ++i) {
		int is_present;
		tree.insert(x, &is_present);
		x = hash32(x);
	}
}

int main(int argc, char *argv[])
{
	uint32_t x, n = 1000000;
	double t;
	if (argc > 1) n = atoi(argv[1]);
	t = cputime();
	x = test_hash(n, 11);
	fprintf(stderr, "%.3f sec - hash [%x]\n", cputime() - t, x);
	t = cputime();
	test_insert_stl(n, 11);
	fprintf(stderr, "%.3f sec - insert to std::set (usually RB-tree)\n", cputime() - t);
	t = cputime();
	test_insert_rb(n, 11);
	fprintf(stderr, "%.3f sec - insert to RB-tree\n", cputime() - t);
	t = cputime();
	test_insert_avl(n, 11);
	fprintf(stderr, "%.3f sec - insert to AVL-tree\n", cputime() - t);
	return 0;
}
