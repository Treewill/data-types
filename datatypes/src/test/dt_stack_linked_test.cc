#include "gtest/gtest.h"

#include "stack.h"
#include "stack/error.h"
#include "stack/linked.h"

static char items[] = "";
static struct dt_stack * new_stack() {
	return dt_stack_linked_new();
}

TEST (StackTest, BasicStackUsage) {
	struct dt_stack * stack = new_stack();
	EXPECT_TRUE(stack) << "New failed!";

	EXPECT_EQ(0, stack->push(stack, items + 0));

	EXPECT_EQ(1, stack->length(stack));

	EXPECT_EQ(items + 0, stack->pop(stack));

	stack->del(stack);
}

TEST (StackTest, SmallStack) {
	struct dt_stack * stack = new_stack();
	
	EXPECT_EQ(0, stack->push(stack, items + 0));
	EXPECT_EQ(0, stack->push(stack, items + 1));
	EXPECT_EQ(0, stack->push(stack, items + 2));
	EXPECT_EQ(0, stack->push(stack, items + 3));

	EXPECT_EQ(4, stack->length(stack));

	EXPECT_EQ(items + 3, stack->pop(stack));
	EXPECT_EQ(items + 2, stack->pop(stack));
	EXPECT_EQ(items + 1, stack->pop(stack));
	EXPECT_EQ(items + 0, stack->pop(stack));
	EXPECT_EQ(NULL, stack->pop(stack));

	EXPECT_EQ(0, stack->length(stack));

	stack->del(stack);
}

TEST (StackTest, SmallPeek) {
	struct dt_stack * stack = new_stack();
	
	EXPECT_EQ(0, stack->push(stack, items + 0));
	EXPECT_EQ(items + 0, stack->peek(stack));
	EXPECT_EQ(0, stack->push(stack, items + 1));
	EXPECT_EQ(items + 1, stack->peek(stack));
	EXPECT_EQ(0, stack->push(stack, items + 2));
	EXPECT_EQ(items + 2, stack->peek(stack));
	EXPECT_EQ(0, stack->push(stack, items + 3));
	EXPECT_EQ(items + 3, stack->peek(stack));

	EXPECT_EQ(4, stack->length(stack));

	EXPECT_EQ(items + 3, stack->pop(stack));
	EXPECT_EQ(items + 2, stack->peek(stack));
	EXPECT_EQ(items + 2, stack->pop(stack));
	EXPECT_EQ(items + 1, stack->peek(stack));
	EXPECT_EQ(items + 1, stack->pop(stack));
	EXPECT_EQ(items + 0, stack->peek(stack));
	EXPECT_EQ(items + 0, stack->pop(stack));
	EXPECT_EQ(NULL, stack->peek(stack));

	EXPECT_EQ(0, stack->length(stack));

	stack->del(stack);

}

TEST (StackTest, LargeStack) {
	struct dt_stack * stack = new_stack();
	
	EXPECT_EQ(0, stack->push(stack, items + 0));
	EXPECT_EQ(items + 0, stack->peek(stack));
	EXPECT_EQ(0, stack->push(stack, items + 1));
	EXPECT_EQ(items + 1, stack->peek(stack));
	EXPECT_EQ(0, stack->push(stack, items + 2));
	EXPECT_EQ(items + 2, stack->peek(stack));
	EXPECT_EQ(0, stack->push(stack, items + 3));
	EXPECT_EQ(items + 3, stack->peek(stack));
	EXPECT_EQ(0, stack->push(stack, items + 4));
	EXPECT_EQ(items + 4, stack->peek(stack));
	EXPECT_EQ(0, stack->push(stack, items + 5));
	EXPECT_EQ(items + 5, stack->peek(stack));
	EXPECT_EQ(0, stack->push(stack, items + 6));
	EXPECT_EQ(items + 6, stack->peek(stack));
	EXPECT_EQ(0, stack->push(stack, items + 7));
	EXPECT_EQ(items + 7, stack->peek(stack));
	EXPECT_EQ(0, stack->push(stack, items + 8));
	EXPECT_EQ(items + 8, stack->peek(stack));
	EXPECT_EQ(0, stack->push(stack, items + 9));
	EXPECT_EQ(items + 9, stack->peek(stack));
	EXPECT_EQ(0, stack->push(stack, items + 10));
	EXPECT_EQ(items + 10, stack->peek(stack));
	EXPECT_EQ(0, stack->push(stack, items + 11));
	EXPECT_EQ(items + 11, stack->peek(stack));
	EXPECT_EQ(0, stack->push(stack, items + 12));
	EXPECT_EQ(items + 12, stack->peek(stack));
	EXPECT_EQ(0, stack->push(stack, items + 13));
	EXPECT_EQ(items + 13, stack->peek(stack));
	EXPECT_EQ(0, stack->push(stack, items + 14));
	EXPECT_EQ(items + 14, stack->peek(stack));
	EXPECT_EQ(0, stack->push(stack, items + 15));
	EXPECT_EQ(items + 15, stack->peek(stack));


	EXPECT_EQ(16, stack->length(stack));

	EXPECT_EQ(items + 15, stack->pop(stack));
	EXPECT_EQ(items + 14, stack->peek(stack));
	EXPECT_EQ(items + 14, stack->pop(stack));
	EXPECT_EQ(items + 13, stack->peek(stack));
	EXPECT_EQ(items + 13, stack->pop(stack));
	EXPECT_EQ(items + 12, stack->peek(stack));
	EXPECT_EQ(items + 12, stack->pop(stack));
	EXPECT_EQ(items + 11, stack->peek(stack));
	EXPECT_EQ(items + 11, stack->pop(stack));
	EXPECT_EQ(items + 10, stack->peek(stack));
	EXPECT_EQ(items + 10, stack->pop(stack));
	EXPECT_EQ(items + 9, stack->peek(stack));
	EXPECT_EQ(items + 9, stack->pop(stack));
	EXPECT_EQ(items + 8, stack->peek(stack));
	EXPECT_EQ(items + 8, stack->pop(stack));
	EXPECT_EQ(items + 7, stack->peek(stack));
	EXPECT_EQ(items + 7, stack->pop(stack));
	EXPECT_EQ(items + 6, stack->peek(stack));
	EXPECT_EQ(items + 6, stack->pop(stack));
	EXPECT_EQ(items + 5, stack->peek(stack));
	EXPECT_EQ(items + 5, stack->pop(stack));
	EXPECT_EQ(items + 4, stack->peek(stack));
	EXPECT_EQ(items + 4, stack->pop(stack));
	EXPECT_EQ(items + 3, stack->peek(stack));
	EXPECT_EQ(items + 3, stack->pop(stack));
	EXPECT_EQ(items + 2, stack->peek(stack));
	EXPECT_EQ(items + 2, stack->pop(stack));
	EXPECT_EQ(items + 1, stack->peek(stack));
	EXPECT_EQ(items + 1, stack->pop(stack));
	EXPECT_EQ(items + 0, stack->peek(stack));
	EXPECT_EQ(items + 0, stack->pop(stack));
	EXPECT_EQ(NULL, stack->peek(stack));

	EXPECT_EQ(0, stack->length(stack));

	stack->del(stack);
}
