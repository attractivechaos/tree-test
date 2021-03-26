#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <set>
#include "kavl.hpp"
#include "rb.hh"
#include "kavl-lite.h"
#include "kavl-no-cnt.hpp"
#include "rc-avl-c1.hpp"
#include "rc-avl-cpp1.hpp"

struct my_node {
	uint32_t x;
	KAVLL_HEAD(struct my_node) head;
};
#define my_cmp(a, b) (((a)->x > (b)->x) - ((a)->x < (b)->x))
KAVLL_INIT(my, struct my_node, head, my_cmp)

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
		q = my_insert(&root, p);
		if (p == q) p = (struct my_node*)malloc(sizeof(*p));
		x = hash32(x);
	}
	free(p);
	kavll_size(struct my_node, head, root, &x);
	kavll_free(struct my_node, head, root, free);
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

uint32_t test_insert_kavlpp_no_cnt(int n, uint32_t x)
{
	klib::AvlNoCnt<uint32_t> tree;
	for (int i = 0; i < n; ++i) {
		tree.insert(x);
		x = hash32(x);
	}
	return tree.size();
}

uint32_t test_insert_RcAvlC1(int n, uint32_t x)
{
	RcAvlC1<uint32_t> tree;
	for (int i = 0; i < n; ++i) {
		tree.insert(x);
		x = hash32(x);
	}
	return tree.size();
}

uint32_t test_insert_RcAvlCpp1(int n, uint32_t x)
{
	AVLtree<uint32_t> tree;
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

#include <getopt.h>

int main(int argc, char *argv[])
{
	int c, algo = -1;
	uint32_t x, n = 1000000;
	double t, t0;

	while ((c = getopt(argc, argv, "n:a:h")) >= 0) {
		if (c == 'n') n = atoi(optarg);
		else if (c == 'a') algo = atoi(optarg);
		else if (c == 'h') {
			printf("Usage: test-insert [options]\n");
			printf("Options:\n");
			printf("  -n INT    number of elements to insert [%d]\n", n);
			printf("  -a INT    algorithm [all]\n");
			printf("            1: std::set, 2: RB from libavl, 3: kavl-lite-C\n");
			printf("            4: kavl-C++-no-counting, 5: kavl-C++-\n");
			printf("            6: RosettaCode-avl-c-v1, 7: RosettaCode-avl-cpp-v1\n");
			return 0;
		}
	}
	if (algo < 0) fprintf(stderr, "Use 'test-insert -h' to see command-line options.\n");

	t = cputime();
	x = test_hash(n, 11);
	t0 = cputime() - t;
	fprintf(stderr, "%.3f sec - hash [%x]\n", cputime() - t, x);

	if (algo < 0 || algo == 1) {
		t = cputime();
		x = test_insert_stl(n, 11);
		fprintf(stderr, "[algo 1] %.3f sec - insert to std::set (usually RB-tree) [%d]\n", cputime() - t - t0, x);
	}

	if (algo < 0 || algo == 2) {
		t = cputime();
		x = test_insert_rb(n, 11);
		fprintf(stderr, "[algo 2] %.3f sec - insert to RB-tree [%d]\n", cputime() - t - t0, x);
	}

	if (algo < 0 || algo == 3) {
		t = cputime();
		x = test_insert_kavl(n, 11);
		fprintf(stderr, "[algo 3] %.3f sec - insert to kavl-lite (AVL-tree; w/o counting) [%d]\n", cputime() - t - t0, x);
	}

	if (algo < 0 || algo == 4) {
		t = cputime();
		x = test_insert_kavlpp_no_cnt(n, 11);
		fprintf(stderr, "[algo 4] %.3f sec - insert to kavlpp (AVL-tree; w/o counting) [%d]\n", cputime() - t - t0, x);
	}

	if (algo < 0 || algo == 5) {
		t = cputime();
		x = test_insert_kavlpp(n, 11);
		fprintf(stderr, "[algo 5] %.3f sec - insert to kavlpp (AVL-tree; w/ counting) [%d]\n", cputime() - t - t0, x);
	}

	if (algo < 0 || algo == 6) {
		t = cputime();
		x = test_insert_RcAvlC1(n, 11);
		fprintf(stderr, "[algo 6] %.3f sec - insert to RcAvlC1 (AVL-tree) [%d]\n", cputime() - t - t0, x);
	}

	if (algo == 7) { // not enabled by default, as this implementation is slow
		t = cputime();
		x = test_insert_RcAvlCpp1(n, 11);
		fprintf(stderr, "[algo 7] %.3f sec - insert to RcAvlCpp1 (AVL-tree) [%d]\n", cputime() - t - t0, x);
	}

	return 0;
}
