#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <algorithm>
#include <vector>
using namespace std;

//TODO object oriented SAT solver? performance issues? readability? maintainability? extensibility?

#define UNDEFINED -1
#define TRUE 1
#define FALSE 0
#define DECISION_MARK 0

/* Global problem "sizes" */
uint numVariables;
uint numClauses;

/* Clauses and related data structures */
vector<vector<int> > clauses;

vector<vector<vector<int>* > > positiveClauses;
vector<vector<vector<int>* > > negativeClauses;

/* Model & backtrack stack */
vector<int> model;
vector<int> modelStack;
uint indexOfNextLitToPropagate;
uint decisionLevel;

/* Heuristic related variables */
vector<double> positiveLiteralActivity;
vector<double> negativeLiteralActivity;
double activityIncrement;
uint conflicts;
int activityIncrementUpdateRate = 10000000; //TODO is this strategy right? which value is best suited? IMPORTANT!!

/* Statistics */
uint propagations;
uint decisions;

/**
 * Returns the variable that this literal represents
 */
inline uint var(int literal) {
	return abs(literal);
}

/**
 * Reads the input problem file from the stdin stream and initializes
 * any remaining necessary data structures and variables.
 */
void initializeWithParsedInput() {
	// Skip comments
	char c = cin.get();
	while (c == 'c') {
		while (c != '\n')
			c = cin.get();
		c = cin.get();
	}

	// Read "cnf numVars numClauses"
	string aux;
	cin >> aux >> numVariables >> numClauses;
	clauses.resize(numClauses);

	// Initialize positive and negative appearances
	positiveClauses.resize(numVariables + 1);
	negativeClauses.resize(numVariables + 1);

	// Read clauses
	for (uint clause = 0; clause < numClauses; ++clause) {
		int literal;
		while (cin >> literal and literal != 0) {
			// add to the list of clauses
			clauses[clause].push_back(literal);

			// add to the list of positive-negative literals
			if (literal > 0) {
				positiveClauses[var(literal)].push_back((vector<int>*) &clauses[clause]);
			}
			else {
				negativeClauses[var(literal)].push_back((vector<int> *) &clauses[clause]);
			}
		}
	}

	// Initialize the remaining necessary variables
		// model and backtrack stack
	model.resize(numVariables + 1, UNDEFINED);
	indexOfNextLitToPropagate = 0;
	decisionLevel = 0;
		// heuristic
	positiveLiteralActivity.resize(numVariables + 1, 0.0);
	negativeLiteralActivity.resize(numVariables + 1, 0.0);
	activityIncrement = 1.0;
	conflicts = 0;
		// statistics
	propagations = 0;
	decisions = 0;
}

/**
 * TODO comment
 */
int valueForLiteral(int literal) {
//TODO could this be faster? there is a lot of if-else branching here!
	if (literal >= 0) {
		return model[literal];
	}
	else {
		if (model[-literal] == UNDEFINED) {
			return UNDEFINED;
		}
		else {
			return 1 - model[-literal];
		}
	}
}

/**
 * The current interpretation (model) is updated with the appropriate value
 * that will make the passed literal become true in the clause where it appeared.
 * The model stack is also increased, pushing the new value on top.
 *
 * @param literal the literal that will become true after the model update
 */
void setLiteralToTrue(int literal) {
	modelStack.push_back(literal);
	if (literal > 0) {
		model[literal] = TRUE;
	}
	else {
		model[-literal] = FALSE;
	}
}

/**
 * Updates the activity counter of the given literal. The activity increment
 * applied is also updated, in order to give more importance to recent activity.
 *
 * @param literal the literal which activity is to be updated
 */
void updateActivityForLiteral(int literal) {
	//update the activity of the literal (we are not distinguishing between positive
	// and negative literals here)
	uint index = var(literal);
	if (literal > 0) {
		positiveLiteralActivity[index] += activityIncrement;
	}
	else {
		negativeLiteralActivity[index] += activityIncrement;
	}
}

//TODO document
void updateActivityForConflictingClause(const vector<int>& clause) {
	//update the activity increment if necessary (every X conflicts)
	++conflicts;
	if ((conflicts % activityIncrementUpdateRate) == 0) {
		//decaying sum
		for (int i = 1; i <= numVariables; ++i) {
			positiveLiteralActivity[i] /= 2.0;
			negativeLiteralActivity[i] /= 2.0;
		}
	}

	//update activity for each literal
	for (int i = 0; i < clause.size(); ++i) {
		updateActivityForLiteral(clause[i]);
	}
}

//TODO document
bool propagateGivesConflict() {
	while (indexOfNextLitToPropagate < modelStack.size()) {
		//retrieve the literal to "be propagated"
		int literalToPropagate = modelStack[indexOfNextLitToPropagate];
		//move forward the "pointer" to the next literal that will be propagated
		++indexOfNextLitToPropagate;

		++propagations; //profiling purposes only

		//traverse only positive/negative appearances
		vector<vector<int>* > clausesToPropagate = literalToPropagate > 0 ?
				negativeClauses[var(literalToPropagate)] :
				positiveClauses[var(literalToPropagate)];

		//traverse the clauses
		for (uint i = 0; i < clausesToPropagate.size(); ++i) {
			//retrieve the next clause
			vector<int> clause = (vector<int>) *clausesToPropagate[i];

			//necessary variables initialization
			bool isSomeLiteralTrue = false;
			int undefinedLiterals = 0;
			int lastUndefinedLiteral = 0;

			//traverse the clause
			for (uint k = 0; not isSomeLiteralTrue and k < clause.size(); ++k) {
				int value = valueForLiteral(clause[k]);
				if (value == TRUE) {
					isSomeLiteralTrue = true;
				}
				else if (value == UNDEFINED) {
					++undefinedLiterals;
					lastUndefinedLiteral = clause[k];
				}
			}
			if (not isSomeLiteralTrue and undefinedLiterals == 0) {
				// A conflict has been found! All literals are false!
				updateActivityForConflictingClause(clause);
				return true;
			}
			else if (not isSomeLiteralTrue and undefinedLiterals == 1) {
				// The 'lastUndefinedLiteral' is "propagated", because it is the only one
				//  remaining in its clause that is undefined and all of the other literals
				//  of the clause are false
				// With the propagation, this literal is added to the stack, too!
				setLiteralToTrue(lastUndefinedLiteral);
			}
		}
	}
	return false;
}

