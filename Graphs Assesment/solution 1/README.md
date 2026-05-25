# Solution 1: Highest-Degree Removal

This solution treats highly connected autonomous systems as the most critical parts of the internet graph.

Method:

1. Count the degree of every vertex in the adjacency-list graph.
2. Select the vertices with the highest degrees.
3. Mark those vertices as removed.
4. Measure the remaining connected components and the size of the largest connected component.

This is a hub-based strategy. It assumes that removing highly connected AS nodes will disrupt many paths at once.

