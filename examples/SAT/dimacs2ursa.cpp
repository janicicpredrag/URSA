#include <string>
#include <iostream>
#include <string.h>

using namespace std;

main() 
{
  string s;
  int v,N,L;

  do 
    getline(cin,s);
  while(s[0]=='c');
  
  cout << "b = true;" << endl; 

  bool bFirst=true;
  while(1) {
    if (!(cin >> v))
      break;

    if (v==0) {
      cout << ");" << endl;  
      bFirst=true;
    }
    else {
      if(v>0) 
        s = "b";
      else {
        s = "!b";
        v = -v;
      }

      if (bFirst)
        cout << "b &&= (" << s << v;
      else 
        cout << " || " << s << v;
      bFirst=false;
    }
  }
 
  cout << "assert(b);" << endl << endl;
 
}
