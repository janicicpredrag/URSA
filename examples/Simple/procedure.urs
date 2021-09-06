procedure power(npowernk,nk) {
  np=npowernk;  
  for(ni=1;ni<nk;ni=ni+1)
     npowernk = npowernk*np;
} 

nk=2;
nxpowernk=nx;
nypowernk=ny;
nzpowernk=nz;

call power(nxpowernk,nk);
call power(nypowernk,nk);
call power(nzpowernk,nk);

assert_all(nxpowernk+nypowernk==nzpowernk);

