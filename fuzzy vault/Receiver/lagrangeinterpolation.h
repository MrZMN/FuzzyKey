#define polydegree 7

uint16_t key[polydegree+1];

void lagrange(double indata[][2],int pointsnum);
void calculate(double indata[][2],int pointsnum,int havechoice[polydegree+1],double anser[polydegree+1][polydegree+1],int choicenum);



