na = nx;
nb = ny;

na ^= nb;
nb ^= na;
na ^= nb;

assert((na!=ny) || (nb!=nx));


