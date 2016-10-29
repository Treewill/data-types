#include "gtest/gtest.h"



#include "list.h"
#include "list/error.h"
#include "list/linked.h"

static char items[] = "";
static struct dt_list * new_list() {
	return dt_list_linked_new();
}

TEST (ListTest, BasicListUsage) {
	struct dt_list * list = new_list();
	EXPECT_TRUE(list) << "New failed!";

	EXPECT_EQ(0, list->insert(list, 0, items + 0));

	EXPECT_EQ(items + 0, list->get(list, 0));

	EXPECT_EQ(1, list->length(list));

	EXPECT_EQ(0, list->remove(list, 0));

	EXPECT_EQ(0, list->length(list));

	list->del(list);
}

TEST (ListTest, SmallList) {
	
	struct dt_list * list = new_list();

	EXPECT_EQ(0, list->insert(list, 0, items + 0));
	EXPECT_EQ(0, list->insert(list, 1, items + 1));
	EXPECT_EQ(0, list->insert(list, 2, items + 2));
	EXPECT_EQ(0, list->insert(list, 3, items + 3));

	EXPECT_EQ(4, list->length(list));

	EXPECT_EQ(items + 0, list->get(list, 0));
	EXPECT_EQ(items + 1, list->get(list, 1));
	EXPECT_EQ(items + 2, list->get(list, 2));
	EXPECT_EQ(items + 3, list->get(list, 3));

	EXPECT_EQ(0, list->remove(list, 1));
	EXPECT_EQ(3, list->length(list));

	EXPECT_EQ(items + 0, list->get(list, 0));
	EXPECT_EQ(items + 2, list->get(list, 1));
	EXPECT_EQ(items + 3, list->get(list, 2));

	EXPECT_EQ(0, list->remove(list, 2));
	EXPECT_EQ(0, list->remove(list, 0));

	EXPECT_EQ(1, list->length(list));

	EXPECT_EQ(items + 2, list->get(list, 0));

	EXPECT_EQ(0, list->insert(list, 1, items + 3));
	EXPECT_EQ(0, list->insert(list, 0, items + 0));

	EXPECT_EQ(3, list->length(list));

	EXPECT_EQ(items + 0, list->get(list, 0));
	EXPECT_EQ(items + 2, list->get(list, 1));
	EXPECT_EQ(items + 3, list->get(list, 2));
	
	EXPECT_EQ(0, list->insert(list, 1, items + 1));

	EXPECT_EQ(4, list->length(list));

	EXPECT_EQ(items + 0, list->get(list, 0));
	EXPECT_EQ(items + 1, list->get(list, 1));
	EXPECT_EQ(items + 2, list->get(list, 2));
	EXPECT_EQ(items + 3, list->get(list, 3));
	
	list->del(list);
}

TEST (ListTest, RandomInsertGet) {
	struct dt_list * list = new_list();

	EXPECT_EQ(0, list->insert(list, 0, items + 3));
	EXPECT_EQ(0, list->insert(list, 0, items + 1));
	EXPECT_EQ(0, list->insert(list, 0, items + 0));
	EXPECT_EQ(0, list->insert(list, 2, items + 2));

	EXPECT_EQ(items + 2, list->get(list, 2));
	EXPECT_EQ(items + 0, list->get(list, 0));
	EXPECT_EQ(items + 1, list->get(list, 1));
	EXPECT_EQ(items + 3, list->get(list, 3));
	
	list->del(list);
}

TEST (IterateForwardTest, ScanTest) {
	struct dt_list * list = new_list();

	EXPECT_EQ(0, list->insert(list, 0, items + 0));
	EXPECT_EQ(0, list->insert(list, 1, items + 1));
	EXPECT_EQ(0, list->insert(list, 2, items + 2));
	EXPECT_EQ(0, list->insert(list, 3, items + 3));

	struct dt_list_iterator * iterator =
		list->iterator(list);

	EXPECT_TRUE(iterator->valid(iterator));
	EXPECT_EQ(items + 0, iterator->get(iterator));

	EXPECT_EQ(0, iterator->next(iterator));
	EXPECT_TRUE(iterator->valid(iterator));
	EXPECT_EQ(items + 1, iterator->get(iterator));

	EXPECT_EQ(0, iterator->next(iterator));
	EXPECT_TRUE(iterator->valid(iterator));
	EXPECT_EQ(items + 2, iterator->get(iterator));

	EXPECT_EQ(0, iterator->next(iterator));
	EXPECT_TRUE(iterator->valid(iterator));
	EXPECT_EQ(items + 3, iterator->get(iterator));

	EXPECT_EQ(DT_LIST_EINDEX, iterator->next(iterator));
	EXPECT_FALSE(iterator->valid(iterator));

	iterator->del(iterator);
	list->del(list);
}



