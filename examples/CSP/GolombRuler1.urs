nM=8;
minimize(nL,1,100);

nA[0]=0;
nA[nM-1]=nL;

bDomain=true;
for(ni=0;ni+1<nM;ni++)  
  bDomain &&= (nA[ni] < nA[ni+1]);

bDistinctDiff=true;
for(ni=0;ni+1<nM;ni++)  
  for(nj=ni+1;nj<nM;nj++) 
    for(nk=0;nk+1<nM;nk++)  
      for(nl=nk+1;nl<nM;nl++)  
        if(ni!=nk)
           bDistinctDiff &&= (nA[ni]-nA[nj] != nA[nk]-nA[nl]);

/*bSymmBreak = ((nA[1]-nA[0]) < (nA[nM-1]-nA[nM-2]));*/

assert(bDomain && bDistinctDiff); 



