# SAT solver

A C++ [SAT solver](http://en.wikipedia.org/wiki/Boolean_satisfiability_problem#Algorithms_for_solving_SAT) 
implementation, based on the [DPLL](http://en.wikipedia.org/wiki/DPLL_algorithm) algorithm.

This solver is an assignment for the 
[Logic in Information Technology](http://www.fib.upc.edu/en/estudiar-enginyeria-informatica/assignatures/LI.html) 
course and is meant to provide us with the basics of SAT solving techniques.

## Implemented features

Based on the code that was already provided, the following enhancements have been implemented in order
to lower the execution time of the solver:

#### Occur lists

The BCP (Boolean Constraint Propagation) procedure (called `propagateGivesConflict()` in the code)
does not traverse the whole set of clauses anymore. Instead, a couple of additional data structures
are used in order to minimize the amount of clauses being visited each time a literal is propagated.

These data structures are called *occurrence lists* and are a couple of lists, indexed by variable. Each
item in these lists is, indeed, another list, containing the clauses in which the variable appears as
a positive or negative literal. Thus, the lists are named `positiveClauses` and `negativeClauses`,
respectively.

The occurrence lists are built during initialization time, as clauses are fed into the solver. In order
to reduce the number of memory read operations, instead of just storing the index of the clause, a
pointer to the clause is kept.

#### Activity-based decision heuristic

The heuristic used to choose the next literal to be *decided* by the DPLL algorithm has also been updated
with respect of what was implemented in the example code that was provided.

The decision heuristic works as follows:

* An *activity* counter is kept for each literal, be it positive or negative, indicating the number of
conflicts in which the literal was involved.
* Each time a conflict is found, the activity of *all* the literals in the clause that causes the conflict
is incremented by a constant factor (1, in our case).
* Because recent conflicts should be given more importance than older ones (in order to rapidly force a
backtrack operation), the activity of all the literals is diminished from time to time. More concretely,
it is divided by 2 every 1000 conflicts.
* When a *new decision* has to be made, the literal with the highest activity of those variables still
undefined in the model is chosen and returned to the DPLL procedure.

It is important to note that 

## Benchmark framework

*Work In Progress*

## Results

## Scripts and sources