TEST (IterateForwardTest, InsertTest) {
	struct dt_list * list = new_list();

	EXPECT_EQ(0, list->insert(list, 0, items + 1));
	EXPECT_EQ(0, list->insert(list, 1, items + 3));


	struct dt_list_iterator * iterator =
		list->iterator(list);
	
	EXPECT_EQ(0, iterator->insert(iterator, items + 0));
	EXPECT_TRUE(iterator->valid(iterator));
	EXPECT_EQ(items + 0, iterator->get(iterator));

	EXPECT_EQ(0, iterator->next(iterator));
	EXPECT_TRUE(iterator->valid(iterator));
	EXPECT_EQ(items + 1, iterator->get(iterator));

	EXPECT_EQ(0, iterator->next(iterator));
	EXPECT_TRUE(iterator->valid(iterator));
	EXPECT_EQ(items + 3, iterator->get(iterator));
	
	EXPECT_EQ(0, iterator->insert(iterator, items + 2));
	EXPECT_TRUE(iterator->valid(iterator));
	EXPECT_EQ(items + 2, iterator->get(iterator));

	EXPECT_EQ(0, iterator->next(iterator));
	EXPECT_TRUE(iterator->valid(iterator));
	EXPECT_EQ(items + 3, iterator->get(iterator));

	EXPECT_EQ(DT_LIST_EINDEX, iterator->next(iterator));
	EXPECT_FALSE(iterator->valid(iterator));

	EXPECT_EQ(0, iterator->insert(iterator, items + 4));
	EXPECT_TRUE(iterator->valid(iterator));
	EXPECT_EQ(items + 4, iterator->get(iterator));

	EXPECT_EQ(DT_LIST_EINDEX, iterator->next(iterator));
	EXPECT_FALSE(iterator->valid(iterator));
	
	iterator->del(iterator);

	EXPECT_EQ(items + 0, list->get(list, 0));
	EXPECT_EQ(items + 1, list->get(list, 1));
	EXPECT_EQ(items + 2, list->get(list, 2));
	EXPECT_EQ(items + 3, list->get(list, 3));
	EXPECT_EQ(items + 4, list->get(list, 4));

	EXPECT_EQ(5, list->length(list));

	list->del(list);
	
}

TEST (IterateForwardTest, DeleteTest) {
	struct dt_list * list = new_list();
	
	EXPECT_EQ(0, list->insert(list, 0, items + 0));
	EXPECT_EQ(0, list->insert(list, 1, items + 1));
	EXPECT_EQ(0, list->insert(list, 2, items + 2));
	EXPECT_EQ(0, list->insert(list, 3, items + 3));
	EXPECT_EQ(0, list->insert(list, 4, items + 4));

	struct dt_list_iterator * iterator =
		list->iterator(list);

	EXPECT_EQ(0, iterator->remove(iterator));
	EXPECT_TRUE(iterator->valid(iterator));
	EXPECT_EQ(items + 1, iterator->get(iterator));

	EXPECT_EQ(0, iterator->next(iterator));
	EXPECT_TRUE(iterator->valid(iterator));
	EXPECT_EQ(items + 2, iterator->get(iterator));
	EXPECT_EQ(0, iterator->remove(iterator));
	EXPECT_TRUE(iterator->valid(iterator));
	EXPECT_EQ(items + 3, iterator->get(iterator));

	EXPECT_EQ(0, iterator->next(iterator));
	EXPECT_TRUE(iterator->valid(iterator));
	EXPECT_EQ(items + 4, iterator->get(iterator));
	EXPECT_EQ(0, iterator->remove(iterator));
	EXPECT_FALSE(iterator->valid(iterator));
	
	iterator->del(iterator);

	EXPECT_EQ(items + 1, list->get(list, 0));
	EXPECT_EQ(items + 3, list->get(list, 1));

	EXPECT_EQ(2, list->length(list));

	list->del(list);
}

TEST (IterateBackwardTest, ScanTest) {
	struct dt_list * list = new_list();

	EXPECT_EQ(0, list->insert(list, 0, items + 0));
	EXPECT_EQ(0, list->insert(list, 1, items + 1));
	EXPECT_EQ(0, list->insert(list, 2, items + 2));
	EXPECT_EQ(0, list->insert(list, 3, items + 3));

	struct dt_list_iterator * iterator =
		list->iterator(list);

	EXPECT_EQ(0, iterator->next(iterator));
	EXPECT_EQ(0, iterator->next(iterator));
	EXPECT_EQ(0, iterator->next(iterator));
	EXPECT_EQ(DT_LIST_EINDEX, iterator->next(iterator));
	EXPECT_FALSE(iterator->valid(iterator));

	EXPECT_EQ(0, iterator->previous(iterator));
	EXPECT_TRUE(iterator->valid(iterator));
	EXPECT_EQ(items + 3, iterator->get(iterator));

	EXPECT_EQ(0, iterator->previous(iterator));
	EXPECT_TRUE(iterator->valid(iterator));
	EXPECT_EQ(items + 2, iterator->get(iterator));

	EXPECT_EQ(0, iterator->previous(iterator));
	EXPECT_TRUE(iterator->valid(iterator));
	EXPECT_EQ(items + 1, iterator->get(iterator));

	EXPECT_EQ(0, iterator->previous(iterator));
	EXPECT_TRUE(iterator->valid(iterator));
	EXPECT_EQ(items + 0, iterator->get(iterator));

	EXPECT_EQ(DT_LIST_EINDEX, iterator->previous(iterator));

	iterator->del(iterator);
	list->del(list);
}

