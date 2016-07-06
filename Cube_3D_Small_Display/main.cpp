#define _USE_MATH_DEFINES

#include <conio.h>
#include <stdio.h>
#include <math.h>
#include <iostream>

enum MessageID : int
{
	ESC,
	LEFT,
	RIGHT,
	UP,
	DOWN,
	ROTX,
	ROTY,
	ROTZ,
	PLUS,
	MINUS
};

namespace Turov_Vitaly 
{
	// Количество вершин куба
	const int size = 8;

	// Исходные координаты куба
	const int pix[size][3] = { { -25,-25,-25 },{ 25,-25,-25 },{ 25,25,-25 },{ -25,25,-25 },
	{ -25,-25,25 },{ 25,-25,25 },{ 25,25,25 },{ -25,25,25 } };

	// Преобразованные координаты куба
	int newpix[size][3];

	/*
	sX, sY - спроецированные координаты
	X, Y, Z - длина сторон спроецированных на координатные линии
	*/
	int X, Y, Z, sX, sY;

	// углы поворота по X,Y,Z в градусах
	int RotX = 0, RotY = 0, RotZ = 0;

	// сдвиг начала координат
	int ShiftX = 325, ShiftY = 225;

	// масштабирование
	double Scale = 2.0;
}

// Отрисовка линии в матрице
void line(int x, int y, int sX, int sY)
{

}

/*
Вычисление фрактальной геометрической (изометрической) проекции
трехмерных точек на двумерную плоскость
*/
void Perspect()
{
	Turov_Vitaly::sX = Turov_Vitaly::X + Turov_Vitaly::Z / 2;
	Turov_Vitaly::sY = Turov_Vitaly::Y - Turov_Vitaly::Z / 2;
}

// 3-D преобразования в 2D
void Compute()
{
	// Считаем направляющие синусы и косинусы
	double sinx = sin((Turov_Vitaly::RotX * M_PI) / 180.0);
	double siny = sin((Turov_Vitaly::RotY * M_PI) / 180.0);
	double sinz = sin((Turov_Vitaly::RotZ * M_PI) / 180.0);
	double cosx = cos((Turov_Vitaly::RotX * M_PI) / 180.0);
	double cosy = cos((Turov_Vitaly::RotY * M_PI) / 180.0);
	double cosz = cos((Turov_Vitaly::RotZ * M_PI) / 180.0);

	// есть старые координаты точек ( в 3D ) и нужно получить координаты точек (в 2D)
	double x, y, z, x1, y1, z1;

	for (int i = 0; i < Turov_Vitaly::size; i++)
	{

		//Исходное направление
		x = Turov_Vitaly::pix[i][0];
		y = Turov_Vitaly::pix[i][1];
		z = Turov_Vitaly::pix[i][2];

		// вокруг Z
		x1 = x * cosz + y * sinz;
		y1 = -x * sinz + y * cosz;
		z1 = z;

		// вокруг X
		x = x1;
		y = y1 * cosx + z1 * sinx;
		z = -y1 * sinx + z1 * cosx;

		// вокруг Y
		x1 = x * cosy - z * siny;
		y1 = y;
		z1 = x * siny + z * cosy;

		// Масштабирование
		Turov_Vitaly::newpix[i][0] = x1 * Turov_Vitaly::Scale;
		Turov_Vitaly::newpix[i][1] = y1 * Turov_Vitaly::Scale;
		Turov_Vitaly::newpix[i][2] = z1 * Turov_Vitaly::Scale;

		// сдвиг по осям X и Y
		Turov_Vitaly::newpix[i][0] += Turov_Vitaly::ShiftX;
		Turov_Vitaly::newpix[i][1] += Turov_Vitaly::ShiftY;
		Turov_Vitaly::newpix[i][2] += 25;
	}
}

// Рисование кубика
void DrawPix()
{
	int x, y;

	// отрисовка передней грани
	for (int i = 0, j = 0; i < 4; i++)
	{
		// исходная точка
		Turov_Vitaly::X = Turov_Vitaly::newpix[i][0];   // Исходная точка
		Turov_Vitaly::Y = Turov_Vitaly::newpix[i][1];
		Turov_Vitaly::Z = Turov_Vitaly::newpix[i][2];

		Perspect();         // Считает проецию на плоскость экрана

		x = Turov_Vitaly::sX;
		y = Turov_Vitaly::sY;

		j = (i < 3) ? (i + 1) : 0;  // if (i < 3) j = i+1; else j = 0;

									// конечная точка
		Turov_Vitaly::X = Turov_Vitaly::newpix[j][0];
		Turov_Vitaly::Y = Turov_Vitaly::newpix[j][1];
		Turov_Vitaly::Z = Turov_Vitaly::newpix[j][2];

		// отрсовка проекции
		Perspect();

		// отрисовка линии
		line(x, y, Turov_Vitaly::sX, Turov_Vitaly::sY);
	}

	// рисуем главную грань (что значит главная грань)
	for (int i = 4, j = 0; i < 8; i++)
	{

		Turov_Vitaly::X = Turov_Vitaly::newpix[i][0];
		Turov_Vitaly::Y = Turov_Vitaly::newpix[i][1];
		Turov_Vitaly::Z = Turov_Vitaly::newpix[i][2];

		Perspect();

		x = Turov_Vitaly::sX;
		y = Turov_Vitaly::sY;

		j = (i < 7) ? (i + 1) : 4; // if (i < 7) j = i+1; else j = 4;

		Turov_Vitaly::X = Turov_Vitaly::newpix[j][0];
		Turov_Vitaly::Y = Turov_Vitaly::newpix[j][1];
		Turov_Vitaly::Z = Turov_Vitaly::newpix[j][2];

		Perspect();

		line(x, y, Turov_Vitaly::sX, Turov_Vitaly::sY);
	}

	// отрисовка соединяющего ребра
	for (int i = 0, j = 0; i < 4; i++)
	{

		Turov_Vitaly::X = Turov_Vitaly::newpix[i][0];
		Turov_Vitaly::Y = Turov_Vitaly::newpix[i][1];
		Turov_Vitaly::Z = Turov_Vitaly::newpix[i][2];

		Perspect();

		x = Turov_Vitaly::sX;
		y = Turov_Vitaly::sY;

		j = i + 4;

		Turov_Vitaly::X = Turov_Vitaly::newpix[j][0];
		Turov_Vitaly::Y = Turov_Vitaly::newpix[j][1];
		Turov_Vitaly::Z = Turov_Vitaly::newpix[j][2];

		Perspect();

		line(x, y, Turov_Vitaly::sX, Turov_Vitaly::sY);
	}
}