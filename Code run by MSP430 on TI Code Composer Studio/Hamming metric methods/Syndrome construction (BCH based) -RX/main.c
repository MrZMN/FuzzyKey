#include <msp430fr5969.h>
#include <stdint.h>
#include "aes256.h"

#define iterationtime 100   //We test the energy consumption of 100 consecutive executions, then take the average

//BCH code configurations//All settings are in BCH_length_k_t format, where t is error tolerance in bit. The maximum tolerable mismatch rate = t/length

//error tolerance = 2%
#define BCH_50_44_1
#define BCH_100_86_2
#define BCH_200_168_4

//error tolerance = 5%
#define BCH_20_15_1
#define BCH_40_28_2
#define BCH_60_42_3
#define BCH_80_52_4
#define BCH_100_65_5
#define BCH_220_136_11

//error tolerance = 10%
#define BCH_10_6_1
#define BCH_20_10_2
#define BCH_30_15_3
#define BCH_40_16_4
#define BCH_50_23_5
#define BCH_60_27_6
#define BCH_90_34_9
#define BCH_120_43_12
#define BCH_210_70_21

#ifdef BCH_50_44_1

#define exectime 3  //the number of executions needed for this construction
#define hashlen 7   //length of hash input
#define n 63       //block length
#define length 50  //codeword length
#define k 44       //data length
#define t 1
//GF tables
int8_t alpha_to[n+1] = {1, 2, 4, 8, 16, 32, 3, 6, 12, 24, 48, 35, 5, 10, 20, 40, 19, 38, 15, 30, 60, 59, 53, 41, 17, 34, 7, 14, 28, 56, 51, 37, 9, 18, 36, 11, 22, 44, 27, 54, 47, 29, 58, 55, 45, 25, 50, 39, 13, 26, 52, 43, 21, 42, 23, 46, 31, 62, 63, 61, 57, 49, 33, 0};
int8_t index_of[n+1] = {-1, 0, 1, 6, 2, 12, 7, 26, 3, 32, 13, 35, 8, 48, 27, 18, 4, 24, 33, 16, 14, 52, 36, 54, 9, 45, 49, 38, 28, 41, 19, 56, 5, 62, 25, 11, 34, 31, 17, 47, 15, 23, 53, 51, 37, 44, 55, 40, 10, 61, 46, 30, 50, 22, 39, 43, 29, 60, 42, 21, 20, 59, 57, 58};

#elif defined BCH_100_86_2

#define exectime 2  //the number of executions needed for this construction
#define hashlen 13   //length of hash input
#define n 127       //block length
#define length 100  //codeword length
#define k 86       //data length
#define t 2
//GF tables
int8_t alpha_to[n+1] = {1, 2, 4, 8, 16, 32, 64, 3, 6, 12, 24, 48, 96, 67, 5, 10, 20, 40, 80, 35, 70, 15, 30, 60, 120, 115, 101, 73, 17, 34, 68, 11, 22, 44, 88, 51, 102, 79, 29, 58, 116, 107, 85, 41, 82, 39, 78, 31, 62, 124, 123, 117, 105, 81, 33, 66, 7, 14, 28, 56, 112, 99, 69, 9, 18, 36, 72, 19, 38, 76, 27, 54, 108, 91, 53, 106, 87, 45, 90, 55, 110, 95, 61, 122, 119, 109, 89, 49, 98, 71, 13, 26, 52, 104, 83, 37, 74, 23, 46, 92, 59, 118, 111, 93, 57, 114, 103, 77, 25, 50, 100, 75, 21, 42, 84, 43, 86, 47, 94, 63, 126, 127, 125, 121, 113, 97, 65, 0};
int8_t index_of[n+1] = {-1, 0, 1, 7, 2, 14, 8, 56, 3, 63, 15, 31, 9, 90, 57, 21, 4, 28, 64, 67, 16, 112, 32, 97, 10, 108, 91, 70, 58, 38, 22, 47, 5, 54, 29, 19, 65, 95, 68, 45, 17, 43, 113, 115, 33, 77, 98, 117, 11, 87, 109, 35, 92, 74, 71, 79, 59, 104, 39, 100, 23, 82, 48, 119, 6, 126, 55, 13, 30, 62, 20, 89, 66, 27, 96, 111, 69, 107, 46, 37, 18, 53, 44, 94, 114, 42, 116, 76, 34, 86, 78, 73, 99, 103, 118, 81, 12, 125, 88, 61, 110, 26, 36, 106, 93, 52, 75, 41, 72, 85, 80, 102, 60, 124, 105, 25, 40, 51, 101, 84, 24, 123, 83, 50, 49, 122, 120, 121};

#elif defined BCH_200_168_4

