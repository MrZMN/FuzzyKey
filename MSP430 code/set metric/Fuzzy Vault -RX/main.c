#include <msp430fr5969.h>
#include <stdint.h>

#define iterationtime 100

//mismatch rate = 2%

//#define exectime 1      //the number of executions needed for this construction
//#define keypolydeg 47    //the degree of key polynomial. key length = (keypolydeg+1)*log(n+1) (bit)
//#define s 50            //set size. The bit length of each set element is log(n+1) with base 2. Each element should be different from each other
//#define chaff 205       //number of chaff points added for security
//#define t 2             //error tolerance, the maximum symmetric difference size between TX/RX sets, must be even number
//#define n 255
////The max tolerable mismatch rate = t/2s
////t = s-keypolydeg-1

////mismatch rate = 5%

//#define exectime 1      //the number of executions needed for this construction
//#define keypolydeg 17    //the degree of key polynomial. key length = (keypolydeg+1)*log(n+1) (bit)
//#define s 20            //set size. The bit length of each set element is log(n+1) with base 2. Each element should be different from each other
//#define chaff 235       //number of chaff points added for security
//#define t 2             //error tolerance, the maximum symmetric difference size between TX/RX sets, must be even number
//#define n 255
////The max tolerable mismatch rate = t/2s
////t = s-keypolydeg-1
//
////mismatch rate = 10%

#define exectime 2      //the number of executions needed for this construction
#define keypolydeg 7    //the degree of key polynomial. key length = (keypolydeg+1)*log(n+1) (bit)
#define s 10            //set size. The bit length of each set element is log(n+1) with base 2. Each element should be different from each other
#define chaff 245       //number of chaff points added for security
#define t 2             //error tolerance, the maximum symmetric difference size between TX/RX sets, must be even number
#define n 255
//The max tolerable mismatch rate = t/2s
//t = s-keypolydeg-1

//GF(2^8) tables
//polynomial form
uint8_t alpha_to[n+1] = {1, 2, 4, 8, 16, 32, 64, 128, 113, 226, 181, 27, 54, 108, 216, 193, 243, 151, 95, 190, 13, 26, 52, 104, 208, 209, 211, 215, 223, 207, 239, 175, 47, 94, 188, 9, 18, 36, 72, 144, 81, 162, 53, 106, 212, 217, 195, 247, 159, 79, 158, 77, 154, 69, 138, 101, 202, 229, 187, 7, 14, 28, 56, 112, 224, 177, 19, 38, 76, 152, 65, 130, 117, 234, 165, 59, 118, 236, 169, 35, 70, 140, 105, 210, 213, 219, 199, 255, 143, 111, 222, 205, 235, 167, 63, 126, 252, 137, 99, 198, 253, 139, 103, 206, 237, 171, 39, 78, 156, 73, 146, 85, 170, 37, 74, 148, 89, 178, 21, 42, 84, 168, 33, 66, 132, 121, 242, 149, 91, 182, 29, 58, 116, 232, 161, 51, 102, 204, 233, 163, 55, 110, 220, 201, 227, 183, 31, 62, 124, 248, 129, 115, 230, 189, 11, 22, 44, 88, 176, 17, 34, 68, 136, 97, 194, 245, 155, 71, 142, 109, 218, 197, 251, 135, 127, 254, 141, 107, 214, 221, 203, 231, 191, 15, 30, 60, 120, 240, 145, 83, 166, 61, 122, 244, 153, 67, 134, 125, 250, 133, 123, 246, 157, 75, 150, 93, 186, 5, 10, 20, 40, 80, 160, 49, 98, 196, 249, 131, 119, 238, 173, 43, 86, 172, 41, 82, 164, 57, 114, 228, 185, 3, 6, 12, 24, 48, 96, 192, 241, 147, 87, 174, 45, 90, 180, 25, 50, 100, 200, 225, 179, 23, 46, 92, 184, 0};
//index form
uint8_t index_of[n+1] = {255, 0, 1, 231, 2, 207, 232, 59, 3, 35, 208, 154, 233, 20, 60, 183, 4, 159, 36, 66, 209, 118, 155, 251, 234, 245, 21, 11, 61, 130, 184, 146, 5, 122, 160, 79, 37, 113, 67, 106, 210, 224, 119, 221, 156, 242, 252, 32, 235, 213, 246, 135, 22, 42, 12, 140, 62, 227, 131, 75, 185, 191, 147, 94, 6, 70, 123, 195, 161, 53, 80, 167, 38, 109, 114, 203, 68, 51, 107, 49, 211, 40, 225, 189, 120, 111, 222, 240, 157, 116, 243, 128, 253, 205, 33, 18, 236, 163, 214, 98, 247, 55, 136, 102, 23, 82, 43, 177, 13, 169, 141, 89, 63, 8, 228, 151, 132, 72, 76, 218, 186, 125, 192, 200, 148, 197, 95, 174, 7, 150, 71, 217, 124, 199, 196, 173, 162, 97, 54, 101, 81, 176, 168, 88, 39, 188, 110, 239, 115, 127, 204, 17, 69, 194, 52, 166, 108, 202, 50, 48, 212, 134, 41, 139, 226, 74, 190, 93, 121, 78, 112, 105, 223, 220, 241, 31, 158, 65, 117, 250, 244, 10, 129, 145, 254, 230, 206, 58, 34, 153, 19, 182, 237, 15, 164, 46, 215, 171, 99, 86, 248, 143, 56, 180, 137, 91, 103, 29, 24, 25, 83, 26, 44, 84, 178, 27, 14, 45, 170, 85, 142, 179, 90, 28, 64, 249, 9, 144, 229, 57, 152, 181, 133, 138, 73, 92, 77, 104, 219, 30, 187, 238, 126, 16, 193, 165, 201, 47, 149, 216, 198, 172, 96, 100, 175, 87};

