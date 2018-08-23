# Notation

**Graph-based notations**
 * A directed graph \\(G\\) is a pair \\((V,E)\\) where \\(V\\) is the set of vertices, and \\(E \subseteq V^2\\) the set of edges. We denote \\(V(G) = V\\) and \\(V(E) = E\\). For any edge \\(e = (u,v) \\), source\\((e) = u\\). and target\\((e) = v \\).
 * The out-degree of a vertex \\( v \\) is the number of distinct edges with source \\(v\\), the in-degree the number of edges is with target \\( v\\). A root vertex has it's in-degree zero, a leaf it's out-degree zero.
 * A path is an ordered set of vertices \\(P\\) with \\(|P| \geq 2\\) such that, for any two consecutive vertices \\(u, v\\) in that sequence, \\( (u,v)  \in E \\). Such a path \\(P\\) connects the first to the last vertex in the ordered set.
 * A graph is cyclic if there exists a path \\(P\\) connecting \\(v\\) to \\(v\\) (for any \\(v\\)). If not, the graph is said to be acyclic.
 * A directed, acyclic graph (DAG) is a directed graph which is a acyclic. In such a DAG, a topological order is an ordered set of all vertices \\(V\\) such that, for any edge \\( (u,v) \\), \\(u\\) occurs strictly before \\(v\\) in the ordered set. Hence, a topological order always has a leaf node as first and root node as last element. Note that such a topological order always exists but is not unique.
  * The induced subgraph of \\(G\\) on a vertex set \\(S \subset V(G) \\) is the graph \\(H = (S, E^\prime)\\) with
 $$ E^\prime = \\{ (u,v) \in E(G) \mid u,v \in S \\}$$
 * For a DAG \\(G\\) and a set of vertices \\(S\\), the subDAG of \\(G\\) rooted at \\(S\\) is the induced subgraph of \\(G\\) on exactly those vertices \\(v\\) such either \\(v \in S\\) or any vertex of \\(S\\) is connected to \\(v\\) by a path.
 
**Recipe-based notations**
 * The dependency graph is the graph where the recipes are vertices, and for any two recipes \\( u,v \\): \\( (u,v) \in E \Leftrightarrow \\) recipe \\(u\\) is dependent on recipe \\(v\\).
