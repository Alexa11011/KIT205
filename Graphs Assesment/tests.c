#include "tests.h"

#include "algorithms.h"
#include "graph.h"

#include <stdio.h>
#include <stdlib.h>

static int expect_int(const char* name, int actual, int expected)
{
	if (actual != expected) {
		printf("FAIL %s: expected %d, got %d\n", name, expected, actual);
		return 0;
	}

	printf("PASS %s\n", name);
	return 1;
}

static int test_graph_structure(void)
{
	Graph graph;
	int ok = 1;

	ok = ok && init_graph(&graph, 3);
	add_undirected_edge(&graph, 0, 1, 1);
	add_undirected_edge(&graph, 1, 2, 1);

	ok = ok && expect_int("graph vertices", graph.V, 3);
	ok = ok && expect_int("undirected edge records", graph_edge_count(&graph), 4);
	ok = ok && expect_int("middle degree", graph_degree(&graph, 1), 2);

	free_graph(&graph);
	return ok;
}

static int test_articulation_points(void)
{
	Graph graph;
	int articulation[5];
	int count;
	int ok = 1;

	ok = ok && init_graph(&graph, 5);
	add_undirected_edge(&graph, 0, 1, 1);
	add_undirected_edge(&graph, 1, 2, 1);
	add_undirected_edge(&graph, 1, 3, 1);
	add_undirected_edge(&graph, 3, 4, 1);

	count = find_articulation_points(&graph, articulation);

	ok = ok && expect_int("articulation count", count, 2);
	ok = ok && expect_int("vertex 1 is articulation", articulation[1], 1);
	ok = ok && expect_int("vertex 3 is articulation", articulation[3], 1);
	ok = ok && expect_int("leaf is not articulation", articulation[4], 0);

	free_graph(&graph);
	return ok;
}

static int test_failure_stats(void)
{
	Graph graph;
	int removed[6] = { 0 };
	ComponentStats stats;
	int ok = 1;

	ok = ok && init_graph(&graph, 6);
	add_undirected_edge(&graph, 0, 1, 1);
	add_undirected_edge(&graph, 1, 2, 1);
	add_undirected_edge(&graph, 3, 4, 1);
	add_undirected_edge(&graph, 4, 5, 1);

	stats = component_stats(&graph, removed);
	ok = ok && expect_int("two components initially", stats.components, 2);
	ok = ok && expect_int("largest initial component", stats.largest_component, 3);

	removed[1] = 1;
	stats = component_stats(&graph, removed);
	ok = ok && expect_int("components after bridge node removal", stats.components, 3);
	ok = ok && expect_int("largest component after removal", stats.largest_component, 3);

	free_graph(&graph);
	return ok;
}

static int test_articulation_solution_selection(void)
{
	Graph graph;
	int removed[5] = { 0 };
	int ok = 1;

	ok = ok && init_graph(&graph, 5);
	add_undirected_edge(&graph, 0, 1, 1);
	add_undirected_edge(&graph, 1, 2, 1);
	add_undirected_edge(&graph, 1, 3, 1);
	add_undirected_edge(&graph, 3, 4, 1);

	select_articulation_degree_vertices(&graph, 1, removed);
	ok = ok && expect_int("articulation solution removes vertex 1", removed[1], 1);
	ok = ok && expect_int("articulation solution keeps leaf", removed[4], 0);

	free_graph(&graph);
	return ok;
}

int run_tests(void)
{
	int ok = 1;

	printf("Unit tests\n");
	printf("----------\n");
	ok = ok && test_graph_structure();
	ok = ok && test_articulation_points();
	ok = ok && test_failure_stats();
	ok = ok && test_articulation_solution_selection();

	if (ok) {
		printf("All tests passed.\n\n");
	} else {
		printf("Some tests failed.\n\n");
	}

	return ok;
}