//put modular operation into RAM
#pragma CODE_SECTION(mod, ".run_from_ram_mod")
uint16_t mod(uint16_t num1, uint16_t num2){
    return num1%num2;
}

//check if 'value' is in set[]
uint8_t isinset(uint8_t value, uint8_t set[], uint8_t setsize){
    uint8_t i;
    uint8_t flag = 0;
    i = 0;
    while(i != setsize){
        if(value == set[i]){
            flag = 1;
        }
        i++;
    }
    return flag;
}

//swap the row of a matrix
void exchange_row(uint8_t *matrix[], uint8_t flag, uint8_t row){
    uint8_t i;
    uint8_t *temp;

    i = flag+1;
    while(i != row){
        if(*(matrix[i]+flag) != n){
            temp = matrix[flag];
            matrix[flag] = matrix[i];
            matrix[i] = temp;
        }
        i++;
    }
}

//solve the solutions of an equation set
void solve_equationset(uint8_t solutions[], uint8_t *matrix[], uint8_t row, uint8_t col){
    //input is a (row*col) size matrix 'matrix[]'

    uint8_t i, j;
    uint8_t k;
    uint8_t div;

    //elementary row transformation
    i = 0;
    while(i != row-1){
        if(*(matrix[i] + i) == n){
            exchange_row(matrix, i, row);
        }
        if(*(matrix[i]+i) == n){
            continue;
        }

        j = i + 1;
        while(j != row){
            if(*(matrix[j]+i) == n){
                div = n;
            }else{
                div = mod(*(matrix[j]+i) + (n - *(matrix[i]+i)), n);
            }

            if(div != n){
                k = i;
                while(k != col){
                    if(*(matrix[i]+k) != n){
                        if(*(matrix[j]+k) == n){
                            *(matrix[j]+k) = mod(div + *(matrix[i]+k), n);
                        }else{
                            *(matrix[j]+k) = index_of[(uint8_t)(alpha_to[(uint8_t)(*(matrix[j]+k))] ^ alpha_to[mod(div + *(matrix[i]+k), n)])];
                        }
                    }
                    k++;
                }
            }
            j++;
        }
        i++;
    }
    //the resulted matrix should be a upper triangular matrix

    int8_t l,m;
    //solve the solutions of the equation set based on the upper triangular matrix
    for(l = row-1; l >= 0; l--){
        for(m = col-2; m > l; m--){
            if(*(matrix[(uint8_t)l]+m) != n && solutions[(uint8_t)m] != n){
                if(*(matrix[(uint8_t)l]+col-1) == n){
                    *(matrix[(uint8_t)l]+col-1) = index_of[alpha_to[mod(*(matrix[(uint8_t)l]+m) + solutions[(uint8_t)m], n)]];
                }else{
                    *(matrix[(uint8_t)l]+col-1) = index_of[(uint8_t)(alpha_to[*(matrix[(uint8_t)l]+col-1)] ^ alpha_to[mod(*(matrix[(uint8_t)l]+m) + solutions[(uint8_t)m], n)])];
                }
            }
        }

        if(*(matrix[(uint8_t)l]+col-1) == n){
            solutions[(uint8_t)l] = n;
        }else{
            solutions[(uint8_t)l] = mod(*(matrix[(uint8_t)l]+col-1) + (n - *(matrix[(uint8_t)l]+l)), n);
        }
    }
    //array solutions[] stores the solutions of the equation set
}

