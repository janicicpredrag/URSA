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

#include <string.h>
#include <GroundNumber.hpp>
#include <Boolean.hpp>

extern unsigned int iAbstractNumberLength;

  GroundNumber::GroundNumber() { 
    m_nLen = (iAbstractNumberLength+7)/8; 
    m_pDigits = new unsigned char[m_nLen];
    for(unsigned int i=0;i<m_nLen;i++) 
      m_pDigits[i] = 0;
  }  
  

  GroundNumber::GroundNumber(unsigned int n) { 
    m_nLen = (iAbstractNumberLength+7)/8;  
    m_pDigits = new unsigned char[m_nLen];
    for(unsigned int i=0;i<sizeof(unsigned int) && i<m_nLen;i++) 
      m_pDigits[i] = n >> (8*i);
    for(unsigned int i=sizeof(unsigned int);i<m_nLen;i++) 
      m_pDigits[i] = 0;
    TruncateToSize();  
}   

  GroundNumber::GroundNumber(const GroundNumber& n) {
    m_nLen = n.m_nLen;  
    m_pDigits = new unsigned char[m_nLen];
    for(unsigned int i=0;i<m_nLen;i++) 
      m_pDigits[i] = n.m_pDigits[i];
    TruncateToSize();  
  }    

  GroundNumber::~GroundNumber() {  delete [] m_pDigits; }

  GroundNumber& GroundNumber::operator = (GroundNumber n) { 
  for(unsigned int i=0;i<m_nLen;i++) 
      m_pDigits[i] = n.m_pDigits[i];
    return *this;
  }

  GroundNumber& GroundNumber::operator = (unsigned int n) { 
    for(unsigned int i=0;i<sizeof(unsigned int) && i<m_nLen;i++) { 
      m_pDigits[i] = (n >> (8*i));
    }
    TruncateToSize();   
    return *this;
  }


  GroundNumber& GroundNumber::operator = (char* s) { 
    unsigned int i;
    for(i=0;i<m_nLen;i++) 
      m_pDigits[i] = 0;

    // hex constant
    if(s[0]=='0' && s[1]=='x') { 
      unsigned int l=strlen(s);
      for( i=l-1 ; i>1 ; i--) { 
         if ( (l-i) % 2 )
           m_pDigits[(l-i-1)/2]  = isdigit(s[i]) ? s[i]-'0' : toupper(s[i])-'A'+10;
         else
           m_pDigits[(l-i-1)/2] |= 16*(isdigit(s[i]) ? s[i]-'0' : toupper(s[i])-'A'+10);
      }
    }

    // bin constant
    else if(s[0]=='0' && s[1]=='b') { 
      unsigned int l=strlen(s);
      for( i=l-1 ; i>1 ; i--)  
         m_pDigits[(l-i-1)/8] |= (s[i]-'0') << ((l-i-1) % 8);
    }

    // dec constant
    else  {
      GroundNumber tmp1, tmp2;
      *this = (unsigned int)0;
      tmp2 = (unsigned int)10;
      unsigned int l=strlen(s);
      for(i=0;i<l;i++) { 
         *this = *this * tmp2;
         tmp1 = (unsigned int)(s[i]-'0');
         *this = *this + tmp1;
      }    
    }

    TruncateToSize();   
    return *this;
  }


	  
  GroundNumber GroundNumber::negate() const {  
     GroundNumber m(*this);
     for(unsigned int i=0;i<m_nLen;i++) 
        m.m_pDigits[i] = (unsigned int)0-m_pDigits[i];
     return m; 
  } 


  GroundNumber GroundNumber::bitnegate() const {  
     GroundNumber m(*this);
     for(unsigned int i=0;i<m_nLen;i++) 
        m.m_pDigits[i] = ~m_pDigits[i];
     return m;
  } 


  GroundNumber GroundNumber::operator << (const GroundNumber &n) { 
    GroundNumber m(*this);              
    //assert(n.IsGroundNumber());
    unsigned int shift = n.GetGroundValueUnsigned(); 
    unsigned char carry, carry_next;
    for(unsigned int i=0;i<shift;i++)  {
      carry=0;
      for(unsigned int j=0;j<m_nLen;j++) { 
        carry_next = (m.m_pDigits[j]>>7);
        m.m_pDigits[j] <<= 1;
        m.m_pDigits[j] |= carry;
        carry=carry_next;
      }
    }
    m.TruncateToSize();
    return m;                           
  }


  GroundNumber GroundNumber::operator >> (const GroundNumber &n) { 
    GroundNumber m(*this);                           
    //assert(n.IsGroundNumber());

    unsigned int shift = n.GetGroundValueUnsigned();
    m.TruncateToSize();
    unsigned char carry, carry_next;
    for(unsigned int i=0;i<shift;i++)  {
      carry=0;
      for(int j=m_nLen-1;j>=0;j--) { 
        carry_next = (m.m_pDigits[j] & 1);
        m.m_pDigits[j] >>= 1;
        m.m_pDigits[j] |= (carry << 7);
        carry = carry_next;
      }
    }
    return m;                           
  }

 
  GroundNumber GroundNumber::sgn() const {
    GroundNumber m;
    m.m_pDigits[0]=0;
    for(unsigned int i=0;i<m_nLen;i++) 
       m.m_pDigits[0] = m.m_pDigits[0] || (m_pDigits[i]!=0);
     return m;
  }


  bool GroundNumber::Bool() const {                           
     for(unsigned int i=0;i<m_nLen;i++) 
        if (m_pDigits[i]!=0)
           return true;
     return false;
  }

  bool GroundNumber::Bit(unsigned int i) const {
     return (m_pDigits[i/8]>>(i%8)) & 1;  
  }


  void GroundNumber::PrintValue(char base) const {  
    switch(base) {
      case('b'): // binary 
        cout << "0b";
        for(int i=iAbstractNumberLength-1;i>=0;i--) 
          cout << (char)(Bit(i) ? '1' : '0');
        break;

      default:   // decimal 
        // print value in the decimal form if it can fit into unsigned int, otherwise print in the hex form
        if(m_nLen<=sizeof(unsigned int)) {
          cout << GetGroundValueUnsigned();
          break;
        }
       
      case('x'): // hex
        cout << "0x";
        for(int i=m_nLen-1;i>=0;i--) { 
          cout << (char)((m_pDigits[i]>>4) >=10 ? 'A'+(m_pDigits[i]>>4)-10 : '0'+(m_pDigits[i]>>4));
          cout << (char)((m_pDigits[i]&15) >=10 ? 'A'+(m_pDigits[i]&15)-10 : '0'+(m_pDigits[i]&15)); 
        }
        break;
       
    }
    cout << endl << endl;
  }


  void GroundNumber::list() const {  
      for(int i=m_nLen-1;i>=0;i--) { 
        cout << (int)m_pDigits[i] << " ";
      }
    cout << endl;
  }


  void GroundNumber::TruncateToSize() {
      int mask=0;
      unsigned int l = (!(iAbstractNumberLength%8) ? 8 : iAbstractNumberLength%8); 
      for(unsigned int i=0;i<l;i++)
        mask = ((mask<<1) | 1);
      m_pDigits[m_nLen-1] &= mask;
  }


