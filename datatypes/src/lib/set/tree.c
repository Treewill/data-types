
#include "set/tree.h"
#include "set/error.h"

#include <stdlib.h>

struct set_implementation;
struct set_tree;

// Note:
// LEFT + RIGHT = BALANCED
enum balance_t {
	LEFT = -1,
	BALANCED = 0,
	RIGHT = 1
};

struct set_tree {
	void * value;
	enum balance_t balance;
	struct set_tree * left;
	struct set_tree * right;
};

struct set_implementation {
	int (* comparator)(void * a, void * b);
	struct set_tree * tree;
};


static int set_insert(struct dt_set * this, void * item);
static void * set_has(const struct dt_set * this, void * item);
static void set_remove(struct dt_set * this, void * item);
static struct dt_list * set_items(const struct dt_set * this);
static void set_del(struct dt_set * this);

static struct set_tree * set_tree_find(
	struct set_tree * tree, void * item,
	int (* comparator)(void * a, void * b));

static int set_tree_insert(
	struct set_tree * * tree, struct set_tree * node,
	int (* comparator)(void * a, void * b));

static void set_tree_insert_balance (
	struct set_tree * * unbalanced, struct set_tree * node,
	int (*comparator)(void * a, void * b));

static int set_tree_remove_find(
	struct set_tree * * tree,
	void * item,
	int (* comparator)(void * a, void * b));

static int set_tree_remove(
	struct set_tree * * tree,
	struct set_tree * * to_swap,
	int direction);

static int set_tree_remove_balance(
	struct set_tree * * tree,
	int side);

static void set_tree_free(struct set_tree * tree);

static void set_tree_collect(
	struct set_tree * tree,
	struct dt_list_iterator * iterator);

static void rotate_left(struct set_tree * * tree);
static void rotate_right(struct set_tree * * tree);

struct dt_set * dt_set_tree_new(
	int (* comparator)(void * a, void * b),
	unsigned int (* hash)(void * item))
{
	struct dt_set * set;
	set = malloc(sizeof(*set));

	if (!set) return NULL;

	struct set_implementation * implementation;
	implementation = malloc(sizeof(*implementation));

	if (!implementation) {
		free(set);
		return NULL;
	}

	set->insert = &set_insert;
	set->has = &set_has;
	set->remove = &set_remove;
	set->items = &set_items;
	set->del = &set_del;
	set->_data = implementation;

	implementation->comparator = comparator;
	implementation->tree = NULL;
	return set;
}

static int set_insert(struct dt_set * this, void * item)
{
	struct set_implementation * data = this->_data;
	struct set_tree * node;
	node = malloc(sizeof(*node));

	if (!node) return DT_SET_ENOMEM;

	node->value = item;
	node->balance = 0;
	node->left = NULL;
	node->right = NULL;

	set_tree_insert(&(data->tree), node, data->comparator);
	return 0;
}

static void * set_has(const struct dt_set * this, void * item)
{
	struct set_implementation * data = this->_data;
	struct set_tree * node;
	node = set_tree_find(data->tree, item, data->comparator);
	if (!node) return NULL;
	return node->value;
}

static void set_remove(struct dt_set * this, void * item)
{
	struct set_implementation * data = this->_data;
	set_tree_remove_find(&(data->tree), item, data->comparator);
}

static struct dt_list * set_items(const struct dt_set * this)
{
	struct set_implementation * data = this->_data;
	struct dt_list * list;
	list = dt_list_new();
	if (!list) return NULL;

	struct dt_list_iterator * iterator;
	iterator = list->iterator(list);

	if (!iterator) {
		list->del(list);
		return NULL;
	}

	set_tree_collect(data->tree, iterator);
	iterator->del(iterator);
	return list;
}

static void set_del(struct dt_set * this)
{
	struct set_implementation * data = this->_data;
	set_tree_free(data->tree);
	free(data);
	free(this);
}

