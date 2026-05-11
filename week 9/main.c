#include <graph.h>
#include <stdio.h>
#include <stdlib.h>

void add_edge(Graph* self, int from, int to, int w) {
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

void free_graph(Graph* self) {
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

void page_rank(Graph* self, double* ranks, int iterations, double damping) {
    double* next_ranks = malloc(self->V * sizeof *next_ranks);
    int* out_degrees = malloc(self->V * sizeof *out_degrees);

    if (next_ranks == NULL || out_degrees == NULL) {
        printf("Error allocating PageRank arrays!\n");
        free(next_ranks);
        free(out_degrees);
        return;
    }

    for (int v = 0; v < self->V; v++) {
        ranks[v] = 1.0 / self->V;
        out_degrees[v] = 0;

        EdgeNodePtr current = self->edges[v].head;
        while (current != NULL) {
            out_degrees[v]++;
            current = current->next;
        }
    }

    for (int i = 0; i < iterations; i++) {
        double sink_rank = 0.0;

        for (int v = 0; v < self->V; v++) {
            next_ranks[v] = (1.0 - damping) / self->V;

            if (out_degrees[v] == 0) {
                sink_rank += ranks[v];
            }
        }

        sink_rank = damping * sink_rank / self->V;
        for (int v = 0; v < self->V; v++) {
            next_ranks[v] += sink_rank;
        }

        for (int from = 0; from < self->V; from++) {
            EdgeNodePtr current = self->edges[from].head;

            while (current != NULL) {
                int to = current->edge.to_vertex;
                next_ranks[to] += damping * ranks[from] / out_degrees[from];
                current = current->next;
            }
        }

        for (int v = 0; v < self->V; v++) {
            ranks[v] = next_ranks[v];
        }
    }

    free(next_ranks);
    free(out_degrees);
}

int main(void) {
    Graph G;
    int from;
    int to;

    FILE* file = fopen("musae_git_edges.csv", "r");
    if (file == NULL) {
        printf("Error opening file!\n");
        return 1;
    }

    if (fscanf(file, "%d", &G.V) != 1) {
        printf("Error reading number of vertices!\n");
        fclose(file);
        return 1;
    }

    G.edges = malloc(G.V * sizeof *G.edges);
    if (G.edges == NULL) {
        printf("Error allocating graph!\n");
        fclose(file);
        return 1;
    }

    for (int v = 0; v < G.V; v++) {
        G.edges[v].head = NULL;
    }

    while (fscanf(file, "%d,%d", &from, &to) == 2) {
        add_edge(&G, from, to, 0);
    }

    fclose(file);

    double* ranks = malloc(G.V * sizeof *ranks);
    if (ranks == NULL) {
        printf("Error allocating ranks!\n");
        free_graph(&G);
        return 1;
    }

    page_rank(&G, ranks, 100, 0.85);

    for (int v = 0; v < G.V; v++) {
        printf("%d: %.6f\n", v, ranks[v]);
    }

    free(ranks);
    free_graph(&G);

    return 0;
}
