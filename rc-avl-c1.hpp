#include <functional>

template<class T, typename Less=std::less<T> >
class RcAvlC1 {
	struct Node {
		T data;
		int height;
		Node *kid[2];
	} dummy, *nnil;
	unsigned count;
	void set_height(Node *n) {
		n->height = 1 + (n->kid[0]->height > n->kid[1]->height? n->kid[0]->height : n->kid[1]->height);
	}
	int balance(const Node *n) { return n->kid[0]->height - n->kid[1]->height; }
	// rotate a subtree according to dir; if new root is nil, old root is freed
	Node *rotate(Node **rootp, int dir) {
		Node *old_r = *rootp, *new_r = old_r->kid[dir];
		if (nnil == (*rootp = new_r))
			delete old_r;
		else {
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
		if (root != nnil) set_height(root);
	}
	// find the Node that contains value as data; or returns 0
	Node *find_recur(Node *p, const T &value) {
		return p == nnil? 0 : p->data == value? p : find_recur(p->kid[value > p->data], value);
	}
	void insert_recur(Node **rootp, const T &value) {
		Node *root = *rootp;
		if (root == nnil) {
			Node *p = new Node;
			p->height = 0, p->kid[0] = p->kid[1] = nnil, p->data = value;
			*rootp = p, ++count;
		} else if (value != root->data) { // don't allow dup keys
			insert_recur(&root->kid[value > root->data], value);
			adjust_balance(rootp);
		}
	}
	void erase_recur(Node **rootp, const T &value) {
		Node *root = *rootp;
		if (root == nnil) return; // not found
		// if this is the Node we want, rotate until off the tree
		if (root->data == value)
			if (nnil == (root = rotate(rootp, ballance(root) < 0)))
				return;
		erase_recur(&root->kid[value > root->data], value);
		adjust_balance(rootp);
	}
	void destroy(Node *p) {
		if (p->kid[0] != nnil) destroy(p->kid[0]);
		if (p->kid[1] != nnil) destroy(p->kid[1]);
		delete p;
	}
public:
	Node *root;
	RcAvlC1() : count(0) {
		dummy.height = 0, dummy.kid[0] = dummy.kid[1] = &dummy;
		root = nnil = &dummy;
	}
	~RcAvlC1() { destroy(root); root = nnil; }
	unsigned size(void) const { return count; }
	T *find(const T &data) { Node *p = find_recur(root, data); return p? &p->data : 0; }
	void insert(const T &data) { insert_recur(&root, data); }
	void erase(const T &data) { erase_recur(&root, data); }
};
