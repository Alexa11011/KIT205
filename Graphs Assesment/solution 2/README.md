# Solution 2: Articulation-First Removal

This solution treats articulation points as structurally critical parts of the internet graph.

Method:

1. Use Tarjan's algorithm to identify articulation points in the adjacency-list graph.
2. Among articulation points, select the highest-degree vertices first.
3. If more vertices must be removed than there are articulation points, fill the remainder using highest-degree vertices.
4. Measure the remaining connected components and the size of the largest connected component.

This is a structure-based strategy. It prioritises vertices whose removal is expected to split the graph into more disconnected pieces.

