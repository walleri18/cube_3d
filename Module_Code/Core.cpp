#define _USE_MATH_DEFINES
#include "Core.h"
#include <math.h>

Core::Core() : ShiftX(325), ShiftY(225), Scale(8),
RotX(0), RotY(0), RotZ(0), X(0), Y(0), Z(0), sX(0),
sY(0), width(16), height(16)
{
	this->matrix = new bool*[this->height];
	
	for (int i = 0; i < this->height; i++)
		this->matrix[i] = new bool[this->width];
}

int Core::getWidth() const
{
	return this->width;
}

int Core::getHeight() const
{
	return this->height;
}

bool ** Core::getMatrix() const
{
	return this->matrix;
}

int Core::ceil(double arg)
{
	return ((int)(arg + 0.5));
}

int Core::abs(double arg)
{
	return ((arg > 0) ? (arg) : (-arg));
}

int Core::len(char *str)
{
	int line(0);

	for (; str[line] != '\0'; line++);

	return line;
}

bool Core::comparison(char *str_one, char *str_two)
{
	int border = (len(str_one) < len(str_two)) ? (len(str_one)) : (len(str_two));

	for (int i = 0; i <= border; i++)
	{
		if (str_one[i] != str_two[i])
			return false;
	}

	return true;
}

void Core::DrawPix()
{
	// Предварительная очистка матрицы (экрана)
	this->clearMatrix();

	// Расчёт
	this->Compute();

	int x(0), y(0), countLine(0);

	// отрисовка передней грани
	for (int i = 0, j = 0; i < 4; i++)
	{
		// исходная точка
		this->X = this->newpix[i][0];
		this->Y = this->newpix[i][1];
		this->Z = this->newpix[i][2];

		// Считает проецию на плоскость экрана
		Perspect();

		x = this->sX;
		y = this->sY;

		j = (i < 3) ? (i + 1) : 0;

		// конечная точка
		this->X = this->newpix[j][0];
		this->Y = this->newpix[j][1];
		this->Z = this->newpix[j][2];


		Perspect();

		this->lineSegment[countLine].x_one = x;
		this->lineSegment[countLine].y_one = y;
		this->lineSegment[countLine].x_two = this->sX;
		this->lineSegment[countLine].y_two = this->sY;

		countLine++;
	}

	// рисуем главную грань
	for (int i = 4, j = 0; i < 8; i++)
	{

		this->X = this->newpix[i][0];
		this->Y = this->newpix[i][1];
		this->Z = this->newpix[i][2];

		Perspect();

		x = this->sX;
		y = this->sY;

		j = (i < 7) ? (i + 1) : 4;

		this->X = this->newpix[j][0];
		this->Y = this->newpix[j][1];
		this->Z = this->newpix[j][2];

		Perspect();

		this->lineSegment[countLine].x_one = x;
		this->lineSegment[countLine].y_one = y;
		this->lineSegment[countLine].x_two = this->sX;
		this->lineSegment[countLine].y_two = this->sY;

		countLine++;
	}

	// отрисовка соединяющего ребра
	for (int i = 0, j = 0; i < 4; i++)
	{
		this->X = this->newpix[i][0];
		this->Y = this->newpix[i][1];
		this->Z = this->newpix[i][2];

		Perspect();

		x = this->sX;
		y = this->sY;

		j = i + 4;

		this->X = this->newpix[j][0];
		this->Y = this->newpix[j][1];
		this->Z = this->newpix[j][2];

		Perspect();

		this->lineSegment[countLine].x_one = x;
		this->lineSegment[countLine].y_one = y;
		this->lineSegment[countLine].x_two = this->sX;
		this->lineSegment[countLine].y_two = this->sY;

		countLine++;
	}

	// Приводим к нашим размерам
	bringingScreenSize();

	// Отрисовываем на виртуальном экране (матрице)
	for (int i = 0; i < countLine; i++)
		this->line(this->lineSegment[i].x_one, this->lineSegment[i].y_one,
				   this->lineSegment[i].x_two, this->lineSegment[i].y_two);
}

void Core::LEFT()
{
	this->ShiftX -= 3;
}

void Core::RIGHT()
{
	this->ShiftX += 3;
}

void Core::UP()
{
	this->ShiftY -= 3;
}

void Core::DOWN()
{
	this->ShiftY += 3;
}

void Core::ROTX()
{
	// Полный круг или нет
	(this->RotX < 357) ? (this->RotX += 3) : (this->RotX = 0);
}

void Core::ROTY()
{
	// Полный круг или нет
	(this->RotY < 357) ? (this->RotY += 3) : (this->RotY = 0);
}

void Core::ROTZ()
{
	// Полный круг или нет
	(this->RotZ < 357) ? (this->RotZ += 3) : (this->RotZ = 0);
}

void Core::PLUS()
{
	this->Scale += 0.1;
}

void Core::MINUS()
{
	(this->Scale > 0.0) ? (this->Scale -= 0.1) : (false);
}

