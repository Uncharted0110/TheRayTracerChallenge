#ifndef MATRICES_H
#define MATRICES_H

#include <iostream>
#include <vector>
#include "PointVec.h"


bool MatrixEquality(std::vector<std::vector<float>>& matA, std::vector<std::vector<float>>& matB);
std::vector<std::vector<float>> Matrix4x4_Multiply(std::vector<std::vector<float>> matA, std::vector<std::vector<float>> matB);
Entity Matrix_Entity_Multiply(std::vector<std::vector<float>> mat, Entity e);
void PrintMatrix(std::vector<std::vector<float>> mat);
std::vector<std::vector<float>> Transpose_Matrix(std::vector<std::vector<float>> mat);
std::vector<std::vector<float>> SubMat(std::vector<std::vector<float>>& mat, int row, int col);
float Determinant(std::vector<std::vector<float>>& mat);;
float Minor(std::vector<std::vector<float>>& mat, int row, int col);
float Cofactor(std::vector<std::vector<float>>& mat, int row, int col);
std::vector<std::vector<float>> Invert_Matrix(std::vector<std::vector<float>> mat);
std::vector<std::vector<float>> Translation(float x, float y, float z);
std::vector<std::vector<float>> Scaling(float x, float y, float z);
float DegToRad(float deg);
std::vector<std::vector<float>> Rotation_X(float rad);
std::vector<std::vector<float>> Rotation_Y(float rad);
std::vector<std::vector<float>> Rotation_Z(float rad);
std::vector<std::vector<float>> Shearing(float xy, float xz, float yx, float yz, float zx, float zy);
std::vector<std::vector<float>> operator*(const std::vector<std::vector<float>>& A, const std::vector<std::vector<float>>& B);

#endif 
