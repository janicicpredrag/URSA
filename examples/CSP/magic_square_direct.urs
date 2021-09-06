nDim=4;
nN=nDim*nDim;
bCorrectSum = (2*nSum*nDim == nN*(nN-1));

bDomainAll=true;
bAllDifferent=true;
for(ni=0;ni<nN;ni++) { 
  bDomain=false;
  for(nj=0;nj<nN;nj++)  {
    bDomain ||= bT[ni][nj];
    for(nk=nj+1;nk<nN;nk++) {
      bDomainAll &&= (!bT[ni][nj] || !bT[ni][nk]);
      bAllDifferent &&= (!bT[nj][ni] || !bT[nk][ni]);
    }
  }
  bDomainAll &&= bDomain;
}

bSum=true;
nSum1=0;
nSum2=0;
for(ni=0;ni<nDim;ni++) { 
  for(nk=0;nk<nN;nk++) { 
    nSum1 += ite(bT[ni+nDim*ni][nk],nk,0);
    nSum2 += ite(bT[(ni+1)*(nDim-1)][nk],nk,0);
  }
  nSum3=0;
  nSum4=0;
  for(nj=0;nj<nDim;nj++) { 
    for(nk=0;nk<nN;nk++) { 
      nSum3 += ite(bT[nDim*ni+nj][nk],nk,0);
      nSum4 += ite(bT[ni+nDim*nj][nk],nk,0);
    }
  }
  bSum &&= (nSum3==nSum);
  bSum &&= (nSum4==nSum);
}
bSum &&= (nSum1==nSum);
bSum &&= (nSum2==nSum);

assert_all(bCorrectSum && bDomainAll && bAllDifferent && bSum); 