#define exectime 1  //the number of executions needed for this construction
#define hashlen 25   //length of hash input
#define n 255       //block length
#define length 200  //codeword length
#define k 168       //data length
#define t 4
//GF tables
int16_t alpha_to[n+1] = {1, 2, 4, 8, 16, 32, 64, 128, 113, 226, 181, 27, 54, 108, 216, 193, 243, 151, 95, 190, 13, 26, 52, 104, 208, 209, 211, 215, 223, 207, 239, 175, 47, 94, 188, 9, 18, 36, 72, 144, 81, 162, 53, 106, 212, 217, 195, 247, 159, 79, 158, 77, 154, 69, 138, 101, 202, 229, 187, 7, 14, 28, 56, 112, 224, 177, 19, 38, 76, 152, 65, 130, 117, 234, 165, 59, 118, 236, 169, 35, 70, 140, 105, 210, 213, 219, 199, 255, 143, 111, 222, 205, 235, 167, 63, 126, 252, 137, 99, 198, 253, 139, 103, 206, 237, 171, 39, 78, 156, 73, 146, 85, 170, 37, 74, 148, 89, 178, 21, 42, 84, 168, 33, 66, 132, 121, 242, 149, 91, 182, 29, 58, 116, 232, 161, 51, 102, 204, 233, 163, 55, 110, 220, 201, 227, 183, 31, 62, 124, 248, 129, 115, 230, 189, 11, 22, 44, 88, 176, 17, 34, 68, 136, 97, 194, 245, 155, 71, 142, 109, 218, 197, 251, 135, 127, 254, 141, 107, 214, 221, 203, 231, 191, 15, 30, 60, 120, 240, 145, 83, 166, 61, 122, 244, 153, 67, 134, 125, 250, 133, 123, 246, 157, 75, 150, 93, 186, 5, 10, 20, 40, 80, 160, 49, 98, 196, 249, 131, 119, 238, 173, 43, 86, 172, 41, 82, 164, 57, 114, 228, 185, 3, 6, 12, 24, 48, 96, 192, 241, 147, 87, 174, 45, 90, 180, 25, 50, 100, 200, 225, 179, 23, 46, 92, 184, 0};
int16_t index_of[n+1] = {-1, 0, 1, 231, 2, 207, 232, 59, 3, 35, 208, 154, 233, 20, 60, 183, 4, 159, 36, 66, 209, 118, 155, 251, 234, 245, 21, 11, 61, 130, 184, 146, 5, 122, 160, 79, 37, 113, 67, 106, 210, 224, 119, 221, 156, 242, 252, 32, 235, 213, 246, 135, 22, 42, 12, 140, 62, 227, 131, 75, 185, 191, 147, 94, 6, 70, 123, 195, 161, 53, 80, 167, 38, 109, 114, 203, 68, 51, 107, 49, 211, 40, 225, 189, 120, 111, 222, 240, 157, 116, 243, 128, 253, 205, 33, 18, 236, 163, 214, 98, 247, 55, 136, 102, 23, 82, 43, 177, 13, 169, 141, 89, 63, 8, 228, 151, 132, 72, 76, 218, 186, 125, 192, 200, 148, 197, 95, 174, 7, 150, 71, 217, 124, 199, 196, 173, 162, 97, 54, 101, 81, 176, 168, 88, 39, 188, 110, 239, 115, 127, 204, 17, 69, 194, 52, 166, 108, 202, 50, 48, 212, 134, 41, 139, 226, 74, 190, 93, 121, 78, 112, 105, 223, 220, 241, 31, 158, 65, 117, 250, 244, 10, 129, 145, 254, 230, 206, 58, 34, 153, 19, 182, 237, 15, 164, 46, 215, 171, 99, 86, 248, 143, 56, 180, 137, 91, 103, 29, 24, 25, 83, 26, 44, 84, 178, 27, 14, 45, 170, 85, 142, 179, 90, 28, 64, 249, 9, 144, 229, 57, 152, 181, 133, 138, 73, 92, 77, 104, 219, 30, 187, 238, 126, 16, 193, 165, 201, 47, 149, 216, 198, 172, 96, 100, 175, 87};

#elif defined BCH_20_15_1

#define exectime 9  //the number of executions needed for this construction
#define hashlen 3   //length of hash input
#define n 31       //block length
#define length 20  //codeword length
#define k 15       //data length
#define t 1
//GF tables
int8_t alpha_to[n+1] = {1, 2, 4, 8, 16, 5, 10, 20, 13, 26, 17, 7, 14, 28, 29, 31, 27, 19, 3, 6, 12, 24, 21, 15, 30, 25, 23, 11, 22, 9, 18, 0};
int8_t index_of[n+1] = {-1, 0, 1, 18, 2, 5, 19, 11, 3, 29, 6, 27, 20, 8, 12, 23, 4, 10, 30, 17, 7, 22, 28, 26, 21, 25, 9, 16, 13, 14, 24, 15};

#elif defined BCH_40_28_2

#define exectime 5  //the number of executions needed for this construction
#define hashlen 5   //length of hash input
#define n 63       //block length
#define length 40  //codeword length
#define k 28       //data length
#define t 2
//GF tables
int8_t alpha_to[n+1] = {1, 2, 4, 8, 16, 32, 3, 6, 12, 24, 48, 35, 5, 10, 20, 40, 19, 38, 15, 30, 60, 59, 53, 41, 17, 34, 7, 14, 28, 56, 51, 37, 9, 18, 36, 11, 22, 44, 27, 54, 47, 29, 58, 55, 45, 25, 50, 39, 13, 26, 52, 43, 21, 42, 23, 46, 31, 62, 63, 61, 57, 49, 33, 0};
int8_t index_of[n+1] = {-1, 0, 1, 6, 2, 12, 7, 26, 3, 32, 13, 35, 8, 48, 27, 18, 4, 24, 33, 16, 14, 52, 36, 54, 9, 45, 49, 38, 28, 41, 19, 56, 5, 62, 25, 11, 34, 31, 17, 47, 15, 23, 53, 51, 37, 44, 55, 40, 10, 61, 46, 30, 50, 22, 39, 43, 29, 60, 42, 21, 20, 59, 57, 58};

#elif defined BCH_60_42_3