//TODO document
void backtrack() {
	//TODO could this backtrack be faster? backjump? (non chronological backtrack)
	uint i = modelStack.size() - 1;
	int literal = 0;
	while (modelStack[i] != DECISION_MARK) { // 0 is the  mark
		literal = modelStack[i];
		model[var(literal)] = UNDEFINED;
		modelStack.pop_back();
		--i;
	}
	// at this point, literal is the last decision
	modelStack.pop_back(); // remove the  mark
	--decisionLevel;
	indexOfNextLitToPropagate = modelStack.size();
	setLiteralToTrue(-literal);  // reverse last decision
}


/**
 * Selects the next undefined variable from the current interpretation (model)
 * that will be decided and propagated. The heuristic used is based on variable
 * activity, in order to quickly find any possible contradictions.
 *
 * @return the next variable to be decided within the DPLL procedure or 0 if no
 * variable is currently undefined
 */
int getNextDecisionLiteral() {
	++decisions; //profiling purpose only
	double maximumActivity = 0.0;
	int mostActiveVariable = 0; // in case no variable is undefined, it will not be modified
	for (uint i = 1; i <= numVariables; ++i) {
		// check only undefined variables
		if (model[i] == UNDEFINED) {
			// search for the most active literal
			if (positiveLiteralActivity[i] >= maximumActivity) {
				maximumActivity = positiveLiteralActivity[i];
				mostActiveVariable = i;
			}
			if (negativeLiteralActivity[i] >= maximumActivity) {
				maximumActivity = negativeLiteralActivity[i];
				mostActiveVariable = -i;
			}
		}
	}
	//return the most active variable or, if none is undefined, 0
	return mostActiveVariable;
}

/**
 * Checks the model (interpretation) against the set of clauses, looking
 * for a contradiction. If any such contradiction is found, the program
 * exits and prints the conflicting clause.
 */
void checkmodel() {
	for (int i = 0; i < numClauses; ++i) {
		bool someTrue = false;
		for (int j = 0; not someTrue and j < clauses[i].size(); ++j) {
			someTrue = (valueForLiteral(clauses[i][j]) == TRUE);
		}
		if (not someTrue) {
			cout << "Error in model, clause is not satisfied:";
			for (int j = 0; j < clauses[i].size(); ++j) {
				cout << clauses[i][j] << " ";
			}
			cout << endl;
			exit(1);
		}
	}
}

/**
 * Exits the program with the appropriate output code. A model check
 * is also performed, in order to avoid errors.
 *
 * @param satisfiable whether the problem was found to be satisfiable or not
 */
void exitWithSatisfiability(bool satisfiable) {
	cout << "conflicts: " << conflicts << "  decisions: " << decisions << "  propagations:" << propagations << endl;
	if (satisfiable) {
		checkmodel();
		cout << "SATISFIABLE" << endl;
		exit(20);
	}
	else {
		cout << "UNSATISFIABLE" << endl;
		exit(10);
	}
}

//TODO document
void executeDPLL() {
	// DPLL algorithm
	while (true) {
		while (propagateGivesConflict()) {
			if (decisionLevel == 0) {
				//there are no more possible decisions (variable assignments),
				// which means that the problem is unsatisfiable
				exitWithSatisfiability(false);
			}
			backtrack();
		}
		int decisionLit = getNextDecisionLiteral();
		if (decisionLit == 0) {
			exitWithSatisfiability(true);
		}
		// start new decision level:
		modelStack.push_back(DECISION_MARK);  // push mark indicating new
		++indexOfNextLitToPropagate;
		++decisionLevel;
		setLiteralToTrue(decisionLit); // now push decisionLit on top of the mark
	}
}

/**
 * Checks for any unit clause and sets the appropriate values in the
 * model accordingly. If a contradiction is found among these unit clauses,
 * early failure is triggered.
 */
void checkUnitClauses() {
	for (uint i = 0; i < numClauses; ++i) {
		if (clauses[i].size() == 1) {
			int literal = clauses[i][0];
			int value = valueForLiteral(literal);
			if (value == FALSE) {
				// This condition will only occur if at least a couple of unit clauses
				//  exist with opposite literals (p and ¬p, for example). When the first
				//  unit clause is used to set a value in the model (see the next condition),
				//  the second unit clause with the same variable will cause a contradiction
				//  to be found, i.e., the problem is unsatisfiable!
				exitWithSatisfiability(false);
			}
			else if (value == UNDEFINED) {
				setLiteralToTrue(literal);
			}
		}
		//TODO could unit clauses be "deleted"? (at least avoid their traversal further on)
	}
}

/**
 * Main program. It reads the SAT problem from the stdin stream, checks unit clauses and
 * executes the DPLL algorithm.
 */
int main(int argc, char **argv) {
	activityIncrementUpdateRate = atoi(argv[1]);

	// Read the problem file (available at the stdin stream) and
	//  initialize the rest of necessary variables
	initializeWithParsedInput();

	// Take care of initial unit clauses, if any
	checkUnitClauses();

	// Execute the main DPLL procedure
	executeDPLL();
}
