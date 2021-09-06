nDim = 8;

bHorizontal = true;
for(nx=0; nx<nDim; nx++) {
  bOne = false;
  bMoreThanOne = false;
  for(ny=0; ny<nDim; ny++)  {
    bMoreThanOne ||= bOne && b[nx][ny];
    bOne ||= b[nx][ny];
  }
  bHorizontal &&= bOne && !bMoreThanOne;
}

bVertical = true; 
for(ny=0; ny<nDim; ny++) {
  bOne = false;
  for(nx=0; nx<nDim; nx++)
    bOne ||= b[nx][ny];
  bVertical &&= bOne;
}

bDiagonal = true;
for(nAx=0; nAx<nDim-1; nAx++)
  for(nBx=nAx+1; nBx<nDim; nBx++)  {
    for(nAy=0; nBx-nAx+nAy<nDim; nAy++)  {
      nBy=nBx-nAx+nAy;
      bDiagonal &&= (!b[nAx][nAy] || !b[nBx][nBy]);
    }
    for(nAy=nBx-nAx; nAy<nDim; nAy++)  {
      nBy=nAy-(nBx-nAx);
      bDiagonal &&= (!b[nAx][nAy] || !b[nBx][nBy]);
    }
  }

assert_all(bHorizontal && bVertical && bDiagonal);


