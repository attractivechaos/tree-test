#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <set>
#include "kavl.hpp"
#include "rb.hh"
#include "kavl.h"

struct my_node {
	uint32_t x;
	KAVL_HEAD(struct my_node) head;
};
#define my_cmp(a, b) (((a)->x > (b)->x) - ((a)->x < (b)->x))
KAVL_INIT(my, struct my_node, head, my_cmp)

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

uint32_t test_hash(int n, uint32_t x)
{
	for (int i = 0; i < n; ++i)
		x = hash32(x);
	return x;
}

uint32_t test_insert_kavl(int n, uint32_t x)
{
	struct my_node *root = 0, *p, *q;
	p = (struct my_node*)malloc(sizeof(*p));
	for (int i = 0; i < n; ++i) {
		p->x = x;
		q = kavl_insert(my, &root, p, 0);
		if (p == q) p = (struct my_node*)malloc(sizeof(*p));
		x = hash32(x);
	}
	free(p);
	x = kavl_size(head, root);
	kavl_free(struct my_node, head, root, free);
	return x;
}

uint32_t test_insert_stl(int n, uint32_t x)
{
	std::set<uint32_t> tree;
	for (int i = 0; i < n; ++i) {
		tree.insert(x);
		x = hash32(x);
	}
	return tree.size();
}

uint32_t test_insert_kavlpp(int n, uint32_t x)
{
	klib::Avl<uint32_t> tree;
	for (int i = 0; i < n; ++i) {
		tree.insert(x);
		x = hash32(x);
	}
	return tree.size();
}

uint32_t test_insert_rb(int n, uint32_t x)
{
	rbset_t<uint32_t, cmp32> tree;
	for (int i = 0; i < n; ++i) {
		int is_present;
		tree.insert(x, &is_present);
		x = hash32(x);
	}
	return tree.count;
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
	x = test_insert_stl(n, 11);
	fprintf(stderr, "%.3f sec - insert to std::set (usually RB-tree) [%d]\n", cputime() - t, x);

	t = cputime();
	x = test_insert_rb(n, 11);
	fprintf(stderr, "%.3f sec - insert to RB-tree [%d]\n", cputime() - t, x);

	t = cputime();
	x = test_insert_kavlpp(n, 11);
	fprintf(stderr, "%.3f sec - insert to kavlpp (AVL-tree) [%d]\n", cputime() - t, x);

	t = cputime();
	x = test_insert_kavl(n, 11);
	fprintf(stderr, "%.3f sec - insert to kavl (AVL-tree) [%d]\n", cputime() - t, x);

	return 0;
}
