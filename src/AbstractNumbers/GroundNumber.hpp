/************************************************************************************
URSA -- Copyright (c) 2010, Predrag Janicic

This file is part of URSA
 
URSA is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.
 
URSA is WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
**************************************************************************************/

#ifndef __GROUND_NUMBER_H
#define __GROUND_NUMBER_H


class Boolean;

class GroundNumber { 
friend class Boolean;

public:
  GroundNumber();  
  GroundNumber(unsigned int n);
  GroundNumber(const GroundNumber& n);
  ~GroundNumber();

  GroundNumber& operator = (GroundNumber n);
  GroundNumber& operator = (unsigned int n); 
  GroundNumber& operator = (char* s);
  GroundNumber negate() const;
  GroundNumber bitnegate() const;

  GroundNumber operator << (const GroundNumber &n);
  GroundNumber operator >> (const GroundNumber &n);
  GroundNumber operator *  (const GroundNumber &n);
  GroundNumber operator +  (const GroundNumber &n);
  GroundNumber operator -  (const GroundNumber &n);
  GroundNumber operator ^  (const GroundNumber &n);
  GroundNumber operator |  (const GroundNumber &n);
  GroundNumber operator &  (const GroundNumber &n);
  bool operator <  (const GroundNumber &n);
  bool operator >  (const GroundNumber &n);
  bool operator <= (const GroundNumber &n);
  bool operator >= (const GroundNumber &n);  
  bool operator == (const GroundNumber &n);
  bool operator != (const GroundNumber &n);

  bool Bit(unsigned int i) const;
  GroundNumber sgn() const;
  bool Bool() const;
  void PrintValue(char base) const;
  void list() const;  
  void TruncateToSize();

  unsigned char* GetGroundValue() const { return m_pDigits; }
  unsigned int GetGroundValueUnsigned() const;

private: 
  unsigned int m_nLen;
  unsigned char* m_pDigits;
};


#endif
