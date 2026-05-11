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

    int* in_degrees = malloc(G.V * sizeof *in_degrees);
    if (in_degrees == NULL) {
        printf("Error allocating in-degrees!\n");
        free_graph(&G);
        return 1;
    }

    for (int v = 0; v < G.V; v++) {
        in_degrees[v] = 0;
    }

    for (int v = 0; v < G.V; v++) {
        EdgeNodePtr current = G.edges[v].head;

        while (current != NULL) {
            in_degrees[current->edge.to_vertex]++;
            current = current->next;
        }
    }

    for (int v = 0; v < G.V; v++) {
        printf("%d: %d\n", v, in_degrees[v]);
    }

    free(in_degrees);
    free_graph(&G);

    return 0;
}