#define exectime 4  //the number of executions needed for this construction
#define hashlen 8   //length of hash input
#define n 63       //block length
#define length 60  //codeword length
#define k 42       //data length
#define t 3
//GF tables
int8_t alpha_to[n+1] = {1, 2, 4, 8, 16, 32, 3, 6, 12, 24, 48, 35, 5, 10, 20, 40, 19, 38, 15, 30, 60, 59, 53, 41, 17, 34, 7, 14, 28, 56, 51, 37, 9, 18, 36, 11, 22, 44, 27, 54, 47, 29, 58, 55, 45, 25, 50, 39, 13, 26, 52, 43, 21, 42, 23, 46, 31, 62, 63, 61, 57, 49, 33, 0};
int8_t index_of[n+1] = {-1, 0, 1, 6, 2, 12, 7, 26, 3, 32, 13, 35, 8, 48, 27, 18, 4, 24, 33, 16, 14, 52, 36, 54, 9, 45, 49, 38, 28, 41, 19, 56, 5, 62, 25, 11, 34, 31, 17, 47, 15, 23, 53, 51, 37, 44, 55, 40, 10, 61, 46, 30, 50, 22, 39, 43, 29, 60, 42, 21, 20, 59, 57, 58};

#elif defined BCH_80_52_4

#define exectime 3  //the number of executions needed for this construction
#define hashlen 10   //length of hash input
#define n 127       //block length
#define length 80  //codeword length
#define k 52       //data length
#define t 4
//GF tables
int8_t alpha_to[n+1] = {1, 2, 4, 8, 16, 32, 64, 3, 6, 12, 24, 48, 96, 67, 5, 10, 20, 40, 80, 35, 70, 15, 30, 60, 120, 115, 101, 73, 17, 34, 68, 11, 22, 44, 88, 51, 102, 79, 29, 58, 116, 107, 85, 41, 82, 39, 78, 31, 62, 124, 123, 117, 105, 81, 33, 66, 7, 14, 28, 56, 112, 99, 69, 9, 18, 36, 72, 19, 38, 76, 27, 54, 108, 91, 53, 106, 87, 45, 90, 55, 110, 95, 61, 122, 119, 109, 89, 49, 98, 71, 13, 26, 52, 104, 83, 37, 74, 23, 46, 92, 59, 118, 111, 93, 57, 114, 103, 77, 25, 50, 100, 75, 21, 42, 84, 43, 86, 47, 94, 63, 126, 127, 125, 121, 113, 97, 65, 0};
int8_t index_of[n+1] = {-1, 0, 1, 7, 2, 14, 8, 56, 3, 63, 15, 31, 9, 90, 57, 21, 4, 28, 64, 67, 16, 112, 32, 97, 10, 108, 91, 70, 58, 38, 22, 47, 5, 54, 29, 19, 65, 95, 68, 45, 17, 43, 113, 115, 33, 77, 98, 117, 11, 87, 109, 35, 92, 74, 71, 79, 59, 104, 39, 100, 23, 82, 48, 119, 6, 126, 55, 13, 30, 62, 20, 89, 66, 27, 96, 111, 69, 107, 46, 37, 18, 53, 44, 94, 114, 42, 116, 76, 34, 86, 78, 73, 99, 103, 118, 81, 12, 125, 88, 61, 110, 26, 36, 106, 93, 52, 75, 41, 72, 85, 80, 102, 60, 124, 105, 25, 40, 51, 101, 84, 24, 123, 83, 50, 49, 122, 120, 121};

#elif defined BCH_100_65_5

#define exectime 2  //the number of executions needed for this construction
#define hashlen 13   //length of hash input
#define n 127       //block length
#define length 100  //codeword length
#define k 65       //data length
#define t 5
//GF tables
int8_t alpha_to[n+1] = {1, 2, 4, 8, 16, 32, 64, 3, 6, 12, 24, 48, 96, 67, 5, 10, 20, 40, 80, 35, 70, 15, 30, 60, 120, 115, 101, 73, 17, 34, 68, 11, 22, 44, 88, 51, 102, 79, 29, 58, 116, 107, 85, 41, 82, 39, 78, 31, 62, 124, 123, 117, 105, 81, 33, 66, 7, 14, 28, 56, 112, 99, 69, 9, 18, 36, 72, 19, 38, 76, 27, 54, 108, 91, 53, 106, 87, 45, 90, 55, 110, 95, 61, 122, 119, 109, 89, 49, 98, 71, 13, 26, 52, 104, 83, 37, 74, 23, 46, 92, 59, 118, 111, 93, 57, 114, 103, 77, 25, 50, 100, 75, 21, 42, 84, 43, 86, 47, 94, 63, 126, 127, 125, 121, 113, 97, 65, 0};
int8_t index_of[n+1] = {-1, 0, 1, 7, 2, 14, 8, 56, 3, 63, 15, 31, 9, 90, 57, 21, 4, 28, 64, 67, 16, 112, 32, 97, 10, 108, 91, 70, 58, 38, 22, 47, 5, 54, 29, 19, 65, 95, 68, 45, 17, 43, 113, 115, 33, 77, 98, 117, 11, 87, 109, 35, 92, 74, 71, 79, 59, 104, 39, 100, 23, 82, 48, 119, 6, 126, 55, 13, 30, 62, 20, 89, 66, 27, 96, 111, 69, 107, 46, 37, 18, 53, 44, 94, 114, 42, 116, 76, 34, 86, 78, 73, 99, 103, 118, 81, 12, 125, 88, 61, 110, 26, 36, 106, 93, 52, 75, 41, 72, 85, 80, 102, 60, 124, 105, 25, 40, 51, 101, 84, 24, 123, 83, 50, 49, 122, 120, 121};

