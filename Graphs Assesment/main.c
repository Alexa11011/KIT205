#include "algorithms.h"
#include "graph.h"
#include "tests.h"

#include <time.h>
#include <stdio.h>
#include <stdlib.h>

static int build_subgraph_first_n(Graph* source, int n, Graph* subgraph)
{
	if (!init_graph(subgraph, n)) {
		return 0;
	}

	for (int from = 0; from < n; from++) {
		for (EdgeNodePtr current = source->edges[from].head; current != NULL; current = current->next) {
			int to = current->edge.to_vertex;
			if (to < n) {
				add_edge(subgraph, from, to, current->edge.weight);
			}
		}
	}

	return 1;
}

static long estimate_degree_solution_peak_bytes(int vertices)
{
	return 3L * (long)vertices * (long)sizeof(int);
}

static long estimate_articulation_solution_peak_bytes(int vertices)
{
	return 5L * (long)vertices * (long)sizeof(int);
}

static double seconds_since(clock_t start, clock_t end)
{
	return (double)(end - start) / CLOCKS_PER_SEC;
}

static void compare_solutions(Graph* full_graph, const char* output_path)
{
	int sizes[] = {
		1000, 1500, 2000, 2500, 3000, 3500, 4000, 5000,
		6000, 7000, 8000, 9000, 10000, 12000, 14000, 16000,
		18000, 20000, 22000, 24000, 26000, 28000, 30000, 32000,
		34000, 36000, 38000, 40000
	};
	int size_count = sizeof sizes / sizeof sizes[0];
	FILE* output = fopen(output_path, "w");

	if (output == NULL) {
		printf("Could not open %s for writing.\n", output_path);
		return;
	}

	fprintf(output, "n,solution1_time_seconds,solution1_estimated_space_bytes,solution1_space_complexity,solution2_time_seconds,solution2_estimated_space_bytes,solution2_space_complexity\n");

	printf("\nWriting comparison data to %s\n", output_path);

	for (int i = 0; i <= size_count; i++) {
		Graph graph;
		int n = i < size_count ? sizes[i] : full_graph->V;
		int remove_count;
		int* removed;
		clock_t start;
		clock_t end;
		double degree_seconds;
		double articulation_seconds;

		if (n > full_graph->V) {
			continue;
		}

		if (!build_subgraph_first_n(full_graph, n, &graph)) {
			printf("Could not build subgraph for n=%d\n", n);
			continue;
		}

		removed = calloc(graph.V, sizeof *removed);
		if (removed == NULL) {
			printf("Could not allocate benchmark array for n=%d\n", n);
			free_graph(&graph);
			continue;
		}

		remove_count = graph.V / 100;
		if (remove_count < 1) {
			remove_count = 1;
		}

		start = clock();
		select_highest_degree_vertices(&graph, remove_count, removed);
		component_stats(&graph, removed);
		end = clock();
		degree_seconds = seconds_since(start, end);

		start = clock();
		select_articulation_degree_vertices(&graph, remove_count, removed);
		component_stats(&graph, removed);
		end = clock();
		articulation_seconds = seconds_since(start, end);

		fprintf(output, "%d,%.6f,%ld,O(n),%.6f,%ld,O(n)\n",
			graph.V,
			degree_seconds,
			estimate_degree_solution_peak_bytes(graph.V),
			articulation_seconds,
			estimate_articulation_solution_peak_bytes(graph.V));

		printf("n=%d complete\n", graph.V);

		free(removed);
		free_graph(&graph);
	}

	fclose(output);
}

static void print_resilience_row(Graph* graph, int remove_count, int trials)
{
	int* removed = calloc(graph->V, sizeof *removed);
	double random_largest_total = 0.0;
	double random_components_total = 0.0;
	ComponentStats degree;
	ComponentStats articulation;

	if (removed == NULL) {
		printf("Error allocating removed array.\n");
		return;
	}

	select_highest_degree_vertices(graph, remove_count, removed);
	degree = component_stats(graph, removed);

	select_articulation_degree_vertices(graph, remove_count, removed);
	articulation = component_stats(graph, removed);

	for (int trial = 0; trial < trials; trial++) {
		ComponentStats random_stats;
		select_random_vertices(graph, remove_count, 1000u + (unsigned int)trial, removed);
		random_stats = component_stats(graph, removed);
		random_largest_total += random_stats.largest_component;
		random_components_total += random_stats.components;
	}

	printf("%8d | %17d | %17d | %23d | %23d | %17.1f | %18.1f\n",
		remove_count,
		degree.largest_component,
		degree.components,
		articulation.largest_component,
		articulation.components,
		random_largest_total / trials,
		random_components_total / trials);

	free(removed);
}

static void run_evaluation(const char* dataset_path)
{
	Graph graph;
	int* articulation;
	int articulation_count;
	int undirected_edges;
	ComponentStats initial_stats;
	int* removed;
	int remove_counts[] = { 0, 40, 200, 400, 800 };
	int remove_options = sizeof remove_counts / sizeof remove_counts[0];

	printf("Loading dataset: %s\n", dataset_path);
	if (!load_matrix_market_symmetric(&graph, dataset_path)) {
		printf("Could not load dataset.\n");
		return;
	}

	undirected_edges = graph_edge_count(&graph) / 2;
	printf("Dataset loaded: %d vertices, %d undirected edges\n\n", graph.V, undirected_edges);

	articulation = calloc(graph.V, sizeof *articulation);
	removed = calloc(graph.V, sizeof *removed);
	if (articulation == NULL || removed == NULL) {
		printf("Error allocating evaluation arrays.\n");
		free(articulation);
		free(removed);
		free_graph(&graph);
		return;
	}

	articulation_count = find_articulation_points(&graph, articulation);
	initial_stats = component_stats(&graph, removed);

	printf("Standard algorithm: articulation points\n");
	printf("---------------------------------------\n");
	printf("Articulation points found: %d\n", articulation_count);
	printf("Initial connected components: %d\n", initial_stats.components);
	printf("Initial largest component: %d vertices\n\n", initial_stats.largest_component);

	printf("Specific problem: AS network resilience\n");
	printf("---------------------------------------\n");
	printf("Compare two critical-node selection solutions with random failures.\n");
	printf("Solution 1: remove highest-degree autonomous systems.\n");
	printf("Solution 2: remove articulation points first, choosing highest-degree articulation points.\n");
	printf("Trials for random failures: 5\n\n");
	printf(" Removed | Degree largest CC | Degree components | Articulation largest CC | Articulation components | Random largest CC | Random components\n");
	printf("---------|-------------------|-------------------|-------------------------|-------------------------|-------------------|------------------\n");
	for (int i = 0; i < remove_options; i++) {
		print_resilience_row(&graph, remove_counts[i], 5);
	}

	compare_solutions(&graph, "comparison_results.csv");

	free(articulation);
	free(removed);
	free_graph(&graph);
}

int main(int argc, char** argv)
{
	const char* dataset_path = "tech-internet-as.mtx";

	if (argc > 1) {
		dataset_path = argv[1];
	}

	if (!run_tests()) {
		return 1;
	}

	run_evaluation(dataset_path);
	return 0;
}
