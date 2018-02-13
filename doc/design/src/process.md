# Overall process


## Global steps
1. Construct the dependency graph \\(D_G\\)
2. Extract the subDAG \\(G\\) of \\(D_G\\) rooted at the specified recipe(s)
3. Find a topological order of \\(G\\)

## Recipe-wise steps

For a recipe, we will have two variants: *pre* and *post*. The *pre*-recipe closely resembles the recipe as supplied by the user, the *post*-recipe contains exactly that information neccessary for other recipes dependent on this recipe. We will process all the recipes of \\(G\\) in an inverse topological order (i.e. from leaf to root), as follows:

1. Construct the *pre*-recipe for \\(v\\) by evaluating all globbing expressions for files, and processing the addition of those files.
2. Construct the *post*-recipe for \\(v\\) based on 
   * the *pre*-recipe of \\(v\\),
   * the *post*-recipe of all recipes \\(u\\) with \\((u,v) \in E(G)\\), and,
   * a topological order of the subDAG of \\(G\\) rooted at \\(v\\).

## Example

Consider the image below, depicting recipes \\(a,b,c,d,x,y\\) and \\(z\\) and the subDAG rooted ad \\(a\\). Dependencies are shown by a dashed arrow, e.g., \\(z\\) depends on \\(a\\). Recipes \\(x, y\\) and \\(z\\) are indicated in gray, as they do not belong to that subDAG (i.e., there is no path from \\(a\\) to any of those vertices). The blue arrows show the propagation of the *post*-recipes.

![Example](example.png)

For the subDAG of vertices \\( \\{ a,b,c,d \\} \\), both \\( (a, b, c, d) \\) and \\( (a,c,b,d) \\) are possible topological orders. A possible processing is thus
 1. *pre*-d
 2. *post*-d: *pre*-d, {}, {d}
 3. *pre*-b
 4. *post*-b: *pre*-b, {*post*-d}, {b, d}
 5. *pre*-c
 6. *post*-c: *pre*-c, {*post*-d}, {c, d}
 7. *pre*-a
 8. *post*-a: *pre*-a, {*post*-b, *post*-c}, {a, b, c, d}

 
