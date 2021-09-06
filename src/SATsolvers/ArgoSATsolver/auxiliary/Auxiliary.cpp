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
#include "Auxiliary.hpp"
#include <sstream>
#include <iostream>

std::string toString(int i) {
  std::ostringstream stream;
  stream << i;
  return stream.str();
}

std::string toString(unsigned i) {
  std::ostringstream stream;
  stream << i;
  return stream.str();
}

std::string toString(unsigned short i) {
  std::ostringstream stream;
  stream << i;
  return stream.str();
}

std::string toString(double d) {
  std::ostringstream stream;
  stream << d;
  return stream.str();
}

bool isInt(const std::string& str) {
  size_t i = 0;
  if (str[i] == '+' || str[i] == '-')
    i++;
  while(isdigit(str[i]))
    i++;
  return str[i] == '\0';
}

bool isFloat(const std::string& str) {
  size_t i = 0;
  if (str[i] == '+' || str[i] == '-')
    i++;

  while(isdigit(str[i]))
    i++;

  if (str[i] == '\0')
    return true;
  if (str[i] != '.')
    return false;

  i++;

  while(isdigit(str[i]))
    i++;

  return str[i] == '\0';
}


//generates a psuedo-random float from [0, 1)
float randFloat() { 
  return rand()/(float(RAND_MAX)+1); 
}

//generates a psuedo-random int from [0, n)
int randInt(int n) {
  return (int)(randFloat() * n);
}

//generates a pseudo-random bool
bool randBool() {
  return rand() > RAND_MAX / 2;
}
