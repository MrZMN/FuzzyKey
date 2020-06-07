#define polydegree 5

long double key[polydegree+1];

void lagrange(long double indata[][2],int pointsnum);
void calculate(long double indata[][2],int pointsnum,int havechoice[polydegree+1],long double anser[polydegree+1][polydegree+1],int choicenum);