static struct set_tree * set_tree_find(
	struct set_tree * tree, void * item,
	int (* comparator)(void * a, void * b))
{
	if (!tree) return NULL;

	int compare = comparator(item, tree->value);
	if (compare == 0) return tree;
	if (compare < 0) return set_tree_find(tree->left, item, comparator);
	return set_tree_find(tree->right, item, comparator);
}

static int set_tree_insert(
	struct set_tree * * tree, struct set_tree * node,
	int (* comparator)(void * a, void * b))
{
	if (!*tree) {
		*tree = node;
		return 1;
	}
	int compare = comparator(node->value, (*tree)->value);
	int offset;
	if (compare == 0) return 0;
	if (compare < 0) {
		offset = set_tree_insert(
			&((*tree)->left), node, comparator);
		if (offset) {
			if ((*tree)->balance == LEFT) {
				set_tree_insert_balance(
					tree, node, comparator);
				return 0;
			} else {
				(*tree)->balance--;
			}
		}
	} else {
		offset = set_tree_insert(
			&((*tree)->right), node, comparator);
		if (offset) {
			if ((*tree)->balance == RIGHT) {
				set_tree_insert_balance(
					tree, node, comparator);
				return 0;
			} else {
				(*tree)->balance++;
			}
		}
	}
	return offset && (*tree)->balance != BALANCED ? 1 : 0;
}

static void set_tree_insert_balance (
	struct set_tree * * unbalanced, struct set_tree * node,
	int (*comparator)(void * a, void * b))
{
	if (!*unbalanced) return;
	if ((*unbalanced)->balance < 0) {
		struct set_tree * * side;
		side = &((*unbalanced)->left);
		int compare = comparator(node->value, (*side)->value);
		if (compare > 0) {
			rotate_left(side);
			rotate_right(unbalanced);
			(*unbalanced)->left->balance = BALANCED;
			(*unbalanced)->right->balance = BALANCED;
			if ((*unbalanced)->balance == RIGHT) {
				(*unbalanced)->left->balance = LEFT;
			} else if ((*unbalanced)->balance == LEFT) {
				(*unbalanced)->right->balance = RIGHT;
			}
			(*unbalanced)->balance = BALANCED;
		} else {
			rotate_right(unbalanced);
			(*unbalanced)->balance = BALANCED;
			(*unbalanced)->right->balance = BALANCED;
		}
	} else {
		struct set_tree * * side;
		side = &((*unbalanced)->right);
		int compare = comparator(node->value, (*side)->value);
		if (compare < 0) {
			rotate_right(side);
			rotate_left(unbalanced);
			(*unbalanced)->left->balance = BALANCED;
			(*unbalanced)->right->balance = BALANCED;
			if ((*unbalanced)->balance == RIGHT) {
				(*unbalanced)->left->balance = LEFT;
			} else if ((*unbalanced)->balance == LEFT) {
				(*unbalanced)->right->balance = RIGHT;
			}
			(*unbalanced)->balance = BALANCED;
		} else {
			rotate_left(unbalanced);
			(*unbalanced)->balance = BALANCED;
			(*unbalanced)->left->balance = BALANCED;
		}
	}
}

static int set_tree_remove_find(
	struct set_tree * * tree,
	void * item,
	int (* comparator)(void * a, void * b))
{
	int compare = comparator(item, (*tree)->value);
	if (compare == 0) {
		return set_tree_remove(tree, tree, BALANCED);
	} else if (compare < 0) {
		if (set_tree_remove_find(
				&((*tree)->left), item, comparator)) {
			return set_tree_remove_balance(tree, LEFT);
		}
	} else {
		if (set_tree_remove_find(
				&((*tree)->right), item, comparator)) {
			return set_tree_remove_balance(tree, RIGHT);
		}
	}
	return 0;
}

