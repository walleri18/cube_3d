#include "Cub.h"

Cub::Cub()
	: sizeCathetus(5), degreenXY(45), degreenXZ(45), zoom(5)
{
	this->paintCub();
}

Cub::Cub(int sizeCathetus, int degreenXY, int degreenXZ, int zoom)
	: sizeCathetus(sizeCathetus), degreenXY(degreenXY), degreenXZ(degreenXZ),
	zoom(zoom)
{
	this->paintCub();
}


Cub::~Cub()
{
	for (int i = 0; i < height; i++)
		delete[] matrix[i];

	delete[] matrix;
}

// Отрисовка куба на матрице
void Cub::paintCub()
{

}
