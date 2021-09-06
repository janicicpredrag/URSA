/* To be used with numbers represented with vectors of size nDim */
nDim = 8;

bHorizontal = true;
for(ni=0; ni<nDim; ni++)
  bHorizontal &&= ((n[ni] & n[ni]-1)==0) && (n[ni]!=0);

nVertical = 0;
for(ni=0; ni<nDim; ni++) 
  nVertical |= n[ni];
bVertical = (nVertical+1 == 0);

bDiagonal = true;
for(nAi=0; nAi<nDim-1; nAi++) 
  for(nAj=0; nAj<nDim; nAj++) 
    for(nBi=nAi+1; nBi<nDim; nBi++) 
      for(nBj=0; nBj<nDim; nBj++)
        if (nBi-nAi==nBj-nAj || nBi-nAi==nAj-nBj)  
          bDiagonal &&= (((n[nBj]<<(nBi-nAi)) & n[nAj])==0);

assert_all(bHorizontal && bVertical && bDiagonal);
