#define _USE_MATH_DEFINES
#include <math.h>
#include <string.h>
#include "Core.h"

//// width - столбцы, ширина
//// height - строки, высота
//#define width 16
//#define height 16

// Матрица отображения
bool matrix[height][width];

// Пространство имён с главными глобальными "объектами"
namespace Turov_Vitaly 
{
	// Количество вершин куба
	const int size = 8;

	// Исходные координаты куба
	const int pix[size][3] = { { -25, -25, -25 },{ 25, -25, -25 },{ 25, 25, -25 },{ -25, 25, -25 },
	{ -25, -25, 25 },{ 25, -25, 25 },{ 25, 25, 25 },{ -25, 25, 25 } };

	// Преобразованные координаты куба
	int newpix[size][3];

	/*
		sX, sY - спроецированные координаты
		X, Y, Z - длина сторон спроецированных на координатные линии
	*/
	int X, Y, Z, sX, sY;

	// Углы поворота по X,Y,Z в градусах
	int RotX(0), RotY(0), RotZ(0);

	// сдвиг начала координат
	int ShiftX(325), ShiftY(225);

	// масштабирование
	double Scale(8);
        
        // Массив отрезков. 12 - это количество рёбер у куба. Если легче, то столько раз вызывается функция line
	LineSegment lineSegment[12];
}

// Глобальное пространство имён со всеми функциями построения
namespace GLOBAL 
{
		void restart()
		{
			Turov_Vitaly::RotX = Turov_Vitaly::RotY = Turov_Vitaly::RotZ = 0;

			Turov_Vitaly::ShiftX = 325;
			Turov_Vitaly::ShiftY = 225;

			Turov_Vitaly::Scale = 8;
		}
		
        int len(char *str)
        {
            int line(0);
            
            for (; str[line] != '\0'; line++);
            
            return line;
        }

	// Сравнение слов (так как не корректно работает strcmp)
	bool comparison(char *str_one, char *str_two)
	{
		// Минимальная длина 
		int border = (strlen(str_one) < strlen(str_two)) ? (strlen(str_one)) : (strlen(str_two));

		for (int i = 0; i <= border; i++)
		{
			if (str_one[i] != str_two[i])
				return false;
		}

		return true;
	}

	// Очистка матрицы отображения
	void clearMatrix()
	{
		for (int i = 0; i < 16; i++)
			for (int j = 0; j < 16; j++)
				matrix[i][j] = false;
	}

	// Отрисовка линии в матрице
	void line(int x0, int y0, int sX, int sY)
	{
		int A(0), B(0), sign(0);

		A = sY - y0;
		B = x0 - sX;

		(ABS(A) > ABS(B)) ? (sign = 1) : (sign = -1);

		int signa, signb;

		(A < 0) ? (signa = -1) : (signa = 1);

		(B < 0) ? (signb = -1) : (signb = 1);

		int f = 0;

		if ((y0 >= 0) && (y0 < height) && (x0 >= 0) && (x0 < width))
			matrix[y0][x0] = true;

		int x(x0), y(y0);

		// Счётчики ограничители
		long visitor_one(0), visitor_two(0);

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

				if ((y >= 0) && (y < height) && (x >= 0) && (x < width))
					matrix[y][x] = true;

				// Если мы превысили разрешённый предел, то выкидываем исключение
				if (visitor_one > sqrt(pow(height, 2) + pow(width, 2)))
					throw 1;

				visitor_one++;

			} while (x != sX || y != sY);

