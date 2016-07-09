#define _USE_MATH_DEFINES

#define CEIL(X) ((int)(X + 0.5))

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
	// width - столбцы, ширина
	// height - строки, высота
	const int width = 16;
	const int height = 16;

	// Матрица отображения
	bool matrix[height][width];

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

	// углы поворота по X,Y,Z в градусах
	int RotX = 0, RotY = 0, RotZ = 0;

	// сдвиг начала координат
	int ShiftX = 325, ShiftY = 225;

	// масштабирование
	double Scale = 8;

	// Структура описывающие отрезки
	struct LineSegment
	{
		// Первая точка
		int x_one;
		int y_one;

		// Вторая точка
		int x_two;
		int y_two;
	};

	// Массив отрезков. 12 - это количество рёбер у куба. Если легче, то столько раз вызывается функция line
	LineSegment lineSegment[12];

	// Битовая матрица для отрисовки на светодиодной матрице
	/*
		Количество строк - это высота нашего виртуального экрана
		Количество столбцов - это количество бит в числе
		У нас 2 экрана в ширину по 8 светодиотов, а short == 2 байта
	*/
	unsigned short int bitMatrix[height] = {0};
}

// Перевод матрицы в удобный вид для светодиодов
void transformingBitMatrix()
{
	// Массив единиц
	unsigned short int massiv[Turov_Vitaly::height][sizeof(short) * 8];

	// Буферная единица
	unsigned short int tmp_one(1);

	// Преобразование булевой матрици к числовой
	for (int i = 0; i < Turov_Vitaly::height; i++)
	{
		for (int j = 0; j < sizeof(short) * 8; j++)
		{
			if (Turov_Vitaly::matrix[i][j])
			{
				tmp_one = 1;

				// Эта операция игнорируется - вопрос почему?
				tmp_one << (sizeof(short) * 8 - j);

				massiv[i][j] = tmp_one;
			}

			else
			{
				massiv[i][j] = 0;
			}
		}
	}

	// Слияние столбцов
	for (int i = 0; i < Turov_Vitaly::height; i++)
		for (int j = 0; j < sizeof(short) * 8; j++)
			Turov_Vitaly::bitMatrix[i] |= massiv[i][j];

}

// Сравнение слов
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

// Очистка матрицы
void clearMatrix()
{
	system("cls");

	for (int i = 0; i < Turov_Vitaly::height; i++)
		for (int j = 0; j < Turov_Vitaly::width; j++)
			Turov_Vitaly::matrix[i][j] = false;
}

// Отрисовка линии в матрице
void line(int x0, int y0, int sX, int sY)
{
	int A(0), B(0), sign(0);

	A = sY - y0;
	B = x0 - sX;

	(abs(A) > abs(B)) ? (sign = 1) : (sign = -1);

	int signa, signb;

	(A < 0) ? (signa = -1) : (signa = 1);

	(B < 0) ? (signb = -1) : (signb = 1);

	int f = 0;

	if ((y0 >= 0) && (y0 < Turov_Vitaly::height) && (x0 >= 0) && (x0 < Turov_Vitaly::width))
		Turov_Vitaly::matrix[y0][x0] = true;

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

			if ((y >= 0) && (y < Turov_Vitaly::height) && (x >= 0) && (x < Turov_Vitaly::width))
				Turov_Vitaly::matrix[y][x] = true;

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

			if ((y >= 0) && (y < Turov_Vitaly::height) && (x >= 0) && (x < Turov_Vitaly::width))
				Turov_Vitaly::matrix[y][x] = true;

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
	Чистая математика без разумных объяснений =)
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

	xK = CEIL((double)(xMax) / Turov_Vitaly::width);
	yK = CEIL((double)(yMax) / Turov_Vitaly::height);

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

		// отрисовка линии
		//line(x, y, Turov_Vitaly::sX, Turov_Vitaly::sY);
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

		//line(x, y, Turov_Vitaly::sX, Turov_Vitaly::sY);
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

		//line(x, y, Turov_Vitaly::sX, Turov_Vitaly::sY);
	}

	// Приводим к нашим размерам
	bringingScreenSize();

	// Отрисовываем на виртуальном экране (матрице)
	for (int i = 0; i < countLine; i++)
		line(Turov_Vitaly::lineSegment[i].x_one, Turov_Vitaly::lineSegment[i].y_one,
			 Turov_Vitaly::lineSegment[i].x_two, Turov_Vitaly::lineSegment[i].y_two);

	// Вывод на экран
	for (int i = 0; i < Turov_Vitaly::height; i++)
	{
		for (int j = 0; j < Turov_Vitaly::width; j++)
			if (Turov_Vitaly::matrix[i][j])
				std::cout << "*";
			else
				std::cout << " ";

		std::cout << std::endl;
	}
}