GroundNumber GroundNumber::operator + (const GroundNumber &a) { 
  GroundNumber m=0;                           
  unsigned int carryn, carry=0;
  for(unsigned int i=0;i<m_nLen;i++)  {
    carryn = ((m_pDigits[i]+a.m_pDigits[i]+carry>=256) ? 1 : 0); 
    m.m_pDigits[i] = m_pDigits[i]+a.m_pDigits[i]+carry;
    carry=carryn;
  }    
  m.TruncateToSize();
  return m;
}  


GroundNumber GroundNumber::operator * (const GroundNumber &a) { 
  GroundNumber m=0;                           
  unsigned int i,j,carry=0, carryn=0;
  for(i=0;i<m_nLen;i++)  {
    for(j=0;j<m_nLen;j++)  {
      carryn = m.m_pDigits[j+i]+m_pDigits[i]*a.m_pDigits[j]+carry;
      carryn = (carryn>=256 ? carryn/256 : 0);      
      m.m_pDigits[j+i] += m_pDigits[i]*a.m_pDigits[j]+carry;
      carry=carryn;
    }
    m.m_pDigits[j+i+1] += carry; 	 
  }    
  m.TruncateToSize();
  return m;
}  


GroundNumber GroundNumber::operator - (const GroundNumber &a) { 
  GroundNumber m;                           
  unsigned int carryn, carry=0;
  for(unsigned int i=0;i<m_nLen;i++)  {
    carryn = (m_pDigits[i]<a.m_pDigits[i]+carry) ? 1 : 0; 
    m.m_pDigits[i] = m_pDigits[i]-a.m_pDigits[i]-carry;
    carry=carryn;
  }
  m.TruncateToSize();
  return m;
}  


GroundNumber GroundNumber::operator ^ (const GroundNumber &a) { 
  GroundNumber m;                           
  for(unsigned int i=0;i<m_nLen;i++)  
    m.m_pDigits[i] = m_pDigits[i]^a.m_pDigits[i];
  return m;
}  


GroundNumber GroundNumber::operator | (const GroundNumber &a) { 
  GroundNumber m;                           
  for(unsigned int i=0;i<m_nLen;i++)  
    m.m_pDigits[i] = m_pDigits[i]|a.m_pDigits[i];
  return m;
}  


GroundNumber GroundNumber::operator & (const GroundNumber &a) { 
  GroundNumber m;                           
  for(unsigned int i=0;i<m_nLen;i++)  
    m.m_pDigits[i] = m_pDigits[i]&a.m_pDigits[i];
  return m;
}  


bool GroundNumber::operator < (const GroundNumber &a) { 
   for(int i=m_nLen-1;i>=0;i--)  { 
      if (m_pDigits[i]>a.m_pDigits[i]) 
         return false;
      if (m_pDigits[i]<a.m_pDigits[i]) 
         return true;
   }
   return false;
}


bool GroundNumber::operator > (const GroundNumber &a) { 
   return !(*this<=a);
}


bool GroundNumber::operator <= (const GroundNumber &a) { 
   for(int i=m_nLen-1;i>=0;i--)  { 
      if (m_pDigits[i]>a.m_pDigits[i]) 
         return false;
      if (m_pDigits[i]<a.m_pDigits[i]) 
         return true;
   }
   return true;
}

bool GroundNumber::operator >= (const GroundNumber &a) { 
   return !(*this<a);
}

bool GroundNumber::operator == (const GroundNumber &a) { 
   for(int i=m_nLen-1;i>=0;i--)  
      if (m_pDigits[i]!=a.m_pDigits[i]) 
         return false;
   return true;
}


bool GroundNumber::operator != (const GroundNumber &a) { 
   for(int i=m_nLen-1;i>=0;i--)  
      if (m_pDigits[i]!=a.m_pDigits[i]) 
         return true;
   return false;
}

unsigned int GroundNumber::GetGroundValueUnsigned() const { 
   unsigned int s=0;
   unsigned int l;
   l = (sizeof(unsigned int)>m_nLen ? m_nLen : sizeof(unsigned int)); 
   for(int i=l-1;i>=0;i--)  {
     s = s*256+m_pDigits[i]; 
   }
   return s;	
}
  