//Division between polynomials. lenA, lenB are the lengthes of polyA[] and polyB[] respectively. store the quotient polynomial in poly[]
void poly_division(uint8_t A[], uint8_t B[], uint8_t lenA, uint8_t lenB, uint8_t plow[]){
    uint8_t hA = 0;
    uint8_t hB = lenA - lenB;
    uint8_t i, div, tem = 0, tem1 = 0;

    while(hA <= hB){
        div = mod(A[hA] + (n - B[hB]), n);
        plow[hA] = div;
        tem = hA;

        i = 0;
        while(i != lenB){
            if(B[(uint8_t)(hB+i)] != n){
                if(A[(uint8_t)(hA+i)] == n){
                    A[(uint8_t)(hA+i)] = alpha_to[mod(B[(uint8_t)(hB+i)] + div, n)];
                }else{
                    A[(uint8_t)(hA+i)] = alpha_to[A[(uint8_t)(hA+i)]] ^ alpha_to[mod(B[(uint8_t)(hB+i)] + div, n)];
                }
                A[(uint8_t)(hA+i)] = index_of[A[(uint8_t)(hA+i)]];
            }

            if(A[(uint8_t)(hA+i)] == n){
                tem++;
            }
            if(A[(uint8_t)(hA+i)] != n || tem == lenA-1 || tem == tem1+lenB){
                tem1 = tem;
            }
            i++;
        }

        hA = tem1;
    }
    //the array plow[] stores the quotient polynoial
}