#elif defined BCH_220_136_11

#define exectime 1  //the number of executions needed for this construction
#define hashlen 28   //length of hash input
#define n 255       //block length
#define length 220  //codeword length
#define k 136       //data length
#define t 11
//GF tables
int16_t alpha_to[n+1] = {1, 2, 4, 8, 16, 32, 64, 128, 113, 226, 181, 27, 54, 108, 216, 193, 243, 151, 95, 190, 13, 26, 52, 104, 208, 209, 211, 215, 223, 207, 239, 175, 47, 94, 188, 9, 18, 36, 72, 144, 81, 162, 53, 106, 212, 217, 195, 247, 159, 79, 158, 77, 154, 69, 138, 101, 202, 229, 187, 7, 14, 28, 56, 112, 224, 177, 19, 38, 76, 152, 65, 130, 117, 234, 165, 59, 118, 236, 169, 35, 70, 140, 105, 210, 213, 219, 199, 255, 143, 111, 222, 205, 235, 167, 63, 126, 252, 137, 99, 198, 253, 139, 103, 206, 237, 171, 39, 78, 156, 73, 146, 85, 170, 37, 74, 148, 89, 178, 21, 42, 84, 168, 33, 66, 132, 121, 242, 149, 91, 182, 29, 58, 116, 232, 161, 51, 102, 204, 233, 163, 55, 110, 220, 201, 227, 183, 31, 62, 124, 248, 129, 115, 230, 189, 11, 22, 44, 88, 176, 17, 34, 68, 136, 97, 194, 245, 155, 71, 142, 109, 218, 197, 251, 135, 127, 254, 141, 107, 214, 221, 203, 231, 191, 15, 30, 60, 120, 240, 145, 83, 166, 61, 122, 244, 153, 67, 134, 125, 250, 133, 123, 246, 157, 75, 150, 93, 186, 5, 10, 20, 40, 80, 160, 49, 98, 196, 249, 131, 119, 238, 173, 43, 86, 172, 41, 82, 164, 57, 114, 228, 185, 3, 6, 12, 24, 48, 96, 192, 241, 147, 87, 174, 45, 90, 180, 25, 50, 100, 200, 225, 179, 23, 46, 92, 184, 0};
int16_t index_of[n+1] = {-1, 0, 1, 231, 2, 207, 232, 59, 3, 35, 208, 154, 233, 20, 60, 183, 4, 159, 36, 66, 209, 118, 155, 251, 234, 245, 21, 11, 61, 130, 184, 146, 5, 122, 160, 79, 37, 113, 67, 106, 210, 224, 119, 221, 156, 242, 252, 32, 235, 213, 246, 135, 22, 42, 12, 140, 62, 227, 131, 75, 185, 191, 147, 94, 6, 70, 123, 195, 161, 53, 80, 167, 38, 109, 114, 203, 68, 51, 107, 49, 211, 40, 225, 189, 120, 111, 222, 240, 157, 116, 243, 128, 253, 205, 33, 18, 236, 163, 214, 98, 247, 55, 136, 102, 23, 82, 43, 177, 13, 169, 141, 89, 63, 8, 228, 151, 132, 72, 76, 218, 186, 125, 192, 200, 148, 197, 95, 174, 7, 150, 71, 217, 124, 199, 196, 173, 162, 97, 54, 101, 81, 176, 168, 88, 39, 188, 110, 239, 115, 127, 204, 17, 69, 194, 52, 166, 108, 202, 50, 48, 212, 134, 41, 139, 226, 74, 190, 93, 121, 78, 112, 105, 223, 220, 241, 31, 158, 65, 117, 250, 244, 10, 129, 145, 254, 230, 206, 58, 34, 153, 19, 182, 237, 15, 164, 46, 215, 171, 99, 86, 248, 143, 56, 180, 137, 91, 103, 29, 24, 25, 83, 26, 44, 84, 178, 27, 14, 45, 170, 85, 142, 179, 90, 28, 64, 249, 9, 144, 229, 57, 152, 181, 133, 138, 73, 92, 77, 104, 219, 30, 187, 238, 126, 16, 193, 165, 201, 47, 149, 216, 198, 172, 96, 100, 175, 87};

#elif defined BCH_10_6_1

#define exectime 22  //the number of executions needed for this construction
#define hashlen 2   //length of hash input
#define n 15       //block length
#define length 10  //codeword length
#define k 6       //data length
#define t 1
//GF tables
int8_t alpha_to[n+1] = {1, 2, 4, 8, 3, 6, 12, 11, 5, 10, 7, 14, 15, 13, 9, 0};
int8_t index_of[n+1] = {-1, 0, 1, 4, 2, 8, 5, 10, 3, 14, 9, 7, 6, 13, 11, 12};

#elif defined BCH_20_10_2

#define exectime 13  //the number of executions needed for this construction
#define hashlen 3   //length of hash input
#define n 31       //block length
#define length 20  //codeword length
#define k 10       //data length
#define t 2
//GF tables
int8_t alpha_to[n+1] = {1, 2, 4, 8, 16, 5, 10, 20, 13, 26, 17, 7, 14, 28, 29, 31, 27, 19, 3, 6, 12, 24, 21, 15, 30, 25, 23, 11, 22, 9, 18, 0};
int8_t index_of[n+1] = {-1, 0, 1, 18, 2, 5, 19, 11, 3, 29, 6, 27, 20, 8, 12, 23, 4, 10, 30, 17, 7, 22, 28, 26, 21, 25, 9, 16, 13, 14, 24, 15};

