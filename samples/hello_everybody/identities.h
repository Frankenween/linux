/* SPDX-License-Identifier: GPL-2.0 */
#include <linux/types.h>

struct identity {
	char  name[20];
	int   id;
	bool  hired;
};

// Create a new identity with given name and id and hired = false.
// Add it to the identity list.
// ID values must be unique, so if given ID already exists, identity is replaced.
// @name: null-terminated string with length < 20
int identity_create(const char __kernel *name, int id);

// Find identity with given id in a list.
struct identity *identity_find(int id);

// Remove identity with given id. If no such identity, nothing happens.
void identity_destroy(int id);

// Find identity with given id and set hired field to 1.
// If no identity with such id exists, an error returned.
int identity_hire(int id);

// Remove all identities from list and free all allocated data
void clean_all_identities(void);
