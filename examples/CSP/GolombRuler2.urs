nM=7;
nL=25;

bRulerEndpoints = num2bool(nRuler & nRuler >> nL & 1);
nMarks=2;
bDistanceDiff=true;

for(ni=1; ni<=nL-1; ni++) {
  nMarks += (nRuler >> ni) & 1;
  n = (nRuler & (nRuler << ni));
  bDistanceDiff &&= (n & (n-1))==0;
}

assert_all(bRulerEndpoints && nMarks==nM && bDistanceDiff);