		else
			do
			{

				f += B*signb;

				if (f > 0)
				{
					f -= A * signa;
					x -= signb;
				}

				y += signa;

				if ((y >= 0) && (y < height) && (x >= 0) && (x < width))
					matrix[y][x] = true;

				// Если мы превысили разрешённый предел, то выкидываем исключение
				if (visitor_two > sqrt(pow(height, 2) + pow(width, 2)))
					throw 1;

				visitor_two++;

			} while (x != sX || y != sY);
	}

	/*
		Вычисление фронтальной геометрической (изометрической) проекции
		трехмерных точек на двумерную плоскость
	*/
	void Perspect()
	{
		Turov_Vitaly::sX = Turov_Vitaly::X + Turov_Vitaly::Z / 2;
		Turov_Vitaly::sY = Turov_Vitaly::Y - Turov_Vitaly::Z / 2;
	}

	/*
		Использование аналитической геометрии.
	*/
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
			x1 = CEIL(x * cosz + y * sinz);
			y1 = CEIL(-x * sinz + y * cosz);
			z1 = z;

			// вокруг X
			x = x1;
			y = CEIL(y1 * cosx + z1 * sinx);
			z = CEIL(-y1 * sinx + z1 * cosx);

			// вокруг Y
			x1 = CEIL(x * cosy - z * siny);
			y1 = y;
			z1 = CEIL(x * siny + z * cosy);

			// Масштабирование
			Turov_Vitaly::newpix[i][0] = CEIL(x1 * Turov_Vitaly::Scale);
			Turov_Vitaly::newpix[i][1] = CEIL(y1 * Turov_Vitaly::Scale);
			Turov_Vitaly::newpix[i][2] = CEIL(z1 * Turov_Vitaly::Scale);

			// сдвиг по осям X и Y
			Turov_Vitaly::newpix[i][0] += Turov_Vitaly::ShiftX;
			Turov_Vitaly::newpix[i][1] += Turov_Vitaly::ShiftY;
			Turov_Vitaly::newpix[i][2] += 25;
		}
	}

	/*
		Это маленький костыль, но зато пришлось меньше мучаться.
		Объясню на защите)
	*/
	// Функция для приведения "здоровых размеров" куба к размеру нашего экрана
	void bringingScreenSize()
	{
		// Количество вызовов функции line
		const int countLine = 12;

		// Определяем отдалённые координаты (имеется ввиду отдельно максимум по x и по y)

		int xElement[countLine * 2];
		int yElement[countLine * 2];

		for (int i = 0, j = 0; i < countLine; i++, j += 2)
		{
			xElement[j] = Turov_Vitaly::lineSegment[i].x_one;
			xElement[j + 1] = Turov_Vitaly::lineSegment[i].x_two;

			yElement[j] = Turov_Vitaly::lineSegment[i].y_one;
			yElement[j + 1] = Turov_Vitaly::lineSegment[i].y_two;
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

		xK = CEIL((double)(xMax) / 16);
		yK = CEIL((double)(yMax) / 16);

		// Максимумы мы нашли. Теперь мы подгоняем под виртуальный экран (матрицу)
		// Примем максимальные значения за границы (правильнее примем их за 100% а остальные подгоним)
		for (int i = 0; i < countLine; i++)
		{
			Turov_Vitaly::lineSegment[i].x_one = ((double)(Turov_Vitaly::lineSegment[i].x_one) / xK);
			Turov_Vitaly::lineSegment[i].x_two = ((double)(Turov_Vitaly::lineSegment[i].x_two) / xK);

			Turov_Vitaly::lineSegment[i].y_one = ((double)(Turov_Vitaly::lineSegment[i].y_one) / yK);
			Turov_Vitaly::lineSegment[i].y_two = ((double)(Turov_Vitaly::lineSegment[i].y_two) / yK);
		}
	}

	/*
		Наша главная функция в которой происходить ("магия") рисование
	*/
	// Рисование кубика
	void DrawPix()
	{
		// Предварительная очистка матрицы (экрана)
		clearMatrix();

		int x(0), y(0), countLine(0);

		// отрисовка передней грани
		for (int i = 0, j = 0; i < 4; i++)
		{
			// исходная точка
			Turov_Vitaly::X = Turov_Vitaly::newpix[i][0];
			Turov_Vitaly::Y = Turov_Vitaly::newpix[i][1];
			Turov_Vitaly::Z = Turov_Vitaly::newpix[i][2];

			// Считает проецию на плоскость экрана
			Perspect();

			x = Turov_Vitaly::sX;
			y = Turov_Vitaly::sY;

			j = (i < 3) ? (i + 1) : 0;

			// конечная точка
			Turov_Vitaly::X = Turov_Vitaly::newpix[j][0];
			Turov_Vitaly::Y = Turov_Vitaly::newpix[j][1];
			Turov_Vitaly::Z = Turov_Vitaly::newpix[j][2];


			Perspect();

			Turov_Vitaly::lineSegment[countLine].x_one = x;
			Turov_Vitaly::lineSegment[countLine].y_one = y;
			Turov_Vitaly::lineSegment[countLine].x_two = Turov_Vitaly::sX;
			Turov_Vitaly::lineSegment[countLine].y_two = Turov_Vitaly::sY;

			countLine++;
		}

		// рисуем главную грань
		for (int i = 4, j = 0; i < 8; i++)
		{

			Turov_Vitaly::X = Turov_Vitaly::newpix[i][0];
			Turov_Vitaly::Y = Turov_Vitaly::newpix[i][1];
			Turov_Vitaly::Z = Turov_Vitaly::newpix[i][2];

			Perspect();

			x = Turov_Vitaly::sX;
			y = Turov_Vitaly::sY;

			j = (i < 7) ? (i + 1) : 4;

			Turov_Vitaly::X = Turov_Vitaly::newpix[j][0];
			Turov_Vitaly::Y = Turov_Vitaly::newpix[j][1];
			Turov_Vitaly::Z = Turov_Vitaly::newpix[j][2];

			Perspect();

			Turov_Vitaly::lineSegment[countLine].x_one = x;
			Turov_Vitaly::lineSegment[countLine].y_one = y;
			Turov_Vitaly::lineSegment[countLine].x_two = Turov_Vitaly::sX;
			Turov_Vitaly::lineSegment[countLine].y_two = Turov_Vitaly::sY;

			countLine++;
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

			Turov_Vitaly::lineSegment[countLine].x_one = x;
			Turov_Vitaly::lineSegment[countLine].y_one = y;
			Turov_Vitaly::lineSegment[countLine].x_two = Turov_Vitaly::sX;
			Turov_Vitaly::lineSegment[countLine].y_two = Turov_Vitaly::sY;

			countLine++;
		}

		// Приводим к нашим размерам
		bringingScreenSize();

		try
		{
			// Отрисовываем на виртуальном экране (матрице)
			for (int i = 0; i < countLine; i++)
				line(Turov_Vitaly::lineSegment[i].x_one, Turov_Vitaly::lineSegment[i].y_one,
					 Turov_Vitaly::lineSegment[i].x_two, Turov_Vitaly::lineSegment[i].y_two);
		}
		catch (...)
		{
			throw 1;
		}
	}
}

