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
#ifndef __ACTIVITIES_H__
#define __ACTIVITIES_H__

#define MAX_ACTIVITY (1e20)

#include "Variable.hpp"

namespace ArgoSat {
class Activities {
public:
  Activities(double bumpAmount, double decayFactor)
    : _bumpAmount(bumpAmount), _decayFactor(decayFactor) {
  }

  void init(int numVars) {
        _activities.resize(numVars);
        _activities.assign(numVars, 0.0);
  }

  void bump(Variable variable) {
    _activities[variable] += _bumpAmount;
    if (_activities[variable] > MAX_ACTIVITY)
      rescaleActivities();
  }

  void decay() {
    _bumpAmount *= _decayFactor;
  }

  double getActivity(Variable variable) const {
     return _activities[variable];
  }

  struct Comparator {
    Comparator(const Activities* activities)
          : _activities(activities) {
    }
    bool operator() (Variable x, Variable y) {
       return _activities->getActivity(x) > _activities->getActivity(y);
    }
    void print(Variable x) {
      printf("%2s %.3f", Variables::toString(x).c_str(), 
	     _activities->getActivity(x));
      
    }
    const Activities* _activities;
  };

private:
  void rescaleActivities() {
    for (Variable var = 0; var < _activities.size(); var++)
           _activities[var] *= 1.0/MAX_ACTIVITY;
    _bumpAmount *= 1.0/MAX_ACTIVITY;
  }

  std::vector<double> _activities;

  double _bumpAmount;
  double _decayFactor;
};

}//namespace ArgoSat

#endif
