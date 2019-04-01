#pragma once

template<class T>
class RcAvlC1 {
	unsigned count; // number of elements in the tree
	struct Node {
		T data;
		int height;
		Node *kid[2];
	};
	inline int get_height(const Node *n) { return n? n->height : 0; }
	inline void set_height(Node *n) {
		int h0 = get_height(n->kid[0]), h1 = get_height(n->kid[1]);
		n->height = 1 + (h0 > h1? h0 : h1);
	}
	inline int balance(const Node *n) { return get_height(n->kid[0]) - get_height(n->kid[1]); }
	inline Node *rotate(Node **rootp, int dir) { // rotate a subtree according to dir; if new root is nil, old root is freed
		Node *old_r = *rootp, *new_r = old_r->kid[dir];
		if (0 == (*rootp = new_r)) {
			delete old_r;
			--count;
		} else {
			old_r->kid[dir] = new_r->kid[!dir];
			set_height(old_r);
			new_r->kid[!dir] = old_r;
		}
		return new_r;
	}
	void adjust_balance(Node **rootp) {
		Node *root = *rootp;
		int b = balance(root)/2;
		if (b) {
			int dir = (1 - b)/2;
			if (balance(root->kid[dir]) == -b)
				rotate(&root->kid[dir], !dir);
			root = rotate(rootp, dir);
		}
		if (root != 0) set_height(root);
	}
	Node *find_recur(Node *p, const T &value) {
		return p == 0? 0 : p->data == value? p : find_recur(p->kid[value > p->data], value);
	}
	void insert_recur(Node **rootp, const T &value) {
		Node *root = *rootp;
		if (root == 0) {
			Node *p = new Node;
			p->height = 0, p->kid[0] = p->kid[1] = 0, p->data = value;
			*rootp = p, ++count;
		} else if (value != root->data) { // don't allow dup keys
			insert_recur(&root->kid[value > root->data], value);
			adjust_balance(rootp);
		}
	}
	void erase_recur(Node **rootp, const T &value) {
		Node *root = *rootp;
		if (root == 0) return; // not found
		if (root->data == value) // if this is the Node we want, rotate until off the tree
			if (0 == (root = rotate(rootp, ballance(root) < 0)))
				return;
		erase_recur(&root->kid[value > root->data], value);
		adjust_balance(rootp);
	}
	void destroy(Node *p) {
		if (p->kid[0]) destroy(p->kid[0]);
		if (p->kid[1]) destroy(p->kid[1]);
		delete p;
	}
public:
	Node *root;
	RcAvlC1() : root(0), count(0) {};
	~RcAvlC1() { destroy(root); }
	unsigned size(void) const { return count; }
	T *find(const T &data) { Node *p = find_recur(root, data); return p? &p->data : 0; }
	void insert(const T &data) { insert_recur(&root, data); }
	void erase(const T &data) { erase_recur(&root, data); }
};
