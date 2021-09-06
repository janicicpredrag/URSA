nN=6;
minimize(nK,1,nN);

bDomain = true;
for(ni=0;ni+1<nN;ni++) 
  for(nj=ni+1;nj<nN;nj++) 
    bDomain &&= (nT[ni][nj]<nK);

bMonochromatic=false;
for(ni=0;ni+2<nN;ni++) 
  for(nj=ni+1;nj+1<nN;nj++) 
    for(nk=nj+1;nk<nN;nk++) 
      bMonochromatic ||= (nT[ni][nj]==nT[ni][nk] && nT[ni][nj]==nT[nj][nk]);

assert(bDomain && !bMonochromatic);



