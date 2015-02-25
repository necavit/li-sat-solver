#include <iostream>
#include <stdlib.h>
#include <algorithm>
#include <vector>
using namespace std;

#define UNDEF -1
#define TRUE 1
#define FALSE 0

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

void readClauses() {
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
}

int currentValueInModel(int literal) {
	if (literal >= 0) {
		return model[literal];
	}
	else {
		if (model[-literal] == UNDEF) {
			return UNDEF;
		}
		else {
			return 1 - model[-literal];
		}
	}
}

void setLiteralToTrue(int lit) {
	modelStack.push_back(lit);
	if (lit > 0) {
		model[lit] = TRUE;
	}
	else {
		model[-lit] = FALSE;
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
				int val = currentValueInModel(clause[k]);
				if (val == TRUE) {
					isSomeLiteralTrue = true;
				}
				else if (val == UNDEF) {
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
	int lit = 0;
	while (modelStack[i] != 0) { // 0 is the DL mark
		lit = modelStack[i];
		model[abs(lit)] = UNDEF;
		modelStack.pop_back();
		--i;
	}
	// at this point, lit is the last decision
	modelStack.pop_back(); // remove the DL mark
	--decisionLevel;
	indexOfNextLitToPropagate = modelStack.size();
	setLiteralToTrue(-lit);  // reverse last decision
}

// Heuristic for finding the next decision literal:
int getNextDecisionLiteral() {
	++decisions; //profiling purpose only

	//TODO enhance this heuristic (implement activity based decision)
	// stupid heuristic:
	for (uint i = 1; i <= numVariables; ++i) {
		if (model[i] == UNDEF) {
			return i;  // returns first UNDEF var, positively
		}
	}
	return 0; // reurns 0 when all literals are defined
}

void checkmodel() {
	for (int i = 0; i < numClauses; ++i) {
		bool someTrue = false;
		for (int j = 0; not someTrue and j < clauses[i].size(); ++j) {
			someTrue = (currentValueInModel(clauses[i][j]) == TRUE);
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

int main() {
	readClauses(); // reads numVars, numClauses and clauses
	model.resize(numVariables + 1, UNDEF);
	indexOfNextLitToPropagate = 0;
	decisionLevel = 0;
	propagations = 0;
	decisions = 0;

	// Take care of initial unit clauses, if any
	for (uint i = 0; i < numClauses; ++i)
		if (clauses[i].size() == 1) {
			int lit = clauses[i][0];
			int val = currentValueInModel(lit);
			if (val == FALSE) {
				cout << "UNSATISFIABLE" << endl;
				return 10;
			}
			else if (val == UNDEF) {
				setLiteralToTrue(lit);
			}
		}

	// DPLL algorithm
	while (true) {
		while (propagateGivesConflict()) {
			if (decisionLevel == 0) {
				cout << "UNSATISFIABLE" << endl;
				return 10;
			}
			backtrack();
		}
		int decisionLit = getNextDecisionLiteral();
		if (decisionLit == 0) {
			checkmodel();
			cout << "SATISFIABLE" << endl;
			return 20;
		}
		// start new decision level:
		modelStack.push_back(0);  // push mark indicating new DL
		++indexOfNextLitToPropagate;
		++decisionLevel;
		setLiteralToTrue(decisionLit); // now push decisionLit on top of the mark
	}
}
