#include "Cub.h"

Cub::Cub()
	: sizeCathetus(5), degreenXY(45), degreenXZ(45), zoom(5)
{
	this->cleanMatrix();

	this->paintCub();
}

Cub::Cub(int sizeCathetus, int degreenXY, int degreenXZ, int zoom)
	: sizeCathetus(sizeCathetus), degreenXY(degreenXY), degreenXZ(degreenXZ),
	zoom(zoom)
{
	this->cleanMatrix();

	this->paintCub();
}

// Отрисовка куба на матрице
void Cub::paintCub()
{

}

// Очистка матрицы
void Cub::cleanMatrix()
{
	for (int i = 0; i < heightDisplay; i++)
		for (int j = 0; j < widthDisplay; j++)
			matrix[i][j] = false;
}
