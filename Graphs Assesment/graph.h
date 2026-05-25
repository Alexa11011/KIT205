#ifndef GRAPH_H
#define GRAPH_H

typedef struct edge {
	int to_vertex;
	int weight;
} Edge;

typedef struct edgeNode {
	Edge edge;
	struct edgeNode* next;
} *EdgeNodePtr;

typedef struct edgeList {
	EdgeNodePtr head;
} EdgeList;

typedef struct graph {
	int V;
	EdgeList* edges;
} Graph;

int init_graph(Graph* self, int vertices);
void add_edge(Graph* self, int from, int to, int w);
void add_undirected_edge(Graph* self, int a, int b, int w);
void free_graph(Graph* self);
int graph_edge_count(Graph* self);
int graph_degree(Graph* self, int vertex);
int load_matrix_market_symmetric(Graph* self, const char* path);

#endif
