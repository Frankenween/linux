// SPDX-License-Identifier: GPL-2.0
#include <linux/slab.h>

#include "identities.h"

struct list_identity {
	struct list_head node;
	struct identity val;
};

static LIST_HEAD(ids);

static struct list_identity *__list_identity_find(int id)
{
	struct list_head *v;

	list_for_each(v, &ids) {
		struct list_identity *cnt_id = list_entry(v, struct list_identity, node);

		if (cnt_id->val.id == id)
			return cnt_id;
	}
	return NULL;
}

int identity_create(const char __kernel *name, int id)
{
	size_t name_len = strlen(name);

	if (name_len >= sizeof((struct identity){0} .name)) {
		pr_err("Invalid length of name: %zd", name_len);
		return -EINVAL;
	}

	struct list_identity *exists = __list_identity_find(id);

	if (exists) {
		memcpy(exists->val.name, name, sizeof(exists->val.name));
		return 0;
	}

	struct list_identity *new_identity = kzalloc(sizeof(struct list_identity), GFP_KERNEL);

	if (new_identity == NULL)
		return -ENOMEM;

	INIT_LIST_HEAD(&new_identity->node);
	strscpy(new_identity->val.name, name, sizeof((struct identity){0} .name));
	new_identity->val.id = id;
	new_identity->val.hired = false;
	list_add(&new_identity->node, &ids);
	return 0;
}

struct identity *identity_find(int id)
{
	struct list_identity *cnt = __list_identity_find(id);

	return cnt ? &cnt->val : NULL;
}

void identity_destroy(int id)
{
	struct list_identity *cnt = __list_identity_find(id);

	if (cnt == NULL)
		return;
	list_del(&cnt->node);
	kfree(cnt);
}

int identity_hire(int id)
{
	struct list_identity *cnt = __list_identity_find(id);

	if (cnt == NULL)
		return 1;
	cnt->val.hired = true;
	return 0;
}

void clean_all_identities(void)
{
	struct list_head *n;
	struct list_head *v;

	list_for_each_safe(v, n, &ids) {
		struct list_identity *cnt_id = list_entry(v, struct list_identity, node);

		list_del(v);
		kfree(cnt_id);
	}
}