//Berlekamp-Welch algorithm to correct the mismatches on set elements
void BerlekampWelch(uint8_t ps[], uint8_t points[][2], uint8_t key[]){
    uint8_t i, j;
    uint8_t augmentMat[s][s+1];       //the augmented matrix used in B-W algorithm
    uint8_t *aMat[s];                 //a pointer array - used to accelarate computation
    uint8_t solution[s];              //the solutions of augmentMat[][]
    uint8_t A[s - t/2], B[s - t/2];   //Divide solutions[] into two parts as A() and B()

    ////Use Reed-Soloman Decoding (Berlekamp-Welch algorithm) to generate a poly from 's' num of points, succeed if at least (s-t/2) points are legit
    //generate the augmented matrix

    i = 0;
    while(i != s){
        j = 0;
        while(j != s-t/2){
            augmentMat[i][j] = mod(points[i][0] * (s-t/2-1-j), n);
            j++;
        }
        if(points[i][1] == n){
            augmentMat[i][s-1] = n;
            augmentMat[i][s] = n;
        }else{
            augmentMat[i][s-1] = points[i][1];
            augmentMat[i][s] = mod(points[i][0] + points[i][1], n);
        }
        i++;
    }
    //convert the 2D augmented array into 1D pointer array format, to accelerate computation
    i = 0;
    while(i != s){
        aMat[i] = augmentMat[i];
        i++;
    }

    //Use Gussian Elimination to solve the solutions of the augmented matrix
    solve_equationset(solution, aMat, s, s+1);

    //The solutions of the equations consist of 2 polys as A[] and B[]
    i = 0;
    while(i != s - t/2){
        A[i] = solution[i];
        if(i == s - t -1){
            B[i] = 0;
        }else{
            B[i] = solution[(unsigned int)(i + t/2)];
        }
        i++;
    }

    //The key polynomial is A()/B(), if the number of errors is within the error threshold
    i = 0;
    while(i != s-t){
        key[i] = n;
        i++;
    }
    poly_division(A, B, s - t/2, t/2 + 1, key);
}

void matchpoint(uint8_t ps[], uint8_t points[][2], uint8_t matchedpoints[][2]){
    uint8_t i;
    uint8_t matchidx = 0;

    i = 0;
    while(i != s+chaff){
        if(isinset(points[i][0], ps, s)){
            matchedpoints[matchidx][0] = points[i][0];
            matchedpoints[matchidx][1] = points[i][1];
            matchidx++;
        }
        i++;
    }

    if(matchidx != s){
        i = 0;
        while(i != s-matchidx){
            matchedpoints[(unsigned int)(s-i-1)][0] = 0;
            matchedpoints[(unsigned int)(s-i-1)][1] = 0;
            i++;
        }
    }
}

