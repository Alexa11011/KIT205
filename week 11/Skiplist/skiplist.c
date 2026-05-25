#include "skiplist.h"

int rand_level();

Skiplist new_skiplist()
{
	int i;

	Skiplist list;

	list.header = malloc(sizeof *(list.header));
	list.header->data = 0;
	list.header->next = malloc(SKIPLIST_MAX_LEVELS * sizeof *(list.header->next));
	for (i = 0; i < SKIPLIST_MAX_LEVELS; i++) {
		list.header->next[i] = NULL;
	}

	list.max_levels = SKIPLIST_MAX_LEVELS;

	return list;
}


void skiplist_insert(Skiplist *self, int data)
{
	SkipNodePtr update[SKIPLIST_MAX_LEVELS];
	SkipNodePtr current = self->header;
	SkipNodePtr node;
	int i, level;

	for (i = self->max_levels - 1; i >= 0; i--) {
		while (current->next[i] != NULL && current->next[i]->data < data) {
			current = current->next[i];
		}
		update[i] = current;
	}

	level = rand_level();

	node = malloc(sizeof *node);
	node->data = data;
	node->next = malloc(level * sizeof *(node->next));

	for (i = 0; i < level; i++) {
		node->next[i] = update[i]->next[i];
		update[i]->next[i] = node;
	}
}

void skiplist_print(Skiplist *self)
{
	SkipNodePtr current = self->header->next[0];
	while (current != NULL) {
		printf("%d ", current->data);
		current = current->next[0];
	}
	printf("\n");
}


SkipNodePtr skiplist_find(Skiplist *self, int data) {
	SkipNodePtr current = self->header;
	int i;
	for (i = self->max_levels-1; i >= 0; i--) {
		while (current->next[i] != NULL && current->next[i]->data < data)
			current = current->next[i];

		if (current->next[i] != NULL && current->next[i]->data == data)
			return current->next[i];
	}
	return NULL;
}

int rand_level()
{
	int level = 1;
	while (rand() < RAND_MAX / 2 && level < SKIPLIST_MAX_LEVELS)
		level++;
	return level;
}

void skiplist_delete(Skiplist *self, int data) {
	SkipNodePtr update[SKIPLIST_MAX_LEVELS];
	SkipNodePtr current = self->header;
	SkipNodePtr node;
	int i;

	for (i = self->max_levels - 1; i >= 0; i--) {
		while (current->next[i] != NULL && current->next[i]->data < data) {
			current = current->next[i];
		}
		update[i] = current;
	}

	node = current->next[0];
	if (node == NULL || node->data != data) {
		return;
	}

	for (i = 0; i < self->max_levels; i++) {
		if (update[i]->next[i] != node) {
			break;
		}
		update[i]->next[i] = node->next[i];
	}

	free(node->next);
	free(node);
}