int main(void)
{
	// Проецирование
	Compute();

	// вывести исходную фигуру
	DrawPix();

	int ch(INT_MAX);   // код клавиши
	bool flag = true;  // признак того, что фигура имеется и ее надо перерисовать
	char message[5] = "\0";

	do
	{
		(flag) ? (flag) : (flag = true);

		std::cin >> message;

		if ((comparison(message, "left") || comparison(message, "Left") || comparison(message, "LEFT")))
			ch = LEFT;

		else if ((comparison(message, "right") || comparison(message, "Right") || comparison(message, "RIGHT")))
			ch = RIGHT;

		else if ((comparison(message, "up") || comparison(message, "Up") || comparison(message, "UP")))
			ch = UP;

		else if ((comparison(message, "down") || comparison(message, "Down") || comparison(message, "DOWN")))
			ch = DOWN;

		else if ((comparison(message, "plus") || comparison(message, "Plus") || comparison(message, "PLUS")))
			ch = PLUS;

		else if ((comparison(message, "minus") || comparison(message, "Minus") || comparison(message, "MINUS")))
			ch = MINUS;

		else if ((comparison(message, "rotx") || comparison(message, "Rotx") || comparison(message, "ROTX")))
			ch = ROTX;

		else if ((comparison(message, "roty") || comparison(message, "Roty") || comparison(message, "ROTY")))
			ch = ROTY;

		else if ((comparison(message, "rotz") || comparison(message, "Rotz") || comparison(message, "ROTZ")))
			ch = ROTZ;

		else if ((comparison(message, "esc") || comparison(message, "Esc") || comparison(message, "ESC")))
			ch = ESC;

		switch (ch)
		{
		case LEFT:
		{
			Turov_Vitaly::ShiftX -= 3;
			break;
		}
		case RIGHT:
		{
			Turov_Vitaly::ShiftX += 3;
			break;
		}
		case UP:
		{
			Turov_Vitaly::ShiftY -= 3;
			break;
		}
		case DOWN:
		{
			Turov_Vitaly::ShiftY += 3;
			break;
		}
		case PLUS:
		{
			Turov_Vitaly::Scale += 0.1;
			break;
		}
		case MINUS:
		{
			(Turov_Vitaly::Scale > 0.0) ? (Turov_Vitaly::Scale -= 0.1) : (false);

			break;
		}
		case ROTX:
		{
			// Полный круг или нет
			(Turov_Vitaly::RotX < 357) ? (Turov_Vitaly::RotX += 3) : (Turov_Vitaly::RotX = 0);

			break;
		}
		case ROTY:
		{
			// Полный круг или нет
			(Turov_Vitaly::RotY < 357) ? (Turov_Vitaly::RotY += 3) : (Turov_Vitaly::RotY = 0);

			break;
		}
		case ROTZ:
		{
			// Полный круг или нет
			(Turov_Vitaly::RotZ < 357) ? (Turov_Vitaly::RotZ += 3) : (Turov_Vitaly::RotZ = 0);

			break;
		}
		default:
			flag = false;
		}

		if (flag)
		{
			Compute();    // вычисляем новые координаты

			DrawPix();    // рисуем

			transformingBitMatrix();

			flag = false;
		}

	} while (ch != ESC);

	return 0;
}