static int set_tree_remove(
	struct set_tree * * tree,
	struct set_tree * * to_swap,
	enum balance_t direction)
{
	int offset;
	int side;
	if (direction == BALANCED) {
		if ((*tree)->right) {
			offset = set_tree_remove(
				&((*tree)->right), to_swap, RIGHT);
			side = RIGHT;
		} else if ((*tree)->left) {
			offset = set_tree_remove(
				&((*tree)->left), to_swap, LEFT);
			side = LEFT;
		} else {
			free(*tree);
			*tree = NULL;
			return 1;
		}
	} else if (direction == RIGHT){
		if (!(*tree)->left) {
			void * value = (*to_swap)->value;
			(*to_swap)->value = (*tree)->value;
			(*tree)->value = value;
			return set_tree_remove(tree, tree, BALANCED);
		}
		offset = set_tree_remove(
			&((*tree)->left), to_swap, direction);
		side = LEFT;
	} else {
		if (!(*tree)->right) {
			void * value = (*to_swap)->value;
			(*to_swap)->value = (*tree)->value;
			(*tree)->value = value;
			return set_tree_remove(tree, tree, BALANCED);
		}
		offset = set_tree_remove(
			&((*tree)->right), to_swap, direction);
		side = RIGHT;
	}

	if (!offset) return 0;
	return set_tree_remove_balance(tree, side);
}

static int set_tree_remove_balance(
	struct set_tree * * tree,
	enum balance_t side)
{
	if ((*tree)->balance != -side) {
		(*tree)->balance -= side;
		return (*tree)->balance == BALANCED ? 1 : 0;
	}

	// Need to re-balance the tree.
	if (side == -1) {
		if ((*tree)->right->balance == LEFT) {
			rotate_right(&((*tree)->right));
			rotate_left(tree);
			(*tree)->left->balance = BALANCED;
			(*tree)->right->balance = BALANCED;
			if ((*tree)->balance == LEFT) {
				(*tree)->right->balance = RIGHT;
			} else if ((*tree)->balance == RIGHT) {
				(*tree)->left->balance = LEFT;
			}
			(*tree)->balance = BALANCED;
			return 1;
		}
		rotate_left(tree);
		if ((*tree)->balance == BALANCED) {
			(*tree)->balance = LEFT;
			(*tree)->left->balance = RIGHT;
			return 0;
		} else {
			(*tree)->balance = BALANCED;
			(*tree)->left->balance = BALANCED;
			return 1;
		}
	} else {
		if ((*tree)->left->balance == RIGHT) {
			rotate_left(&((*tree)->left));
			rotate_right(tree);
			(*tree)->right->balance = BALANCED;
			(*tree)->left->balance = BALANCED;
			if ((*tree)->balance == RIGHT) {
				(*tree)->left->balance = LEFT;
			} else if ((*tree)->balance == LEFT) {
				(*tree)->right->balance = RIGHT;
			}
			(*tree)->balance = BALANCED;
			return 1;
		}
		rotate_right(tree);
		if ((*tree)->balance == BALANCED) {
			(*tree)->balance = RIGHT;
			(*tree)->right->balance = LEFT;
			return 0;
		} else {
			(*tree)->balance = BALANCED;
			(*tree)->right->balance = BALANCED;
			return 1;
		}
	}
}

static void set_tree_collect(
	struct set_tree * tree,
	struct dt_list_iterator * iterator)
{
	if (!tree) return;
	set_tree_collect(tree->left, iterator);
	iterator->insert(iterator, tree->value);
	iterator->next(iterator);
	set_tree_collect(tree->right, iterator);
}

static void set_tree_free(struct set_tree * tree)
{
	if (!tree) return;
	set_tree_free(tree->left);
	set_tree_free(tree->right);
	free(tree);
}

static void rotate_left(struct set_tree * * tree)
{
	struct set_tree * root = *tree;
	struct set_tree * right = root->right;
	struct set_tree * subtree = right->left;

	*tree = right;
	right->left = root;
	root->right = subtree;
}

static void rotate_right(struct set_tree * * tree)
{
	struct set_tree * root = *tree;
	struct set_tree * left = root->left;
	struct set_tree * subtree = left->right;

	*tree = left;
	left->right = root;
	root->left = subtree;
}
