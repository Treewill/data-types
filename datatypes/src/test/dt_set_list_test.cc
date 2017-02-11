#include "gtest/gtest.h"

#include "set.h"
#include "set/error.h"
#include "set/list.h"

#include <ctype.h>
#include <string.h>

static char items[] =
	"\x00\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0a\x0b\x0c\x0d\x0e\x0f"
	"\x10\x11\x12\x13\x14\x15\x16\x17\x18\x19\x1a\x1b\x1c\x1d\x1e\x1f"
	"\x20\x21\x22\x23\x24\x25\x26\x27\x28\x29\x2a\x2b\x2c\x2d\x2e\x2f"
	"\x30\x31\x32\x33\x34\x35\x36\x37\x38\x39\x3a\x3b\x3c\x3d\x3e\x3f"
	"\x40\x41\x42\x43\x44\x45\x46\x47\x48\x49\x4a\x4b\x4c\x4d\x4e\x4f"
	"\x50\x51\x52\x53\x54\x55\x56\x57\x58\x59\x5a\x5b\x5c\x5d\x5e\x5f"
	"\x60\x61\x62\x63\x64\x65\x66\x67\x68\x69\x6a\x6b\x6c\x6d\x6e\x6f"
	"\x70\x71\x72\x73\x74\x75\x76\x77\x78\x79\x7a\x7b\x7c\x7d\x7e\x7f"
	"\x80\x81\x82\x83\x84\x85\x86\x87\x88\x89\x8a\x8b\x8c\x8d\x8e\x8f"
	"\x90\x91\x92\x93\x94\x95\x96\x97\x98\x99\x9a\x9b\x9c\x9d\x9e\x9f"
	"\xa0\xa1\xa2\xa3\xa4\xa5\xa6\xa7\xa8\xa9\xaa\xab\xac\xad\xae\xaf"
	"\xb0\xb1\xb2\xb3\xb4\xb5\xb6\xb7\xb8\xb9\xba\xbb\xbc\xbd\xbe\xbf"
	"\xc0\xc1\xc2\xc3\xc4\xc5\xc6\xc7\xc8\xc9\xca\xcb\xcc\xcd\xce\xcf"
	"\xd0\xd1\xd2\xd3\xd4\xd5\xd6\xd7\xd8\xd9\xda\xdb\xdc\xdd\xde\xdf"
	"\xe0\xe1\xe2\xe3\xe4\xe5\xe6\xe7\xe8\xe9\xea\xeb\xec\xed\xee\xef"
	"\xf0\xf1\xf2\xf3\xf4\xf5\xf6\xf7\xf8\xf9\xfa\xfb\xfc\xfd\xfe\xff";

int compare(void * a, void * b)
{
	char x = *(char *)a;
	char y = *(char *)b;
	return
		x == y ? 0 :
		x < y ? -1 : 1;
}

unsigned int hash(void * c)
{
	// We need an imperfect hash to simulate
	// real data.
	return tolower(*(char *)c);
}

struct dt_set * new_set()
{
	return dt_set_list_new(&compare, &hash);
}

TEST (SetTest, BasicSetUsage) {
	struct dt_set * set = new_set();
	EXPECT_TRUE(set) << "New failed!";

	EXPECT_FALSE(set->has(set, items + 'a'));
	EXPECT_EQ(0, set->insert(set, items + 'a'));
	EXPECT_TRUE(set->has(set, items + 'a'));
	set->remove(set, items + 'a');
	EXPECT_FALSE(set->has(set, items + 'a'));

	set->del(set);
}

TEST (SetTest, UniqueHashes) {
	struct dt_set * set = new_set();

	#define _alphabet "abcdefghijklmnopqrstuvwxyz"
	char alphabet[sizeof(_alphabet)];
	strcpy(alphabet, _alphabet);
	#undef _alphabet

	#define _shuffled "mdgotewibshpafrzynkxljcvqu"
	char shuffled[sizeof(_shuffled)];
	strcpy(shuffled, _shuffled);
	#undef _shuffled

	char * iter;
	for (iter = alphabet; *iter; iter++) {
		EXPECT_EQ(0, set->insert(set, iter));
	}

	for(iter = shuffled; *iter; iter++) {
		EXPECT_TRUE(set->has(set, iter));
	}

	for (iter = shuffled; *iter; iter++) {
		set->remove(set, iter);
		EXPECT_FALSE(set->has(set, iter));
	}

	set->del(set);
}

TEST (SetTest, CollidingHashes) {
	struct dt_set * set = new_set();

	#define _alphabet "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
	char alphabet[sizeof(_alphabet)];
	strcpy(alphabet, _alphabet);
	#undef _alphabet

	#define _shuffled "IelKpBqdSFiAaZQNrGxOEnmfvHXkJsDhgjRbtyUCMwWYPLVoTcuz"
	char shuffled[sizeof(_shuffled)];
	strcpy(shuffled, _shuffled);
	#undef _shuffled

	char * iter;
	for (iter = alphabet; *iter; iter++) {
		EXPECT_EQ(0, set->insert(set, iter));
	}

	for(iter = shuffled; *iter; iter++) {
		EXPECT_TRUE(set->has(set, iter));
	}

	for (iter = shuffled; *iter; iter++) {
		set->remove(set, iter);
		EXPECT_FALSE(set->has(set, iter));
	}

	set->del(set);
}

bool list_has(struct dt_list * list, void * item)
{
	struct dt_list_iterator * iterator;
	iterator = list->iterator(list);
	bool result = false;

	for (; iterator->valid(iterator) && !result;
		iterator->next(iterator)) {

		if (!(compare(item, iterator->get(iterator)))) result = true;
	}

	iterator->del(iterator);
	return result;
}

TEST (SetListTest, BasicUsage) {
	struct dt_set * set = new_set();

	#define _alphabet "Catfish"
	char alphabet[sizeof(_alphabet)];
	strcpy(alphabet, _alphabet);
	#undef _alphabet

	char * iter;
	for (iter = alphabet; *iter; iter++) {
		EXPECT_EQ(0, set->insert(set, iter));
	}

	struct dt_list * list;
	list = set->items(set);

	for (iter = alphabet; *iter; iter++) {
		EXPECT_TRUE(list_has(list, iter));
	}

	list->del(list);
	set->del(set);
}

void string_difference(
	char const * a,
	char const * b,
	char * difference)
{
	for (; *a; a++) {
		for (const char * c = b; *c; c++) {
			if (*a == *c) goto CONTINUE;
		}
		*difference = *a;
		difference++;
		CONTINUE:;
	}
	*difference = '\0';
}

TEST (SetListTest, ShrunkSet) {
	struct dt_set * set = new_set();

	#define _alphabet "Catfish"
	char alphabet[sizeof(_alphabet)];
	strcpy(alphabet, _alphabet);

	#define _dropped "if"
	char dropped[sizeof(_dropped)];
	strcpy(dropped, _dropped);
	#undef _dropped

	char remaining[sizeof(_alphabet)];
	#undef _alphabet

	string_difference(alphabet, dropped, remaining);

	char * iter;
	for (iter = alphabet; *iter; iter++) {
		EXPECT_EQ(0, set->insert(set, iter));
	}

	for (iter = dropped; *iter; iter++) {
		set->remove(set, iter);
	}

	struct dt_list * list;
	list = set->items(set);

	for (iter = dropped; *iter; iter++) {
		EXPECT_FALSE(list_has(list, iter));
	}

	for (iter = remaining; *iter; iter++) {
		EXPECT_TRUE(list_has(list, iter));
	}

	list->del(list);
	set->del(set);
}