TEST (IterateBackwardTest, InsertTest) {
	struct dt_list * list = new_list();

	EXPECT_EQ(0, list->insert(list, 0, items + 1));
	EXPECT_EQ(0, list->insert(list, 1, items + 3));


	struct dt_list_iterator * iterator =
		list->iterator(list);
	
	EXPECT_EQ(0, iterator->next(iterator));
	EXPECT_EQ(DT_LIST_EINDEX, iterator->next(iterator));
	
	EXPECT_EQ(0, iterator->insert(iterator, items + 4));
	EXPECT_TRUE(iterator->valid(iterator));
	EXPECT_EQ(items + 4, iterator->get(iterator));

	EXPECT_EQ(0, iterator->previous(iterator));
	EXPECT_TRUE(iterator->valid(iterator));
	EXPECT_EQ(items + 3, iterator->get(iterator));
	EXPECT_EQ(0, iterator->insert(iterator, items + 2));
	EXPECT_TRUE(iterator->valid(iterator));
	EXPECT_EQ(items + 2, iterator->get(iterator));

	EXPECT_EQ(0, iterator->previous(iterator));
	EXPECT_TRUE(iterator->valid(iterator));
	EXPECT_EQ(items + 1, iterator->get(iterator));
	EXPECT_EQ(0, iterator->insert(iterator, items + 0));
	EXPECT_TRUE(iterator->valid(iterator));
	EXPECT_EQ(items + 0, iterator->get(iterator));

	iterator->del(iterator);

	EXPECT_EQ(items + 0, list->get(list, 0));
	EXPECT_EQ(items + 1, list->get(list, 1));
	EXPECT_EQ(items + 2, list->get(list, 2));
	EXPECT_EQ(items + 3, list->get(list, 3));
	EXPECT_EQ(items + 4, list->get(list, 4));

	EXPECT_EQ(5, list->length(list));

	list->del(list);
	
}

TEST (IterateBackwardTest, DeleteTest) {
	struct dt_list * list = new_list();
	
	EXPECT_EQ(0, list->insert(list, 0, items + 0));
	EXPECT_EQ(0, list->insert(list, 1, items + 1));
	EXPECT_EQ(0, list->insert(list, 2, items + 2));
	EXPECT_EQ(0, list->insert(list, 3, items + 3));
	EXPECT_EQ(0, list->insert(list, 4, items + 4));

	struct dt_list_iterator * iterator =
		list->iterator(list);

	EXPECT_EQ(0, iterator->next(iterator));
	EXPECT_EQ(0, iterator->next(iterator));
	EXPECT_EQ(0, iterator->next(iterator));
	EXPECT_EQ(0, iterator->next(iterator));
	EXPECT_EQ(DT_LIST_EINDEX, iterator->next(iterator));
	
	EXPECT_EQ(0, iterator->previous(iterator));
	EXPECT_EQ(0, iterator->remove(iterator));
	EXPECT_FALSE(iterator->valid(iterator));
	EXPECT_EQ(0, iterator->previous(iterator));
	EXPECT_TRUE(iterator->valid(iterator));
	EXPECT_EQ(items + 3, iterator->get(iterator));

	EXPECT_EQ(0, iterator->previous(iterator));
	EXPECT_TRUE(iterator->valid(iterator));
	EXPECT_EQ(items + 2, iterator->get(iterator));
	EXPECT_EQ(0, iterator->remove(iterator));
	EXPECT_TRUE(iterator->valid(iterator));
	EXPECT_EQ(0, iterator->previous(iterator));
	EXPECT_TRUE(iterator->valid(iterator));
	EXPECT_EQ(items + 1, iterator->get(iterator));

	EXPECT_EQ(0, iterator->previous(iterator));
	EXPECT_TRUE(iterator->valid(iterator));
	EXPECT_EQ(items + 0, iterator->get(iterator));
	EXPECT_EQ(0, iterator->remove(iterator));
	EXPECT_TRUE(iterator->valid(iterator));
	EXPECT_EQ(DT_LIST_EINDEX, iterator->previous(iterator));

	iterator->del(iterator);

	EXPECT_EQ(items + 1, list->get(list, 0));
	EXPECT_EQ(items + 3, list->get(list, 1));

	EXPECT_EQ(2, list->length(list));

	list->del(list);
}
