#include "algorithms.h"

#include <stdlib.h>

static void articulation_dfs(Graph* graph, int vertex, int parent, int* visited,
	int* discovery, int* low, int* is_articulation, int* time)
{
	int children = 0;

	visited[vertex] = 1;
	discovery[vertex] = low[vertex] = ++(*time);

	for (EdgeNodePtr current = graph->edges[vertex].head; current != NULL; current = current->next) {
		int to = current->edge.to_vertex;

		if (!visited[to]) {
			children++;
			articulation_dfs(graph, to, vertex, visited, discovery, low, is_articulation, time);

			if (low[to] < low[vertex]) {
				low[vertex] = low[to];
			}

			if (parent != -1 && low[to] >= discovery[vertex]) {
				is_articulation[vertex] = 1;
			}
		} else if (to != parent && discovery[to] < low[vertex]) {
			low[vertex] = discovery[to];
		}
	}

	if (parent == -1 && children > 1) {
		is_articulation[vertex] = 1;
	}
}

int find_articulation_points(Graph* graph, int* is_articulation)
{
	int* visited = calloc(graph->V, sizeof *visited);
	int* discovery = calloc(graph->V, sizeof *discovery);
	int* low = calloc(graph->V, sizeof *low);
	int time = 0;
	int count = 0;

	if (visited == NULL || discovery == NULL || low == NULL) {
		free(visited);
		free(discovery);
		free(low);
		return -1;
	}

	for (int v = 0; v < graph->V; v++) {
		is_articulation[v] = 0;
	}

	for (int v = 0; v < graph->V; v++) {
		if (!visited[v]) {
			articulation_dfs(graph, v, -1, visited, discovery, low, is_articulation, &time);
		}
	}

	for (int v = 0; v < graph->V; v++) {
		if (is_articulation[v]) {
			count++;
		}
	}

	free(visited);
	free(discovery);
	free(low);
	return count;
}

ComponentStats component_stats(Graph* graph, const int* removed)
{
	ComponentStats stats = { 0, 0, 0 };
	int* visited = calloc(graph->V, sizeof *visited);
	int* stack = malloc(graph->V * sizeof *stack);

	if (visited == NULL || stack == NULL) {
		free(visited);
		free(stack);
		return stats;
	}

	for (int start = 0; start < graph->V; start++) {
		int top = 0;
		int size = 0;

		if (removed[start] || visited[start]) {
			continue;
		}

		stats.remaining_vertices++;
		stats.components++;
		visited[start] = 1;
		stack[top++] = start;

		while (top > 0) {
			int vertex = stack[--top];
			size++;

			for (EdgeNodePtr current = graph->edges[vertex].head; current != NULL; current = current->next) {
				int to = current->edge.to_vertex;
				if (!removed[to] && !visited[to]) {
					visited[to] = 1;
					stack[top++] = to;
					stats.remaining_vertices++;
				}
			}
		}

		if (size > stats.largest_component) {
			stats.largest_component = size;
		}
	}

	free(visited);
	free(stack);
	return stats;
}

void select_highest_degree_vertices(Graph* graph, int count, int* removed)
{
	for (int v = 0; v < graph->V; v++) {
		removed[v] = 0;
	}

	for (int i = 0; i < count; i++) {
		int best_vertex = -1;
		int best_degree = -1;

		for (int v = 0; v < graph->V; v++) {
			int degree;
			if (removed[v]) {
				continue;
			}

			degree = graph_degree(graph, v);
			if (degree > best_degree) {
				best_degree = degree;
				best_vertex = v;
			}
		}

		if (best_vertex != -1) {
			removed[best_vertex] = 1;
		}
	}
}

void select_articulation_degree_vertices(Graph* graph, int count, int* removed)
{
	int* is_articulation = calloc(graph->V, sizeof *is_articulation);

	for (int v = 0; v < graph->V; v++) {
		removed[v] = 0;
	}

	if (is_articulation == NULL) {
		return;
	}

	find_articulation_points(graph, is_articulation);

	for (int i = 0; i < count; i++) {
		int best_vertex = -1;
		int best_degree = -1;

		for (int v = 0; v < graph->V; v++) {
			int degree;
			if (removed[v] || !is_articulation[v]) {
				continue;
			}

			degree = graph_degree(graph, v);
			if (degree > best_degree) {
				best_degree = degree;
				best_vertex = v;
			}
		}

		if (best_vertex == -1) {
			for (int v = 0; v < graph->V; v++) {
				int degree;
				if (removed[v]) {
					continue;
				}

				degree = graph_degree(graph, v);
				if (degree > best_degree) {
					best_degree = degree;
					best_vertex = v;
				}
			}
		}

		if (best_vertex != -1) {
			removed[best_vertex] = 1;
		}
	}

	free(is_articulation);
}

void select_random_vertices(Graph* graph, int count, unsigned int seed, int* removed)
{
	int* order = malloc(graph->V * sizeof *order);

	if (order == NULL) {
		return;
	}

	for (int v = 0; v < graph->V; v++) {
		removed[v] = 0;
		order[v] = v;
	}

	srand(seed);
	for (int i = graph->V - 1; i > 0; i--) {
		int j = rand() % (i + 1);
		int temp = order[i];
		order[i] = order[j];
		order[j] = temp;
	}

	for (int i = 0; i < count && i < graph->V; i++) {
		removed[order[i]] = 1;
	}

	free(order);
}
