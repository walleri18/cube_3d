#include "Cub.h"

Cub::Cub()
	: sizeCathetus(5), degreenXY(45), degreenXZ(45), zoom(5)
{
	this->createMatrix();

	this->cleanMatrix();

	this->paintCub();
}

Cub::Cub(int sizeCathetus, int degreenXY, int degreenXZ, int zoom)
	: sizeCathetus(sizeCathetus), degreenXY(degreenXY), degreenXZ(degreenXZ),
	zoom(zoom)
{
	this->createMatrix();

	this->cleanMatrix();

	this->paintCub();
}


Cub::~Cub()
{
	for (int i = 0; i < height; i++)
		delete[] matrix[i];

	delete[] matrix;
}

// Создание матрицы
void Cub::createMatrix()
{
	matrix = new bool*[height];

	for (int i = 0; i < height; i++)
		matrix[i] = new bool[width];
}

// Отрисовка куба на матрице
void Cub::paintCub()
{

}

// Очистка матрицы
void Cub::cleanMatrix()
{
	for (int i = 0; i < width; i++)
		for (int j = 0; j < height; j++)
			matrix[i][j] = false;
}