#elif defined BCH_30_15_3

#define exectime 9  //the number of executions needed for this construction
#define hashlen 4   //length of hash input
#define n 31       //block length
#define length 30  //codeword length
#define k 15       //data length
#define t 3
//GF tables
int8_t alpha_to[n+1] = {1, 2, 4, 8, 16, 5, 10, 20, 13, 26, 17, 7, 14, 28, 29, 31, 27, 19, 3, 6, 12, 24, 21, 15, 30, 25, 23, 11, 22, 9, 18, 0};
int8_t index_of[n+1] = {-1, 0, 1, 18, 2, 5, 19, 11, 3, 29, 6, 27, 20, 8, 12, 23, 4, 10, 30, 17, 7, 22, 28, 26, 21, 25, 9, 16, 13, 14, 24, 15};

#elif defined BCH_40_16_4

#define exectime 8  //the number of executions needed for this construction
#define hashlen 5   //length of hash input
#define n 63       //block length
#define length 40  //codeword length
#define k 16       //data length
#define t 4
//GF tables
int8_t alpha_to[n+1] = {1, 2, 4, 8, 16, 32, 3, 6, 12, 24, 48, 35, 5, 10, 20, 40, 19, 38, 15, 30, 60, 59, 53, 41, 17, 34, 7, 14, 28, 56, 51, 37, 9, 18, 36, 11, 22, 44, 27, 54, 47, 29, 58, 55, 45, 25, 50, 39, 13, 26, 52, 43, 21, 42, 23, 46, 31, 62, 63, 61, 57, 49, 33, 0};
int8_t index_of[n+1] = {-1, 0, 1, 6, 2, 12, 7, 26, 3, 32, 13, 35, 8, 48, 27, 18, 4, 24, 33, 16, 14, 52, 36, 54, 9, 45, 49, 38, 28, 41, 19, 56, 5, 62, 25, 11, 34, 31, 17, 47, 15, 23, 53, 51, 37, 44, 55, 40, 10, 61, 46, 30, 50, 22, 39, 43, 29, 60, 42, 21, 20, 59, 57, 58};

#elif defined BCH_50_23_5

#define exectime 6  //the number of executions needed for this construction
#define hashlen 7   //length of hash input
#define n 63       //block length
#define length 50  //codeword length
#define k 23       //data length
#define t 5
//GF tables
int8_t alpha_to[n+1] = {1, 2, 4, 8, 16, 32, 3, 6, 12, 24, 48, 35, 5, 10, 20, 40, 19, 38, 15, 30, 60, 59, 53, 41, 17, 34, 7, 14, 28, 56, 51, 37, 9, 18, 36, 11, 22, 44, 27, 54, 47, 29, 58, 55, 45, 25, 50, 39, 13, 26, 52, 43, 21, 42, 23, 46, 31, 62, 63, 61, 57, 49, 33, 0};
int8_t index_of[n+1] = {-1, 0, 1, 6, 2, 12, 7, 26, 3, 32, 13, 35, 8, 48, 27, 18, 4, 24, 33, 16, 14, 52, 36, 54, 9, 45, 49, 38, 28, 41, 19, 56, 5, 62, 25, 11, 34, 31, 17, 47, 15, 23, 53, 51, 37, 44, 55, 40, 10, 61, 46, 30, 50, 22, 39, 43, 29, 60, 42, 21, 20, 59, 57, 58};

#elif defined BCH_60_27_6

#define exectime 5  //the number of executions needed for this construction
#define hashlen 8   //length of hash input
#define n 63       //block length
#define length 60  //codeword length
#define k 27       //data length
#define t 6
//GF tables
int8_t alpha_to[n+1] = {1, 2, 4, 8, 16, 32, 3, 6, 12, 24, 48, 35, 5, 10, 20, 40, 19, 38, 15, 30, 60, 59, 53, 41, 17, 34, 7, 14, 28, 56, 51, 37, 9, 18, 36, 11, 22, 44, 27, 54, 47, 29, 58, 55, 45, 25, 50, 39, 13, 26, 52, 43, 21, 42, 23, 46, 31, 62, 63, 61, 57, 49, 33, 0};
int8_t index_of[n+1] = {-1, 0, 1, 6, 2, 12, 7, 26, 3, 32, 13, 35, 8, 48, 27, 18, 4, 24, 33, 16, 14, 52, 36, 54, 9, 45, 49, 38, 28, 41, 19, 56, 5, 62, 25, 11, 34, 31, 17, 47, 15, 23, 53, 51, 37, 44, 55, 40, 10, 61, 46, 30, 50, 22, 39, 43, 29, 60, 42, 21, 20, 59, 57, 58};

#elif defined BCH_90_34_9

