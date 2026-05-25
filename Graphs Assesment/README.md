# Graphs Assessment

Topic: resilience of the internet autonomous-system graph under node failures.

This project uses the Week 8 and Week 9 adjacency-list graph code as its base. Those original weekly files are left unchanged. The assessment files keep the same core adjacency-list structs (`Edge`, `EdgeNodePtr`, `EdgeList`, `Graph`) and add loading, tests, Tarjan articulation points, and a resilience evaluation.

The two comparable solutions are:

- `solution 1`: remove the highest-degree autonomous systems first.
- `solution 2`: remove articulation points first, choosing the highest-degree articulation points.

Build and run:

```bash
make run
```

The program prints:

- unit tests for the graph data structure
- unit tests for articulation points
- unit tests for failure/component statistics
- an evaluation on `tech-internet-as.mtx`
- a CSV file, `comparison_results.csv`, with time and estimated memory comparisons for both solutions over increasing graph sizes
