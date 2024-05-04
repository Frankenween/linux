// SPDX-License-Identifier: GPL-2.0
#include <linux/printk.h>
#include <linux/string.h>

#include "test_identities.h"
#include "identities.h"

static int try_insert(const char *name, int id)
{
	int ret = identity_create(name, id);

	if (ret == 0)
		return 0;
	pr_err("Error in creating identity %d: expected 0, got %d", id, ret);
	return 1;
}

static int check_existance(int id, const char *name)
{
	struct identity *cnt = identity_find(id);

	if (name == NULL && cnt == NULL)
		return 0;
	if (name == NULL) {
		pr_err("Entry %d isn't expected to exist", id);
		return 1;
	}
	if (cnt == NULL) {
		pr_err("Entry %d is expected to exist", id);
		return 1;
	}
	if (strcmp(name, cnt->name)) {
		pr_err("Entry %d has different name. Expected \"%s\", got \"%s\"",
		       id, name, cnt->name);
		return 1;
	}
	return 0;
}

static int check_hired(int id, bool hired)
{
	struct identity *cnt = identity_find(id);

	if (cnt == NULL) {
		pr_err("id %d expected to exixst", id);
		return 1;
	}
	if (cnt->hired != hired) {
		pr_err("hired status in %d wrong: expected %d got %d", id, hired, cnt->hired);
		return 1;
	}
	return 0;
}

int test_identities(void)
{
	pr_debug("Staring find test");
	int result = 0;

	result |= try_insert("id1", 1);
	result |= try_insert("vova", 2);
	result |= try_insert("riabchun", 3);
	result |= try_insert("0", 0);
	result |= try_insert("-1", -1);
	result |= try_insert("abcdefghijklmonpqrs", 239);
	if (result)
		goto DONE_TEST;
	if (identity_create("abcdefghijklmonpqrst", 100500) == 0) {
		pr_err("String is too long to be added");
		result = 1;
		goto DONE_TEST;
	}

	result |= check_existance(-1, "-1");
	result |= check_existance(3, "riabchun");
	result |= check_existance(0xAB0BA, NULL);
	result |= check_existance(1, "id1");
	result |= check_existance(2, "vova");
	result |= check_existance(0, "0");
	result |= check_existance(239, "abcdefghijklmonpqrs");
	if (result)
		goto DONE_TEST;

	result |= check_hired(0, false);
	result |= check_hired(1, false);
	result |= check_hired(-1, false);
	result |= check_hired(3, false);
	if (result)
		goto DONE_TEST;

	result |= identity_hire(3);
	result |= identity_hire(1);
	if (result) {
		pr_err("Failed to hire");
		goto DONE_TEST;
	}

	result |= try_insert("vladimir", 2);
	if (result) {
		pr_err("Failed to replace identity");
		goto DONE_TEST;
	}
	result |= check_existance(2, "vladimir");
	if (result) {
		pr_err("Error in replacement");
		goto DONE_TEST;
	}
	result |= check_hired(2, false);
	if (result) {
		pr_err("hired state is wrong after identity replacement");
		goto DONE_TEST;
	}

	identity_destroy(1);
	identity_destroy(0);
	result |= check_existance(1, NULL);
	result |= check_existance(0, NULL);
	if (result)
		goto DONE_TEST;

	result |= try_insert("2", 0);
	if (result)
		pr_err("Failed to insert after removal");

	result |= check_existance(0, "2");
	if (result)
		goto DONE_TEST;
	result |= check_hired(0, false);

DONE_TEST:
	clean_all_identities();
	return result;
}
