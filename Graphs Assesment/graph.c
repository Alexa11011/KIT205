#include "graph.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int init_graph(Graph* self, int vertices)
{
	self->V = vertices;
	self->edges = malloc(vertices * sizeof *self->edges);
	if (self->edges == NULL) {
		self->V = 0;
		return 0;
	}

	for (int v = 0; v < vertices; v++) {
		self->edges[v].head = NULL;
	}

	return 1;
}

void add_edge(Graph* self, int from, int to, int w)
{
	EdgeNodePtr new_node = malloc(sizeof *new_node);
	if (new_node == NULL) {
		printf("Error allocating edge!\n");
		return;
	}

	new_node->edge.to_vertex = to;
	new_node->edge.weight = w;
	new_node->next = self->edges[from].head;
	self->edges[from].head = new_node;
}

void add_undirected_edge(Graph* self, int a, int b, int w)
{
	add_edge(self, a, b, w);
	if (a != b) {
		add_edge(self, b, a, w);
	}
}

void free_graph(Graph* self)
{
	for (int v = 0; v < self->V; v++) {
		EdgeNodePtr current = self->edges[v].head;

		while (current != NULL) {
			EdgeNodePtr next = current->next;
			free(current);
			current = next;
		}
	}

	free(self->edges);
	self->edges = NULL;
	self->V = 0;
}

int graph_edge_count(Graph* self)
{
	int count = 0;

	for (int v = 0; v < self->V; v++) {
		EdgeNodePtr current = self->edges[v].head;
		while (current != NULL) {
			count++;
			current = current->next;
		}
	}

	return count;
}

int graph_degree(Graph* self, int vertex)
{
	int degree = 0;
	EdgeNodePtr current = self->edges[vertex].head;

	while (current != NULL) {
		degree++;
		current = current->next;
	}

	return degree;
}

int load_matrix_market_symmetric(Graph* self, const char* path)
{
	FILE* file = fopen(path, "r");
	char line[256];
	int rows;
	int cols;
	int entries;
	int from;
	int to;

	if (file == NULL) {
		printf("Error opening %s\n", path);
		return 0;
	}

	do {
		if (fgets(line, sizeof line, file) == NULL) {
			fclose(file);
			return 0;
		}
	} while (line[0] == '%');

	if (sscanf(line, "%d %d %d", &rows, &cols, &entries) != 3 || rows != cols) {
		fclose(file);
		return 0;
	}

	if (!init_graph(self, rows)) {
		fclose(file);
		return 0;
	}

	while (fscanf(file, "%d %d", &from, &to) == 2) {
		add_undirected_edge(self, from - 1, to - 1, 1);
	}

	fclose(file);
	(void)entries;
	return 1;
}
