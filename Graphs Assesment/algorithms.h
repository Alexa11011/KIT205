#ifndef ALGORITHMS_H
#define ALGORITHMS_H

#include "graph.h"

typedef struct component_stats {
	int remaining_vertices;
	int components;
	int largest_component;
} ComponentStats;

int find_articulation_points(Graph* graph, int* is_articulation);
ComponentStats component_stats(Graph* graph, const int* removed);
void select_highest_degree_vertices(Graph* graph, int count, int* removed);
void select_articulation_degree_vertices(Graph* graph, int count, int* removed);
void select_random_vertices(Graph* graph, int count, unsigned int seed, int* removed);

#endif
