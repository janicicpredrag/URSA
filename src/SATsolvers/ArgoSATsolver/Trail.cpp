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
#include "Trail.hpp"
#include "Auxiliary.hpp"

namespace ArgoSat {
std::string Trail::toString() const {
  std::string result = "";
  size_t level = 0;
  for (size_t i = 0; i < _literals.size(); i++) {
    if (level < _levels.size() && _levels[level] == i) {
      result += "|"; 
      level++;
    }
    result += Literals::toString(_literals[i]) + " ";
  }
  return result;
}

const unsigned Trail::UNKNOWN = (unsigned)(-1);

}//namespace ArgoSat
