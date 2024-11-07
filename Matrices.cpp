#include <iostream>
#include <vector>
#include "Matrices.h"

#define EPSILON 0.00001

bool MatrixEquality(std::vector<std::vector<float>>& matA, std::vector<std::vector<float>>& matB) {
	for (int i = 0; i < matA.size(); i++) {
		if (matA[i].size() - matB[i].size()) {
			std::cout << "Matrices sizes aren't equal" << std::endl;
			return false;
		}
		for (int j = 0; j < matA.size(); j++) {
			if (abs(matA[i][j] - matB[i][j]) > EPSILON)
				return false;
		}
	}

	return true;
}

void PrintMatrix(std::vector<std::vector<float>> matrix) {
	for (int i = 0; i < matrix.size(); i++) {
		for (int j = 0; j < matrix.size(); j++)
			std::cout << matrix[i][j] << " ";
		std::cout << std::endl;
	}
}

std::vector<std::vector<float>> operator*(const std::vector<std::vector<float>>& A, const std::vector<std::vector<float>>& B) {
	// Initialize the result matrix with zeroes
	std::vector<std::vector<float>> result(4, std::vector<float>(4, 0.0f));

	// Perform matrix multiplication
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			for (int k = 0; k < 4; k++) {
				result[i][j] += A[i][k] * B[k][j];
			}
		}
	}

	return result;
}

std::vector<std::vector<float>> Matrix4x4_Multiply(std::vector<std::vector<float>> matA, std::vector<std::vector<float>> matB) {
	std::vector<std::vector<float>> matC(matA.size(), std::vector<float>(matA.size()));

	for (int i = 0; i < 4; i++) {
		if (matA[i].size() != matB[i].size()) {
			std::cout << "Matrices sizes aren't equal" << std::endl;
			return matC;
		}
		for (int j = 0; j < 4; j++) {
			matC[i][j] = matA[i][0] * matB[0][j] +
				matA[i][1] * matB[1][j] +
				matA[i][2] * matB[2][j] +
				matA[i][3] * matB[3][j];
		}
	}

	return matC;
}

Entity Matrix_Entity_Multiply(std::vector<std::vector<float>> mat, Entity e) {
	return Entity(
		(mat[0][0] * e.x + mat[0][1] * e.y + mat[0][2] * e.z + mat[0][3] * e.w),
		(mat[1][0] * e.x + mat[1][1] * e.y + mat[1][2] * e.z + mat[1][3] * e.w),
		(mat[2][0] * e.x + mat[2][1] * e.y + mat[2][2] * e.z + mat[2][3] * e.w),
		(mat[3][0] * e.x + mat[3][1] * e.y + mat[3][2] * e.z + mat[3][3] * e.w)
	);
}


std::vector<std::vector<float>> Transpose_Matrix(std::vector<std::vector<float>> mat) {
	std::vector<std::vector<float>> trans(mat.size(), std::vector<float>(mat.size()));

	for (int i = 0; i < mat.size(); i++) {
		for (int j = 0; j < mat.size(); j++) {
			trans[i][j] = mat[j][i];
		}
	}

	return trans;
}

std::vector<std::vector<float>> SubMat(std::vector<std::vector<float>>& mat, int row, int col) {
	std::vector<std::vector<float>> res(mat.size() - 1, std::vector<float>(mat.size() - 1));

	for (int a = 0, i = 0; a < mat.size(); a++) {
		if (a == row)
			continue;
		for (int b = 0, j = 0; b < mat.size(); b++) {
			if (b == col)
				continue;
			res[i][j] = mat[a][b];
			j++;
		}
		i++;
	}

	return res;
}

float Determinant(std::vector<std::vector<float>>& mat) {
	if (mat.size() == 2)
		return mat[0][0] * mat[1][1] - mat[0][1] * mat[1][0];

	float det = 0;
	for (int i = 0; i < mat.size(); i++)
		det += mat[0][i] * Cofactor(mat, 0, i);

	return det;
}

float Minor(std::vector<std::vector<float>>& mat, int row, int col) {
	std::vector<std::vector<float>> res = SubMat(mat, row, col);
	return Determinant(res);
}

float Cofactor(std::vector<std::vector<float>>& mat, int row, int col) {
	float minor = Minor(mat, row, col);
	if (minor == 0)
		return minor;
	return ((row + col) % 2 == 0 ? 1.0f : -1.0f) * minor;
}

std::vector<std::vector<float>> Invert_Matrix(std::vector<std::vector<float>> mat) {
	std::vector<std::vector<float>> inv(mat.size(), std::vector<float>(mat.size()));

	float det = Determinant(mat);
	if (det == 0) {
		std::cout << "Matrix is non-invertible" << std::endl;
		return inv;
	}

	for (int i = 0; i < mat.size(); i++)
		for (int j = 0; j < mat.size(); j++) {
			inv[i][j] = Cofactor(mat, j, i) / det;
			if (abs(inv[i][j] - 0) < EPSILON)
				inv[i][j] = 0;
		}
	return inv;
}

std::vector<std::vector<float>> Translation(float x, float y, float z) {
	return std::vector<std::vector<float>>{
		{1.0f, 0.0f, 0.0f, x},
		{0.0f, 1.0f, 0.0f, y},
		{0.0f, 0.0f, 1.0f, z},
		{0.0f, 0.0f, 0.0f, 1.0f}
	};
}

std::vector<std::vector<float>> Scaling(float x, float y, float z) {
	return std::vector<std::vector<float>>{
		{x, 0.0f, 0.0f, 0.0f},
		{ 0.0f, y, 0.0f, 0.0f},
		{ 0.0f, 0.0f, z, 0.0f},
		{ 0.0f, 0.0f, 0.0f, 1.0f }
	};
}

float DegToRad(float deg) {
	return deg * 3.14159f / 180.0f;
}

std::vector<std::vector<float>> Rotation_X(float rad) {
	return std::vector<std::vector<float>>{
		{1.0f, 0.0f, 0.0f, 0.0f},
		{ 0.0f, cos(rad), -sin(rad), 0.0f},
		{ 0.0f, sin(rad), cos(rad), 0.0f},
		{ 0.0f, 0.0f, 0.0f, 1.0f }
	};
}

std::vector<std::vector<float>> Rotation_Y(float rad) {
	return std::vector<std::vector<float>>{
		{cos(rad), 0.0f, sin(rad), 0.0f},
		{ 0.0f, 1.0f, 0.0f, 0.0f },
		{ -sin(rad), 0.0f, cos(rad), 0.0f},
		{ 0.0f, 0.0f, 0.0f, 1.0f }
	};
}

std::vector<std::vector<float>> Rotation_Z(float rad) {
	return std::vector<std::vector<float>>{
		{cos(rad), -sin(rad), 0.0f, 0.0f},
		{ sin(rad), cos(rad), 0.0f, 0.0f},
		{ 0.0f, 0.0f, 1.0f, 0.0f },
		{ 0.0f, 0.0f, 0.0f, 1.0f }
	};
}

std::vector<std::vector<float>> Shearing(float xy, float xz, float yx, float yz, float zx, float zy) {
	return std::vector<std::vector<float>>{
		{1.0f, xy, xz, 0.0f},
		{ yx, 1.0f, yz, 0.0f },
		{ zx, zy, 1.0f, 0.0f },
		{ 0.0f, 0.0f, 0.0f, 1.0f }
	};
}