void Core::clearMatrix()
{
	for (int i = 0; i < 16; i++)
		for (int j = 0; j < 16; j++)
			this->matrix[i][j] = false;
}

void Core::Perspect()
{
	this->sX = this->X + this->Z / 2;
	this->sY = this->Y - this->Z / 2;
}

void Core::Compute()
{
	// Считаем направляющие синусы и косинусы
	double sinx = sin((this->RotX * M_PI) / 180.0);
	double siny = sin((this->RotY * M_PI) / 180.0);
	double sinz = sin((this->RotZ * M_PI) / 180.0);
	double cosx = cos((this->RotX * M_PI) / 180.0);
	double cosy = cos((this->RotY * M_PI) / 180.0);
	double cosz = cos((this->RotZ * M_PI) / 180.0);

	// есть старые координаты точек ( в 3D ) и нужно получить координаты точек (в 2D)
	double x, y, z, x1, y1, z1;

	for (int i = 0; i < Core::size; i++)
	{

		//Исходное направление
		x = this->pix[i][0];
		y = this->pix[i][1];
		z = this->pix[i][2];

		// вокруг Z
		x1 = Core::ceil(x * cosz + y * sinz);
		y1 = Core::ceil(-x * sinz + y * cosz);
		z1 = z;

		// вокруг X
		x = x1;
		y = Core::ceil(y1 * cosx + z1 * sinx);
		z = Core::ceil(-y1 * sinx + z1 * cosx);

		// вокруг Y
		x1 = Core::ceil(x * cosy - z * siny);
		y1 = y;
		z1 = Core::ceil(x * siny + z * cosy);

		// Масштабирование
		this->newpix[i][0] = Core::ceil(x1 * this->Scale);
		this->newpix[i][1] = Core::ceil(y1 * this->Scale);
		this->newpix[i][2] = Core::ceil(z1 * this->Scale);

		// сдвиг по осям X и Y
		this->newpix[i][0] += this->ShiftX;
		this->newpix[i][1] += this->ShiftY;
		this->newpix[i][2] += 25;
	}
}

void Core::bringingScreenSize()
{
	// Количество вызовов функции line
	const int countLine = 12;

	// Определяем отдалённые координаты (имеется ввиду отдельно максимум по x и по y)

	int xElement[countLine * 2];
	int yElement[countLine * 2];

	for (int i = 0, j = 0; i < countLine; i++, j += 2)
	{
		xElement[j] = this->lineSegment[i].x_one;
		xElement[j + 1] = this->lineSegment[i].x_two;

		yElement[j] = this->lineSegment[i].y_one;
		yElement[j + 1] = this->lineSegment[i].y_two;
	}

	int xMax = xElement[0];
	int yMax = yElement[0];

	for (int i = 0; i < (countLine * 2); i++)
	{
		(xElement[i] > xMax) ? (xMax = xElement[i]) : (false);

		(yElement[i] > yMax) ? (yMax = yElement[i]) : (false);
	}

	// Расчитаем масштабирование
	int xK(1), yK(1);

	xK = Core::ceil((double)(xMax) / 16);
	yK = Core::ceil((double)(yMax) / 16);

	// Максимумы мы нашли. Теперь мы подгоняем под виртуальный экран (матрицу)
	// Примем максимальные значения за границы (правильнее примем их за 100% а остальные подгоним)
	for (int i = 0; i < countLine; i++)
	{
		this->lineSegment[i].x_one = ((double)(this->lineSegment[i].x_one) / xK);
		this->lineSegment[i].x_two = ((double)(this->lineSegment[i].x_two) / xK);

		this->lineSegment[i].y_one = ((double)(this->lineSegment[i].y_one) / yK);
		this->lineSegment[i].y_two = ((double)(this->lineSegment[i].y_two) / yK);
	}
}

void Core::line(int x0, int y0, int sX, int sY)
{
	int A(0), B(0), sign(0);

	A = sY - y0;
	B = x0 - sX;

	(Core::abs(A) > Core::abs(B)) ? (sign = 1) : (sign = -1);

	int signa, signb;

	(A < 0) ? (signa = -1) : (signa = 1);

	(B < 0) ? (signb = -1) : (signb = 1);

	int f = 0;

	if ((y0 >= 0) && (y0 < this->height) && (x0 >= 0) && (x0 < this->width))
		this->matrix[y0][x0] = true;

	int x(x0), y(y0);

	if (sign == -1)
		do
		{

			f += A * signa;

			if (f > 0)
			{
				f -= B * signb;
				y += signa;
			}

			x -= signb;

			if ((y >= 0) && (y < this->height) && (x >= 0) && (x < this->width))
				this->matrix[y][x] = true;

		} while (x != sX || y != sY);

	else
		do
		{

			f += B * signb;

			if (f > 0)
			{
				f -= A * signa;
				x -= signb;
			}

			y += signa;

			if ((y >= 0) && (y < this->height) && (x >= 0) && (x < this->width))
				this->matrix[y][x] = true;

		} while (x != sX || y != sY);
}
