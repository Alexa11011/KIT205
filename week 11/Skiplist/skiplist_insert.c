void skiplist_insert(Skiplist* self, int data)
{
	SkipNodePtr update[SKIPLIST_MAX_LEVELS];
	SkipNodePtr current = self->header;
	int i, level;

	/* Search phase — fill update[] with the rightmost node
	   at each level that sits just before the insertion point */
	for (i = self->max_levels - 1; i >= 0; i--) {
		while (current->next[i] != NULL && current->next[i]->data < data)
			current = current->next[i];
		update[i] = current;
	}

	level = rand_level();

	/* Create new node */
	SkipNodePtr node = malloc(sizeof * node);
	node->data = data;
	node->next = malloc(level * sizeof * (node->next));

	/* Rewire forward pointers at each level */
	for (i = 0; i < level; i++) {
		node->next[i] = update[i]->next[i];
		update[i]->next[i] = node;
	}
}