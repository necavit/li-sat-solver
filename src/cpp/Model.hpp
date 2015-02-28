/*
 * Model.hpp
 *
 *  Created on: Feb 28, 2015
 *      Author: David Martínez Rodríguez
 */

#ifndef _MODEL_HPP_
#define _MODEL_HPP_

#include <vector>
#include "ModelValues.hpp"

class Model {

private:
	std::vector<int> model;

public:
	/**
	 * Builds a new model for the number of variables specified.
	 *
	 * @param numVariables the number of variables this model will hold
	 */
	Model(int numVariables);

	/**
	 * Destroys the model object.
	 */
	~Model();

	/**
	 * Returns the value that the literal has within the current interpretation (model).
	 *
	 * @param literal the literal to be checked
	 * @return the value of the literal with the current model
	 */
	int valueForLiteral(int literal);

	/**
	 * The current interpretation (model) is updated with the appropriate value
	 * that will make the passed literal become true in the clause where it appeared.
	 * The model stack is also increased, pushing the new value on top.
	 *
	 * @param literal the literal that will become true after the model update
	 */
	void setLiteralToTrue(int literal);

	/**
	 * Returns whether or not the variable is undefined in the current model.
	 *
	 * @param variable the variable to be checked
	 * @return true if the variable is undefined; false otherwise
	 */
	bool isVariableUndefined(int variable);

	/**
	 * Sets the variable's value in the current model as undefined.
	 *
	 * @param variable the variable that will become undefeined
	 */
	void setVariableUndefined(int variable);
};

#endif _MODEL_HPP_