/*
Это наш интерфейс, который мы предоставляем для дальнейшего отображения кубика
*/
// Пространство имён с командами
namespace Commandos 
{
	void LEFT()
	{
		Turov_Vitaly::ShiftX -= 3;
	}

	void RIGHT()
	{
		Turov_Vitaly::ShiftX += 3;
	}

	void UP()
	{
		Turov_Vitaly::ShiftY -= 3;
	}

	void DOWN()
	{
		Turov_Vitaly::ShiftY += 3;
	}

	void ROTX()
	{
		// Полный круг или нет
		(Turov_Vitaly::RotX < 357) ? (Turov_Vitaly::RotX += 3) : (Turov_Vitaly::RotX = 0);
	}

	void ROTY()
	{
		// Полный круг или нет
		(Turov_Vitaly::RotY < 357) ? (Turov_Vitaly::RotY += 3) : (Turov_Vitaly::RotY = 0);
	}

	void ROTZ()
	{
		// Полный круг или нет
		(Turov_Vitaly::RotZ < 357) ? (Turov_Vitaly::RotZ += 3) : (Turov_Vitaly::RotZ = 0);
	}

	void PLUS()
	{
		Turov_Vitaly::Scale += 0.1;
	}

	void MINUS()
	{
		(Turov_Vitaly::Scale > 0.0) ? (Turov_Vitaly::Scale -= 0.1) : (false);
	}

	void ROTX_INF()
	{
		while (true)
		{
			ROTX();

			GLOBAL::Compute();

			GLOBAL::DrawPix();
		}
	}

	void ROTY_INF()
	{
		while (true)
		{
			ROTY();

			GLOBAL::Compute();

			GLOBAL::DrawPix();
		}
	}

	void ROTZ_INF()
	{
		while (true)
		{
			ROTZ();

			GLOBAL::Compute();

			GLOBAL::DrawPix();
		}
	}

	void LEFT_INF()
	{
		while (true)
		{
			LEFT();

			GLOBAL::Compute();

			GLOBAL::DrawPix();
		}
	}

	void RIGHT_INF()
	{
		while (true)
		{
			RIGHT();


			GLOBAL::Compute();

			GLOBAL::DrawPix();
		}
	}

	void UP_INF()
	{
		while (true)
		{
			UP();

			GLOBAL::Compute();

			GLOBAL::DrawPix();
		}
	}

	void DOWN_INF()
	{
		while (true)
		{
			DOWN();

			GLOBAL::Compute();

			GLOBAL::DrawPix();
		}
	}

	void MINUS_INF()
	{
		while (true)
		{
			MINUS();

			GLOBAL::Compute();

			GLOBAL::DrawPix();
		}
	}

	void PLUS_INF()
	{
		while (true)
		{
			PLUS();

			GLOBAL::Compute();

			GLOBAL::DrawPix();
		}
	}
}
