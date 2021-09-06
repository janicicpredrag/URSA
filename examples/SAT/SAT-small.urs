b = true; 
b &&= (b1  || b2  || b3 );
b &&= (!b1 || b2  || !b3 );
b &&= (b1  || !b2 || !b3 );

assert_all(b);
