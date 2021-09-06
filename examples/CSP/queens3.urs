nDim = 8;

bHorizontal = true;
for(ni=0; ni<nDim; ni++) {
  bOne = false;
  bMoreThanOne = false;
  for(nj=0; nj<nDim; nj++)  {
    bMoreThanOne ||= bOne && b[ni][nj];
    bOne ||= b[ni][nj];
  }
  bHorizontal &&= bOne && !bMoreThanOne;
}

bVertical = true;
for(ni=0; ni<nDim; ni++) {
  bOne = false;
  for(nj=0; nj<nDim; nj++)
    bOne ||= b[nj][ni];
  bVertical &&= bOne;
}

bDiagonal = true;
for(nAi=0; nAi<nDim-1; nAi++)
  for(nAj=0; nAj<nDim; nAj++)
    for(nBi=nAi+1; nBi<nDim; nBi++)
      for(nBj=0; nBj<nDim; nBj++)
        if (nBi-nAi==nBj-nAj || nBi-nAi==nAj-nBj)
          bDiagonal &&= (!b[nAi][nAj] || !b[nBi][nBj]);

assert_all(bHorizontal && bVertical && bDiagonal);