#define exectime 4  //the number of executions needed for this construction
#define hashlen 12   //length of hash input
#define n 127       //block length
#define length 90  //codeword length
#define k 34       //data length
#define t 9
//GF tables
int8_t alpha_to[n+1] = {1, 2, 4, 8, 16, 32, 64, 3, 6, 12, 24, 48, 96, 67, 5, 10, 20, 40, 80, 35, 70, 15, 30, 60, 120, 115, 101, 73, 17, 34, 68, 11, 22, 44, 88, 51, 102, 79, 29, 58, 116, 107, 85, 41, 82, 39, 78, 31, 62, 124, 123, 117, 105, 81, 33, 66, 7, 14, 28, 56, 112, 99, 69, 9, 18, 36, 72, 19, 38, 76, 27, 54, 108, 91, 53, 106, 87, 45, 90, 55, 110, 95, 61, 122, 119, 109, 89, 49, 98, 71, 13, 26, 52, 104, 83, 37, 74, 23, 46, 92, 59, 118, 111, 93, 57, 114, 103, 77, 25, 50, 100, 75, 21, 42, 84, 43, 86, 47, 94, 63, 126, 127, 125, 121, 113, 97, 65, 0};
int8_t index_of[n+1] = {-1, 0, 1, 7, 2, 14, 8, 56, 3, 63, 15, 31, 9, 90, 57, 21, 4, 28, 64, 67, 16, 112, 32, 97, 10, 108, 91, 70, 58, 38, 22, 47, 5, 54, 29, 19, 65, 95, 68, 45, 17, 43, 113, 115, 33, 77, 98, 117, 11, 87, 109, 35, 92, 74, 71, 79, 59, 104, 39, 100, 23, 82, 48, 119, 6, 126, 55, 13, 30, 62, 20, 89, 66, 27, 96, 111, 69, 107, 46, 37, 18, 53, 44, 94, 114, 42, 116, 76, 34, 86, 78, 73, 99, 103, 118, 81, 12, 125, 88, 61, 110, 26, 36, 106, 93, 52, 75, 41, 72, 85, 80, 102, 60, 124, 105, 25, 40, 51, 101, 84, 24, 123, 83, 50, 49, 122, 120, 121};

#elif defined BCH_120_43_12

#define exectime 3  //the number of executions needed for this construction
#define hashlen 15   //length of hash input
#define n 127       //block length
#define length 120  //codeword length
#define k 43       //data length
#define t 12
//GF tables
int8_t alpha_to[n+1] = {1, 2, 4, 8, 16, 32, 64, 3, 6, 12, 24, 48, 96, 67, 5, 10, 20, 40, 80, 35, 70, 15, 30, 60, 120, 115, 101, 73, 17, 34, 68, 11, 22, 44, 88, 51, 102, 79, 29, 58, 116, 107, 85, 41, 82, 39, 78, 31, 62, 124, 123, 117, 105, 81, 33, 66, 7, 14, 28, 56, 112, 99, 69, 9, 18, 36, 72, 19, 38, 76, 27, 54, 108, 91, 53, 106, 87, 45, 90, 55, 110, 95, 61, 122, 119, 109, 89, 49, 98, 71, 13, 26, 52, 104, 83, 37, 74, 23, 46, 92, 59, 118, 111, 93, 57, 114, 103, 77, 25, 50, 100, 75, 21, 42, 84, 43, 86, 47, 94, 63, 126, 127, 125, 121, 113, 97, 65, 0};
int8_t index_of[n+1] = {-1, 0, 1, 7, 2, 14, 8, 56, 3, 63, 15, 31, 9, 90, 57, 21, 4, 28, 64, 67, 16, 112, 32, 97, 10, 108, 91, 70, 58, 38, 22, 47, 5, 54, 29, 19, 65, 95, 68, 45, 17, 43, 113, 115, 33, 77, 98, 117, 11, 87, 109, 35, 92, 74, 71, 79, 59, 104, 39, 100, 23, 82, 48, 119, 6, 126, 55, 13, 30, 62, 20, 89, 66, 27, 96, 111, 69, 107, 46, 37, 18, 53, 44, 94, 114, 42, 116, 76, 34, 86, 78, 73, 99, 103, 118, 81, 12, 125, 88, 61, 110, 26, 36, 106, 93, 52, 75, 41, 72, 85, 80, 102, 60, 124, 105, 25, 40, 51, 101, 84, 24, 123, 83, 50, 49, 122, 120, 121};

#elif defined BCH_210_70_21

