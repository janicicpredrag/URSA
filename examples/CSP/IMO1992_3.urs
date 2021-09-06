/*
Given nine points in space, no four of which are coplanar, find the minimal
natural number n such that for any coloring with red or blue of n edges
drawn between these nine points there always exists a triangle having all
edges of the same color.
*/

maximize(n,1,36);
nPoints=9;

nNumberOfEdges=0; bTwoColors=true; bNoMonochromaticTriangle=true;
for(ni=1;ni<=nPoints;ni++) 
  for(nj=ni+1;nj<=nPoints;nj++)  {
    nNumberOfEdges += ite(nE[ni][nj]==0,0,1);
    bTwoColors &&= nE[ni][nj]<3;
    for(nk=nj+1;nk<=nPoints;nk++) 
      bNoMonochromaticTriangle &&= (nE[ni][nj]!=nE[nj][nk] || nE[ni][nj]!=nE[ni][nk] || nE[ni][nj]==0);
  }
 
assert(nNumberOfEdges==n && bTwoColors && bNoMonochromaticTriangle);



