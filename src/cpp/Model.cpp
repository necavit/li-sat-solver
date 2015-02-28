/*
 * Model.cpp
 *
 *  Created on: Feb 28, 2015
 *      Author: David Martínez Rodríguez
 */

#include "Model.hpp"

Model::Model(int numVariables) {
	model.resize(numVariables + 1, UNDEFINED);
}

Model::~Model() {
	delete model;
}

int Model::valueForLiteral(int literal) {
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

void Model::setLiteralToTrue(int literal) {
	modelStack.push_back(literal);
	if (literal > 0) {
		model[literal] = TRUE;
	}
	else {
		model[-literal] = FALSE;
	}
}

bool Model::isVariableUndefined(int variable) {
	return model[variable] == UNDEFINED;
}

void Model::setVariableUndefined(int variable) {
	model[variable] = UNDEFINED;
}