//RX of Fuzzy Vault
int main(){

    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer
    // Configure all port pins as output low to save power
    PAOUT = 0; PBOUT = 0; PJOUT = 0;
    PADIR = 0xFFFF; PBDIR = 0xFFFF; PJDIR = 0xFF;
    PAIFG = 0; PBIFG = 0;
    PM5CTL0 &= ~LOCKLPM5;       // Disable GPIO power-on default high-impedance mode to activate previously configured port settings

    __no_operation();                         // SET A BREAKPOINT HERE
    uint16_t ite = 0;
    uint8_t i;
    while(ite!= iterationtime * exectime){

        //points received from TX
        uint8_t points[s+chaff][2] = {{0, 32}, {1, 88}, {2, 175}, {3, 246}, {4, 94}, {5, 90}, {6, 236}, {7, 31}, {8, 187}, {9, 244}, {10, 179}, {11, 126}, {12, 228}, {13, 213}, {14, 207}, {15, 188}, {16, 186}, {17, 174}, {18, 57}, {19, 51}, {20, 153}, {21, 202}, {22, 253}, {23, 129}, {24, 233}, {25, 11}, {26, 136}, {27, 17}, {28, 88}, {29, 139}, {30, 34}, {31, 150}, {32, 52}, {33, 252}, {34, 144}, {35, 245}, {36, 3}, {37, 77}, {38, 11}, {39, 138}, {40, 141}, {41, 50}, {42, 123}, {43, 217}, {44, 2}, {45, 81}, {46, 19}, {47, 80}, {48, 47}, {49, 225}, {50, 63}, {51, 186}, {52, 181}, {53, 57}, {54, 172}, {55, 74}, {56, 205}, {57, 198}, {58, 25}, {59, 70}, {60, 203}, {61, 151}, {62, 43}, {63, 230}, {64, 231}, {65, 141}, {66, 131}, {67, 34}, {68, 95}, {69, 229}, {70, 54}, {71, 49}, {72, 220}, {73, 230}, {74, 207}, {75, 104}, {76, 141}, {77, 250}, {78, 199}, {79, 61}, {80, 152}, {81, 228}, {82, 156}, {83, 109}, {84, 102}, {85, 7}, {86, 109}, {87, 219}, {88, 99}, {89, 51}, {90, 217}, {91, 67}, {92, 172}, {93, 170}, {94, 179}, {95, 201}, {96, 138}, {97, 72}, {98, 0}, {99, 80}, {100, 130}, {101, 82}, {102, 109}, {103, 227}, {104, 163}, {105, 29}, {106, 106}, {107, 24}, {108, 239}, {109, 85}, {110, 218}, {111, 127}, {112, 108}, {113, 47}, {114, 151}, {115, 134}, {116, 224}, {117, 12}, {118, 205}, {119, 76}, {120, 95}, {121, 78}, {122, 80}, {123, 170}, {124, 195}, {125, 155}, {126, 19}, {127, 12}, {128, 230}, {129, 170}, {130, 189}, {131, 221}, {132, 43}, {133, 147}, {134, 85}, {135, 248}, {136, 183}, {137, 254}, {138, 166}, {139, 43}, {140, 23}, {141, 120}, {142, 31}, {143, 88}, {144, 102}, {145, 179}, {146, 69}, {147, 111}, {148, 28}, {149, 146}, {150, 115}, {151, 119}, {152, 51}, {153, 56}, {154, 120}, {155, 154}, {156, 72}, {157, 240}, {158, 116}, {159, 161}, {160, 178}, {161, 21}, {162, 80}, {163, 76}, {164, 63}, {165, 103}, {166, 245}, {167, 236}, {168, 198}, {169, 119}, {170, 225}, {171, 134}, {172, 238}, {173, 170}, {174, 81}, {175, 240}, {176, 114}, {177, 125}, {178, 70}, {179, 133}, {180, 130}, {181, 176}, {182, 168}, {183, 223}, {184, 189}, {185, 99}, {186, 5}, {187, 27}, {188, 250}, {189, 112}, {190, 60}, {191, 248}, {192, 134}, {193, 247}, {194, 133}, {195, 57}, {196, 203}, {197, 138}, {198, 71}, {199, 23}, {200, 187}, {201, 76}, {202, 229}, {203, 250}, {204, 232}, {205, 188}, {206, 195}, {207, 131}, {208, 209}, {209, 229}, {210, 92}, {211, 78}, {212, 111}, {213, 64}, {214, 83}, {215, 139}, {216, 143}, {217, 133}, {218, 175}, {219, 65}, {220, 182}, {221, 92}, {222, 202}, {223, 142}, {224, 238}, {225, 135}, {226, 35}, {227, 26}, {228, 48}, {229, 139}, {230, 192}, {231, 199}, {232, 247}, {233, 159}, {234, 20}, {235, 1}, {236, 83}, {237, 44}, {238, 195}, {239, 144}, {240, 4}, {241, 17}, {242, 22}, {243, 130}, {244, 60}, {245, 31}, {246, 93}, {247, 8}, {248, 89}, {249, 1}, {250, 32}, {251, 234}, {252, 8}, {253, 173}, {254, 102}};

        //TX's set generated from PS measurements. Each element should be different from each other. We use 1~s to simulate it.
        uint8_t ps[s];

        i = 0;
        while(i != s){
            ps[i] = i+1;
            i++;
        }

        //add some errors (for test only)
        i = 0;
        while(i != t/2){
            ps[i] += 100;
            i++;
        }

        //matched points
        uint8_t matchedpoints[s][2];
        //match the legit points from all received points
        matchpoint(ps, points, matchedpoints);

        //the key
        uint8_t key[keypolydeg+1];
        //correct the miamatches on sets
        BerlekampWelch(ps, matchedpoints, key);

        __no_operation();                         // SET A BREAKPOINT HERE
        ite++;
    }
}