#define exectime 2  //the number of executions needed for this construction
#define hashlen 27   //length of hash input
#define n 255       //block length
#define length 210  //codeword length
#define k 70       //data length
#define t 21
//GF tables
int16_t alpha_to[n+1] = {1, 2, 4, 8, 16, 32, 64, 128, 113, 226, 181, 27, 54, 108, 216, 193, 243, 151, 95, 190, 13, 26, 52, 104, 208, 209, 211, 215, 223, 207, 239, 175, 47, 94, 188, 9, 18, 36, 72, 144, 81, 162, 53, 106, 212, 217, 195, 247, 159, 79, 158, 77, 154, 69, 138, 101, 202, 229, 187, 7, 14, 28, 56, 112, 224, 177, 19, 38, 76, 152, 65, 130, 117, 234, 165, 59, 118, 236, 169, 35, 70, 140, 105, 210, 213, 219, 199, 255, 143, 111, 222, 205, 235, 167, 63, 126, 252, 137, 99, 198, 253, 139, 103, 206, 237, 171, 39, 78, 156, 73, 146, 85, 170, 37, 74, 148, 89, 178, 21, 42, 84, 168, 33, 66, 132, 121, 242, 149, 91, 182, 29, 58, 116, 232, 161, 51, 102, 204, 233, 163, 55, 110, 220, 201, 227, 183, 31, 62, 124, 248, 129, 115, 230, 189, 11, 22, 44, 88, 176, 17, 34, 68, 136, 97, 194, 245, 155, 71, 142, 109, 218, 197, 251, 135, 127, 254, 141, 107, 214, 221, 203, 231, 191, 15, 30, 60, 120, 240, 145, 83, 166, 61, 122, 244, 153, 67, 134, 125, 250, 133, 123, 246, 157, 75, 150, 93, 186, 5, 10, 20, 40, 80, 160, 49, 98, 196, 249, 131, 119, 238, 173, 43, 86, 172, 41, 82, 164, 57, 114, 228, 185, 3, 6, 12, 24, 48, 96, 192, 241, 147, 87, 174, 45, 90, 180, 25, 50, 100, 200, 225, 179, 23, 46, 92, 184, 0};
int16_t index_of[n+1] = {-1, 0, 1, 231, 2, 207, 232, 59, 3, 35, 208, 154, 233, 20, 60, 183, 4, 159, 36, 66, 209, 118, 155, 251, 234, 245, 21, 11, 61, 130, 184, 146, 5, 122, 160, 79, 37, 113, 67, 106, 210, 224, 119, 221, 156, 242, 252, 32, 235, 213, 246, 135, 22, 42, 12, 140, 62, 227, 131, 75, 185, 191, 147, 94, 6, 70, 123, 195, 161, 53, 80, 167, 38, 109, 114, 203, 68, 51, 107, 49, 211, 40, 225, 189, 120, 111, 222, 240, 157, 116, 243, 128, 253, 205, 33, 18, 236, 163, 214, 98, 247, 55, 136, 102, 23, 82, 43, 177, 13, 169, 141, 89, 63, 8, 228, 151, 132, 72, 76, 218, 186, 125, 192, 200, 148, 197, 95, 174, 7, 150, 71, 217, 124, 199, 196, 173, 162, 97, 54, 101, 81, 176, 168, 88, 39, 188, 110, 239, 115, 127, 204, 17, 69, 194, 52, 166, 108, 202, 50, 48, 212, 134, 41, 139, 226, 74, 190, 93, 121, 78, 112, 105, 223, 220, 241, 31, 158, 65, 117, 250, 244, 10, 129, 145, 254, 230, 206, 58, 34, 153, 19, 182, 237, 15, 164, 46, 215, 171, 99, 86, 248, 143, 56, 180, 137, 91, 103, 29, 24, 25, 83, 26, 44, 84, 178, 27, 14, 45, 170, 85, 142, 179, 90, 28, 64, 249, 9, 144, 229, 57, 152, 181, 133, 138, 73, 92, 77, 104, 219, 30, 187, 238, 126, 16, 193, 165, 201, 47, 149, 216, 198, 172, 96, 100, 175, 87};

#endif

//put modular operation into RAM
#pragma CODE_SECTION(mod, ".run_from_ram_mod")
uint16_t mod(uint16_t num1, uint16_t num2){
    return num1%num2;
}

//Calculate the syndrome of BCH codeword
void getsyndrome(uint8_t codeword[], int16_t s[]){

    uint8_t i, j;

    i = 1;
    while(i != 2*t+1){
        s[i] = 0;
        j = 0;
        while(j != length){
            s[i] ^= alpha_to[mod(i * j, n)] & (-codeword[j]);
            j++;
        }
        i++;
    }
}

//Correct the error using BCH decoding 
//This function partially refers to Simon Rockliff's implementation, cf. http://www.eccpage.com/rs.c
//We acknowledge Simon's contribution by stating his name here, as required.
void correcterr(uint8_t codeword[], int16_t s[]){

    uint16_t i, j, u, q;
    uint8_t count = 0;
    int16_t elp[2*t+2][t+1], du[2*t], l[2*t+1], u_l[2*t], reg[2*t];
    uint8_t loc[t];

    //Step 1: convert the syndrome into index form
    i = 1;
    while(i != 2*t+1){
        s[i] = index_of[(uint8_t)(s[i])];
        i++;
    }

    //Step 2: calculate the error location polynomial
    du[0] = 0;
    du[1] = s[1];
    elp[0][0] = 0;
    elp[1][0] = 1;

    i = 1;
    while(i != 2*t){
        elp[0][i] = -1;
        elp[1][i] = 0;
        i++;
    }

    l[0] = 0;
    l[1] = 0;
    u_l[0] = -1;
    u_l[1] = 0;

    u = 0;

    //calculate the error location poly
    do {
        u++;
        if(du[u] == -1){
            l[u + 1] = l[u];
            i = 0;
            while(i != l[u]+1){
                elp[u + 1][i] = elp[u][i];
                elp[u][i] = index_of[(uint8_t)(elp[u][i])];
                i++;
            }
        }else{
            q = u - 1;
            while ((du[q] == -1) && (q > 0))
                q--;
            if(q > 0) {
                j = q;
                do{
                    j--;
                    if ((du[j] != -1) && (u_l[q] < u_l[j]))
                        q = j;
                }while (j > 0);
            }

            if(l[u] > l[q] + u - q){
                l[u + 1] = l[u];
            }
            else{
                l[u + 1] = l[q] + u - q;
            }

            i = 0;
            while(i != 2*t){
                elp[u + 1][i] = 0;
                i++;
            }

            i = 0;
            while(i != l[q]+1){
                if(elp[q][i] != -1){
                    elp[u + 1][i + u - q] = alpha_to[mod(du[u] + n - du[q] + elp[q][i], n)];
                }
                i++;
            }

            i = 0;
            while(i != l[u]+1){
                elp[u + 1][i] ^= elp[u][i];
                elp[u][i] = index_of[elp[u][i]];
                i++;
            }

        }
        u_l[u + 1] = u - l[u + 1];

        if (u < 2*t) {
            if (s[u + 1] != -1){
                du[(uint8_t)(u + 1)] = alpha_to[(uint8_t)(s[u + 1])];
            }else{
                du[u + 1] = 0;
            }

            i = 1;
            while(i != l[u + 1]+1){
                if ((s[u + 1 - i] != -1) && (elp[u + 1][i] != 0)){
                    du[u + 1] ^= alpha_to[mod(s[u + 1 - i] + index_of[(uint8_t)(elp[u + 1][i])], n)];
                }
                i++;
            }

            du[u + 1] = index_of[(uint8_t)(du[u + 1])];
        }
    }while ((u < 2*t) && (l[u + 1] <= t));

    u++;

    //if the errors are under error tolerance interval
    if(l[u] <= t){
        i = 0;
        while(i != l[u]+1){
            elp[u][i] = index_of[(uint8_t)(elp[u][i])];
            i++;
        }

        //Step 3: Chien search - find roots of the error location polynomial
        i = 1;
        while(i != l[u]+1){
            reg[i] = elp[u][i];
            i++;
        }

        count = 0;

        i = 1;
        while(i != n+1){
            q = 1;
            j = 1;
            while(j != l[u]+1){
                if (reg[j] != -1) {
                    reg[j] = mod(reg[j] + j, n);
                    q ^= alpha_to[(uint8_t)(reg[j])];
                }
                j++;
            }
            if(!q){
                loc[count] = n - i;
                count++;
            }
            i++;
        }

        //Step 4: correct the malformed codeword
        if(count == l[u]){
            i = 0;
            while(i != l[u]){
                codeword[loc[i]] ^= 1;
                i++;
            }
        }
    }
}

