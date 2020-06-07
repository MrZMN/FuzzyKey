#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

#include "lagrangeinterpolation.h"

// indata[][]: points; pointsnum: num of points
void lagrange(long double indata[][2],int pointsnum){

    int i, j, choicenum=0;
    
    long double anser[polydegree+1][polydegree+1];
    int havechoice[polydegree+1];
    long double finalanser[polydegree+1];
    long double L0[polydegree+1];

    // Initialisation
    for(i=0;i<polydegree+1;i++){
        for(j=0;j<polydegree+1;j++){
            anser[i][j]=0;
	    }
    }
    for(i=0;i<polydegree+1;i++){
	   havechoice[i]=0;
       L0[i]=1;
       finalanser[i]=0;
    }

    // calculate the numerator
    calculate(indata,pointsnum,havechoice,anser,choicenum);

    // calculate the denominator
    for(i=0;i<pointsnum;i++){
        for(j=0;j<pointsnum;j++){
            if(i!=j){
                L0[i] *= (indata[i][0]-indata[j][0]);	
            }
        }
    }

    // construct the poly f(x)
    for(i=0;i<pointsnum;i++){
        for(j=0;j<pointsnum;j++){
            finalanser[i] += ((anser[j][i]*indata[j][1])/L0[j]);
        }
        key[i] = finalanser[i];
    }

}

void calculate(long double indata[][2],int pointsnum,int havechoice[polydegree+1],long double anser[polydegree+1][polydegree+1],int choicenum){
    int flag=1;
    int i=0,j=0;
    int count=0;

    if(choicenum!=pointsnum+1){

        if(choicenum==0 || choicenum==1){
            i=0;
        }else{
            i=havechoice[choicenum-1]+1;
        }

        for(;i<pointsnum;i++){
            
            for(j=0;j<choicenum;j++){ 
                if(i==havechoice[j]){
                    flag=0;
                }
            }
            
            if(flag!=0){
                havechoice[choicenum]=i;
                count=choicenum+1;
                calculate(indata,pointsnum,havechoice,anser,count);
                if(choicenum!=0){
                    int k;
                    long double multitemp=1;
                    for(k=1;k<count;k++){
                        multitemp=multitemp*indata[havechoice[k]][0]*-1;
                    }
                    anser[havechoice[0]][choicenum]+=multitemp;
                }else{
                    anser[havechoice[0]][0]=1;
                }
            }
            
	    flag=1;	
        }
    }
}
