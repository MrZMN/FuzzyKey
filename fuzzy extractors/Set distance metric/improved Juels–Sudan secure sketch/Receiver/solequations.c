#include "solequations.h"

void guass_elimination(long double *matrix[], int row, int col)
{
int i, j, k;
long double coe;

for(i=0;i<row-1;i++){
  exchange_row(matrix, i, row);
  if(fabsl(*(matrix[i]+i))<0.00001)
    continue;
  for(j=i+1;j<row;j++){
    coe = *(matrix[j]+i) / *(matrix[i]+i);
  for(k=i;k<col;k++)
    *(matrix[j]+k) -= coe * *(matrix[i]+k);
  }
}
}

void exchange_row(long double *matrix[], int flag, int row)
{
 int i;
 long double *temp;

 for(i=flag+1;i<row;i++)
  if(fabsl(*(matrix[flag]+flag))<fabsl(*(matrix[i]+flag))){
   temp = matrix[flag];
   matrix[flag] = matrix[i];
   matrix[i] = temp;
  }
}

void show_solution(long double *matrix[], int row, int col)
{
 int i, j;

 for(i=0;i<row;i++){
  for(j=0;j<col;j++)
   printf("%Lf ", *(matrix[i]+j));
  putchar('\n');
 }
}