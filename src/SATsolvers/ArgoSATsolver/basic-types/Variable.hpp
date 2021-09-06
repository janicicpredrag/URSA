/******************************************************************************
 * Copyright (C) 2007-2009. Filip Maric, Predrag Janicic
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * This program is inspired by MiniSat solver (C) Een, Sorensson 2003-2006.
 ******************************************************************************/
#ifndef __VARIABLE_H__
#define __VARIABLE_H__

#include "ExtendedBoolean.hpp"
#include <vector>
#include <sstream>
#include <fstream>
#include <iostream>

namespace ArgoSat {
typedef unsigned UNSIGNED;
typedef UNSIGNED Variable;

class Variables {
public:
  Variables() :
    _num_vars(0) {
  }

  void setNumberOfVariables(size_t n) {
	for (size_t i = 0; i < n; i++)
	  newVariable();
  }

  Variable newVariable() {
    Variable variable = _num_vars++;
    _isDecisionVariable.push_back(true);
    return variable;
  }

  static std::string toString(Variable variable);

  void clear() {
	_num_vars = 0;
  }

  size_t size() const {
	return _num_vars;
  }

  Variable random() const;

  bool isDecisionVariable(Variable variable) const {
      return _isDecisionVariable[variable];
  }

  void setDecisionVariablesFromFile(const std::string& fileName);

private:
  std::vector<bool> _isDecisionVariable;
  size_t _num_vars;
};
}//namespace ArgoSat
#endif