//Hirose
void hirose(uint8_t hash[32], uint8_t data[], uint8_t datalength){

    uint8_t i, j;
    uint8_t H[16], G[16], key[32], AEScipher[16];

    i = 0;
    while(i != 16){
        H[i] = 0;
        G[i] = 0;
        i++;
    }

    i = 0;
    while(i != 1){      //number of invocations of the Hirose compression function. The input length of each invocation is 16 bytes
        //update the 256-bit key
        j = 0;
        while(j != 16){
            key[j] = H[j];
            if(j+i*16 < datalength){
                key[(uint8_t)(j+16)] = data[(uint8_t)(j+i*16)];
            }else{
                key[(uint8_t)(j+16)] = 0;
            }
            j++;
        }

        AES256_setCipherKey(AES256_BASE, key, AES256_KEYLENGTH_256BIT);

        //Encrypt
        j = 0;
        while(j != 16){
            H[j] = G[j] ^ 0xFF;
            j++;
        }
        AES256_encryptData(AES256_BASE, H, AEScipher);
        //update H
        j = 0;
        while(j != 16){
            H[j] ^= AEScipher[j];
            j++;
        }

        //Encrypt
        AES256_encryptData(AES256_BASE, G, AEScipher);
        //update G
        j = 0;
        while(j != 16){
            G[j] ^= AEScipher[j];
            j++;
        }
        i++;
    }

    //conbine the arrays to get the result
    i = 0;
    while(i != 16){
        hash[i] = H[i];
        hash[(uint8_t)(i+16)] = G[i];
        i++;
    }
}

int main(){

    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer
    // Configure all port pins as output low to save power
    PAOUT = 0; PBOUT = 0; PJOUT = 0;
    PADIR = 0xFFFF; PBDIR = 0xFFFF; PJDIR = 0xFF;
    PAIFG = 0; PBIFG = 0;
    PM5CTL0 &= ~LOCKLPM5;       // Disable GPIO power-on default high-impedance mode to activate previously configured port settings

    
    uint16_t ite = 0;
    uint8_t i;

    //Extracted PS bit string (uniformly distributed random). Assume it's composed of all 1s.
    uint8_t ps[length];
    i = 0;
    while(i != length){
        ps[i] = 1;
        i++;
    }

    //add some mismatches to the PS bit string (for test only)
    i = 0;
    while(i != t){
        ps[i] ^= 1;
        i++;
    }

    __no_operation();                         // SET A BREAKPOINT HERE
    while(ite!= iterationtime * exectime){

        //Received from TX.
        int16_t securesketchTX[2*t+1];      //the received message contains the last 2*t elements only. The first element is 0 by default

        //Generated by RX
        int16_t securesketchRX[2*t+1];
        getsyndrome(ps, securesketchRX);

        //calculate the xor of two syndromes
        i = 1;
        while(i != 2*t+1){
            securesketchRX[i] ^= securesketchTX[i];
            i++;
        }

        //correct the errors
        correcterr(ps, securesketchRX);

        //Strong extractor
        uint8_t j;
        uint8_t hashinput[hashlen];       //input of the hash. array length is l, where 8 * l >= length
        uint8_t hash[32];           //output of the hash

        //for BCH code based methods, we gather each bit together into bytes as the input of hash
        i = 0;
        while(i != hashlen){
            hashinput[i] = 0;
            j = 0;
            while(j != 8){
                if(i*8+j < length){
                    hashinput[i] ^= ps[(uint8_t)(i*8+j)] << (7-j);
                }
                j++;
            }
            i++;
        }

        //hash
        hirose(hash, hashinput, hashlen);

        ite++;
    }
}
