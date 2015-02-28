#include <iostream>
#include <stdlib.h>
#include <algorithm>
#include <vector>
using namespace std;

#define UNDEFINED -1
#define TRUE 1
#define FALSE 0
#define DECISION_MARK 0

uint numVariables;
uint numClauses;
uint propagations;
uint decisions;
vector<vector<int> > clauses;
vector<vector<int> > positiveClauses;
vector<vector<int> > negativeClauses;
vector<int> model;
vector<int> modelStack;
uint indexOfNextLitToPropagate;
uint decisionLevel;

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
	positiveClauses.resize(numVariables);
	negativeClauses.resize(numVariables);

	// Read clauses
	for (uint clause = 0; clause < numClauses; ++clause) {
		int literal;
		while (cin >> literal and literal != 0) {
			clauses[clause].push_back(literal);
			if (literal > 0) {
				positiveClauses[abs(literal)].push_back(clause);
			}
			else {
				negativeClauses[abs(literal)].push_back(clause);
			}
		}
	}

	// Initialize the remaining necessary variables
	model.resize(numVariables + 1, UNDEFINED);
	indexOfNextLitToPropagate = 0;
	decisionLevel = 0;
	propagations = 0;
	decisions = 0;
}

/**
 * The current interpretation (model) is updated with the appropriate value
 * that will make the passed literal become true in the clause where it appeared.
 * The model stack is also increased, pushing the new value on top.
 *
 * @param literal the literal that will become true after the model update
 */
int valueForLiteral(int literal) {
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

bool propagateGivesConflict() {
	while (indexOfNextLitToPropagate < modelStack.size()) {
		//retrieve the literal to "be propagated"
		int literalToPropagate = modelStack[indexOfNextLitToPropagate];

		++indexOfNextLitToPropagate;
		++propagations; //profiling purposes only

		//TODO traverse only positive/negative appearances!
		vector<int> clausesToPropagate = literalToPropagate > 0 ?
				positiveClauses[abs(literalToPropagate)] :
				negativeClauses[abs(literalToPropagate)];

		//traverse the clauses
		for (uint i = 0; i < clausesToPropagate.size(); ++i) {
			bool isSomeLiteralTrue = false;
			int undefinedLiterals = 0;
			int lastUndefinedLiteral = 0;

			//retrieve the next clause
			vector<int> clause = clauses[clausesToPropagate[i]];

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
			if (not isSomeLiteralTrue and undefinedLiterals == 0)	{
				return true; // conflict! all lits false
			}
			else if (not isSomeLiteralTrue and undefinedLiterals == 1) {
				setLiteralToTrue(lastUndefinedLiteral);
			}
		}
	}
	return false;
}

void backtrack() {
	uint i = modelStack.size() - 1;
	int literal = 0;
	while (modelStack[i] != DECISION_MARK) { // 0 is the  mark
		literal = modelStack[i];
		model[abs(literal)] = UNDEFINED;
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

	//TODO enhance this heuristic (implement activity based decision)
	// stupid heuristic:
	for (uint i = 1; i <= numVariables; ++i) {
		if (model[i] == UNDEFINED) {
			return i;  // returns first undefined variable
		}
	}
	return 0; // reurns 0 when all literals are defined
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

void executeDPLL() {
	// DPLL algorithm
	while (true) {
		while (propagateGivesConflict()) {
			if (decisionLevel == 0) {
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

int main() {
	// Read the problem file (available at the stdin stream) and
	//  initialize the rest of necessary variables
	initializeWithParsedInput();

	// Take care of initial unit clauses, if any
	checkUnitClauses();

	// Execute the main DPLL procedure
	executeDPLL();
}
