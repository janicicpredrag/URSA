nDim=4;
nN=nDim*nDim;
bCorrectSum = (2*nSum*nDim == nN*(nN-1));

bDomain=true;
bDistinct=true;
for(ni=0;ni<nN;ni++) { 
  bDomain &&= (nT[ni]<nN);
  for(nj=ni+1;nj<nN;nj++) 
    bDistinct &&= (nT[ni] != nT[nj]);
}


bSum=true;
nSum1=0;
nSum2=0;
for(ni=0;ni<nDim;ni++) { 
  nSum1 += nT[ni+nDim*ni];
  nSum2 += nT[(ni+1)*(nDim-1)];
  nSum3=0;
  nSum4=0;
  for(nj=0;nj<nDim;nj++) { 
    nSum3 += nT[nDim*ni+nj];
    nSum4 += nT[ni+nDim*nj];          
  }
  bSum &&= (nSum3==nSum);
  bSum &&= (nSum4==nSum);
}
bSum &&= (nSum1==nSum);
bSum &&= (nSum2==nSum);

bNoSymmetry = (nT[0]<nT[nDim-1]) && (nT[0]<nT[nDim-1]) && (nT[0]<nT[nDim*(nDim-1)]) && (nT[0]<nT[nDim*nDim-1]) && (nT[nDim-1]<nT[nDim*(nDim-1)]);

assert_all(bCorrectSum && bDomain && bDistinct && bSum && bNoSymmetry); 


