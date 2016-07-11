/*
* max7219.cpp
* Author: Thomas Müller
* Copyright 2013 Thomas Müller <tmuel at gmx.net>
* $Id$
* Автор куба: Туров Виталий
* Так как пишу вслепую, то прошу не ругать за огромное количество ошибок
*/


/******************************************************************************
***   Include                                                               ***
******************************************************************************/
#define _USE_MATH_DEFINES

#define CEIL(X) ((int)(X + 0.5))

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <cstring>
//#include "max7219.h"
#include <wiringPi.h>
#include <wiringPiSPI.h>
#include <unistd.h>

using namespace std;

int DEBUG_ACTIVE = 0; // Global debug variable

/******************************************************************************
***   Defines and Constants                                                 ***
******************************************************************************/

//MAX7219/MAX7221's memory register addresses:
// See Table 2 on page 7 in the Datasheet
const char NoOp = 0x00;

// Так удобнее. Можно работать прям в циле, а не в ручную
const char Digit[sizeof(char) * 8] = {	0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08 };

//const char Digit0 = 0x01;
//const char Digit1 = 0x02;
//const char Digit2 = 0x03;
//const char Digit3 = 0x04;
//const char Digit4 = 0x05;
//const char Digit5 = 0x06;
//const char Digit6 = 0x07;
//const char Digit7 = 0x08;

const char DecodeMode = 0x09;
const char Intensity = 0x0A;
const char ScanLimit = 0x0B;
const char ShutDown = 0x0C;
const char DisplayTest = 0x0F;


const char numOfDevices = 4;

/******************************************************************************
***   Function Definitions                                                  ***
******************************************************************************/

void setup();
void loop();

/******************************************************************************
***   Global Variables                                                      ***
******************************************************************************/



/******************************************************************************
***   Class: RasPiSPI                                                       ***
******************************************************************************/

class RasPiSPI
{
private:
	int SpiFd; // File descriptor of spi port

	char *TxBuffer;
	char *RxBuffer;

	int TxBufferIndex;
	int RxBufferIndex;

public:
	RasPiSPI(); // Konstruktor
	~RasPiSPI(); // Destruktor

	void begin() {
		begin(0, 1000000);
	} // default use channel 0 and 1MHz clock speed
	void begin(int, int);

	void transfer(char);
	void endTransfer();
};

RasPiSPI::RasPiSPI() // CONSTRUCTOR
{
	if (DEBUG_ACTIVE > 0) {
		cout << "RasPiSPI Konstruktor" << endl;
	}

	TxBuffer = new char[1024]; // Buffer for TxData
	RxBuffer = new char[1024]; // Buffer for RxData

	TxBufferIndex = 0;
	RxBufferIndex = 0;
}
RasPiSPI::~RasPiSPI() // DESTRUCTOR
{
	if (DEBUG_ACTIVE > 0) {
		cout << "RasPiSPI Destruktor" << endl;
	}

	delete[] TxBuffer;
	delete[] RxBuffer;

	close(SpiFd); // Close SPI port
}

RasPiSPI SPI; // Create class SPI

void RasPiSPI::begin(int channel, int speed)
{
	if ((SpiFd = wiringPiSPISetup(channel, speed)) < 0)
	{	// Open port for reading and writing
		cout << "Failed to open SPI port " << channel << "! Please try with sudo" << endl;
	}
	if (DEBUG_ACTIVE > 0) {
		cout << "Filehandle opened" << endl;
	}
}

void RasPiSPI::transfer(char c)
{
	TxBuffer[TxBufferIndex] = c;
	TxBufferIndex++;
}
void RasPiSPI::endTransfer()
{
	int temp = write(SpiFd, TxBuffer, TxBufferIndex); // Write the data from TxBuffer to the SPI bus...
	if (DEBUG_ACTIVE > 1)
	{ // Debug level 2
		cout << "Written: " << temp << " Index: " << TxBufferIndex << " Buffer: ";
		for (int i = 0; i < TxBufferIndex; i++)
		{
			cout << int(TxBuffer[i]) << " ";
		}
		cout << endl;
	}
	TxBufferIndex = 0; // ...and reset the index
}

/*
* End of class RasPiSPI
*/

/******************************************************************************
***   Main                                                                  ***
******************************************************************************/
int main(int argc, char **argv)
{
	int initOnly = 0;

	cout << argc << '\n';
	for (int i = 0;i<argc;i++)
	{
		if (DEBUG_ACTIVE > 0) {
			cout << argv[i] << '\n';
		}

		if (strcmp(argv[i], "-init") == 0)
		{
			initOnly = 1;
			cout << "Attention only initialisation will be done" << endl;
		}
		if (strcmp(argv[i], "-DEBUG") == 0)
		{
			DEBUG_ACTIVE = 1;
			cout << "!!! DEBUG ACTIVE !!!" << endl;
		}
	}

	if (DEBUG_ACTIVE > 0) {
		cout << "Program Started" << endl;
	}
	if (DEBUG_ACTIVE > 0) {
		cout << "Begin Setup" << endl;
	}
	setup();
	if (DEBUG_ACTIVE > 0) {
		cout << "Setup done" << endl;
	}

	if (initOnly != 0) {
		return 0;
	}

	while (1)
	{
		if (DEBUG_ACTIVE > 0) {
			cout << "Begin Loop" << endl;
		}
		loop();
		if (DEBUG_ACTIVE > 0) {
			cout << "End Loop" << endl;
		}
	}

	return 0;
}

// Writes data to the selected device or does broadcast if device number is 255
void SetData(char adr, char data, char device)
{
	// Count from top to bottom because first data which is sent is for the last device in the chain
	for (int i = numOfDevices; i > 0; i--)
	{
		if ((i == device) || (device == 255))
		{
			SPI.transfer(adr);
			SPI.transfer(data);
		}
		else // if its not the selected device send the noop command
		{
			SPI.transfer(NoOp);
			SPI.transfer(0);
		}
	}
	SPI.endTransfer();

	delay(1);
}

// Writes the same data to all devices
void SetData(char adr, char data) {
	SetData(adr, data, 255);
} // write to all devices (255 = Broadcast) 

void SetShutDown(char Mode) 
{
	SetData(ShutDown, !Mode);
}
void SetScanLimit(char Digits) 
{
	SetData(ScanLimit, Digits);
}

void SetIntensity(char intense) 
{
	SetData(Intensity, intense);
}

void SetDecodeMode(char Mode) 
{
	SetData(DecodeMode, Mode);
}

/******************************************************************************
***   Setup                                                                 ***
******************************************************************************/

void setup()
{
	// The MAX7219 has officially no SPI / Microwire support like the MAX7221 but the
	// serial interface is more or less the same like a SPI connection

	SPI.begin();

	// Disable the decode mode because at the moment i dont use 7-Segment displays
	if (DEBUG_ACTIVE > 0) 
	{
		cout << "SetDecodeMode(false);" << endl;
	}

	SetDecodeMode(false);

	// Set the number of digits; start to count at 0
	if (DEBUG_ACTIVE > 0) 
	{
		cout << "SetScanLimit(7);" << endl;
	}

	SetScanLimit(7);

	// Set the intensity between 0 and 15. Attention 0 is not off!
	if (DEBUG_ACTIVE > 0) 
	{
		cout << "SetIntensity(0);" << endl;
	}

	SetIntensity(0);

	// Disable shutdown mode
	if (DEBUG_ACTIVE > 0) 
	{
		cout << "SetShutDown(false);" << endl;
	}

	SetShutDown(false);

	if (DEBUG_ACTIVE > 0) 
	{
		cout << "Write Patterns" << endl;
	}

	// Write some patterns
	/*
		Видимо это для первого дисплея
	*/
	SetData(Digit[0], 0b10000000, 1);
	SetData(Digit[1], 0b01000000, 1);
	SetData(Digit[2], 0b00100000, 1);
	SetData(Digit[3], 0b00010000, 1);
	SetData(Digit[4], 0b00001000, 1);
	SetData(Digit[5], 0b00000100, 1);
	SetData(Digit[6], 0b00000010, 1);
	SetData(Digit[7], 0b00000001, 1);

	/*
		А это для второго и так далее
	*/

	SetData(Digit[0], 0b00000001, 2);
	SetData(Digit[1], 0b00000010, 2);
	SetData(Digit[2], 0b00000100, 2);
	SetData(Digit[3], 0b00001000, 2);
	SetData(Digit[4], 0b00010000, 2);
	SetData(Digit[5], 0b00100000, 2);
	SetData(Digit[6], 0b01000000, 2);
	SetData(Digit[7], 0b10000000, 2);

	SetData(Digit[0], 0b10000000, 3);
	SetData(Digit[1], 0b01000000, 3);
	SetData(Digit[2], 0b00100000, 3);
	SetData(Digit[3], 0b00010000, 3);
	SetData(Digit[4], 0b00001000, 3);
	SetData(Digit[5], 0b00000100, 3);
	SetData(Digit[6], 0b00000010, 3);
	SetData(Digit[7], 0b00000001, 3);

	SetData(Digit[0], 0b10000000, 4);
	SetData(Digit[1], 0b01000000, 4);
	SetData(Digit[2], 0b00100000, 4);
	SetData(Digit[3], 0b00010000, 4);
	SetData(Digit[4], 0b00001000, 4);
	SetData(Digit[5], 0b00000100, 4);
	SetData(Digit[6], 0b00000010, 4);
	SetData(Digit[7], 0b00000001, 4);

	/*
		Они ведь так располагаются?
		1 | 2
		3 | 4
		Я надеюсь на это =)
	*/

	if (DEBUG_ACTIVE > 0) 
	{
		cout << "Delay 1000" << endl;
	}

	delay(1000);

}

/******************************************************************************
***   Моя программа                                                         ***
******************************************************************************/
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
	MINUS,
	ROTX_INF,
	ROTY_INF,
	ROTZ_INF,
	LEFT_INF,
	RIGHT_INF,
	UP_INF,
	DOWN_INF,
	PLUS_INF,
	MINUS_INF
};

namespace Turov_Vitaly {
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
	У нас 2 экрана в ширину и 2 в высоту по 8 светодиодов, а char == 1 байт
	В конечном итого 4 матрицы

	Расположение дисплеев
	1 | 2
	3 | 4
	*/
	struct FourMatrixLED
	{
		unsigned char one_matrix[sizeof(char) * 8];
		unsigned char two_matrix[sizeof(char) * 8];
		unsigned char three_matrix[sizeof(char) * 8];
		unsigned char four_matrix[sizeof(char) * 8];
	};

	FourMatrixLED LED;
}

namespace GLOBAL {
	// Перевод матрицы в удобный вид для светодиодов
	void transformingBitMatrix()
	{
		// Массивы единиц
		unsigned char massiv_one[(int)(Turov_Vitaly::height / 2.0)][(sizeof(short) * 8) / 2];
		unsigned char massiv_two[(int)(Turov_Vitaly::height / 2.0)][(sizeof(short) * 8) / 2];
		unsigned char massiv_three[(int)(Turov_Vitaly::height / 2.0)][(sizeof(short) * 8) / 2];
		unsigned char massiv_four[(int)(Turov_Vitaly::height / 2.0)][(sizeof(short) * 8) / 2];

		// Буферная единица
		unsigned char tmp_one(1);

		// Преобразование булевой матрицы к числовым
		for (int i = 0; i < (Turov_Vitaly::height / 2); i++)
			for (int j = 0; j < ((sizeof(short) * 8) / 2); j++)
			{
				// Первый дисплей
				if (Turov_Vitaly::matrix[i][j])
				{
					tmp_one = 1;

					tmp_one = tmp_one << (sizeof(char) * 8 - j);

					massiv_one[i][j] = tmp_one;
				}

				else
					massiv_one[i][j] = 0;

				// Второй дисплей
				if (Turov_Vitaly::matrix[i][j + sizeof(char) * 8])
				{
					tmp_one = 1;

					tmp_one = tmp_one << (sizeof(char) * 8 - j);

					massiv_two[i][j] = tmp_one;
				}

				else
					massiv_two[i][j] = 0;

				// Третий дисплей
				if (Turov_Vitaly::matrix[i + sizeof(char) * 8][j])
				{
					tmp_one = 1;

					tmp_one = tmp_one << (sizeof(char) * 8 - j);

					massiv_three[i][j] = tmp_one;
				}

				else
					massiv_three[i][j] = 0;

				// Четвёртый дисплей
				if (Turov_Vitaly::matrix[i + sizeof(char) * 8][j + sizeof(char) * 8])
				{
					tmp_one = 1;

					tmp_one = tmp_one << (sizeof(char) * 8 - j);

					massiv_four[i][j] = tmp_one;
				}

				else
					massiv_four[i][j] = 0;
			}

		// Слияние столбцов
		for (int i = 0; i < (Turov_Vitaly::height / 2); i++)
			for (int j = 0; j < ((sizeof(short) * 8) / 2); j++)
			{
				Turov_Vitaly::LED.one_matrix[i] |= massiv_one[i][j];
				Turov_Vitaly::LED.two_matrix[i] |= massiv_two[i][j];
				Turov_Vitaly::LED.three_matrix[i] |= massiv_three[i][j];
				Turov_Vitaly::LED.four_matrix[i] |= massiv_four[i][j];
			}
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
		system("clear");

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

			(yElement[i] > yMax) ? (yMax = yEalement[i]) : (false);
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

		// Отрисовываем на виртуальном экране (матрице)
		for (int i = 0; i < countLine; i++)
			line(Turov_Vitaly::lineSegment[i].x_one, Turov_Vitaly::lineSegment[i].y_one,
				 Turov_Vitaly::lineSegment[i].x_two, Turov_Vitaly::lineSegment[i].y_two);

		// Вывод на экран консоли
		for (int i = 0; i < Turov_Vitaly::height; i++)
		{
			for (int j = 0; j < Turov_Vitaly::width; j++)
				if (Turov_Vitaly::matrix[i][j])
					std::cout << "*";
				else
					std::cout << " ";

			std::cout << std::endl;
		}

		// Перевод матрицы для отображения на светодиодов
		transformingBitMatrix();

		// Вывод матрицы на светодиоды
		for (int i = 1; i <= 4; i++)
			for (int j = 0; j < (sizeof(char) * 8); j++)
			{
				// Первый экран
				SetData(Digit[j], Turov_Vitaly::LED.one_matrix[j], i);

				// Второй экран
				SetData(Digit[j], Turov_Vitaly::LED.two_matrix[j], i);

				// Третий экран
				SetData(Digit[j], Turov_Vitaly::LED.three_matrix[j], i);

				// Четвёртый экран
				SetData(Digit[j], Turov_Vitaly::LED.four_matrix[j], i);
			}
	}
}

namespace Commandos {
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
/******************************************************************************
***   Loop                                                                  ***
******************************************************************************/

using namespace GLOBAL;

void loop()
{
	// Проецирование
	Compute();

	// вывести исходную фигуру
	DrawPix();

	int ch;   // код клавиши
	bool flag = true;  // признак того, что фигура имеется и ее надо перерисовать
	char message[100] = "\0";

	do
	{
		(flag) ? (flag) : (flag = true);

		std::cin >> message;

		if ((comparison(message, "left") || comparison(message, "Left") || comparison(message, "LEFT")))
			ch = LEFT;

		else if ((comparison(message, "left_inf") || comparison(message, "Left_inf") || comparison(message, "LEFT_INF")))
			ch = LEFT_INF;

		else if ((comparison(message, "right") || comparison(message, "Right") || comparison(message, "RIGHT")))
			ch = RIGHT;

		else if ((comparison(message, "right_inf") || comparison(message, "Right_inf") || comparison(message, "RIGHT_INF")))
			ch = RIGHT_INF;

		else if ((comparison(message, "up") || comparison(message, "Up") || comparison(message, "UP")))
			ch = UP;

		else if ((comparison(message, "up_inf") || comparison(message, "Up_inf") || comparison(message, "UP_INF")))
			ch = UP_INF;

		else if ((comparison(message, "down") || comparison(message, "Down") || comparison(message, "DOWN")))
			ch = DOWN;

		else if ((comparison(message, "down_inf") || comparison(message, "Down_inf") || comparison(message, "DOWN_INF")))
			ch = DOWN_INF;

		else if ((comparison(message, "plus") || comparison(message, "Plus") || comparison(message, "PLUS")))
			ch = PLUS;

		else if ((comparison(message, "plus_inf") || comparison(message, "Plus_inf") || comparison(message, "PLUS_INF")))
			ch = PLUS_INF;

		else if ((comparison(message, "minus") || comparison(message, "Minus") || comparison(message, "MINUS")))
			ch = MINUS;

		else if ((comparison(message, "minus_inf") || comparison(message, "Minus_inf") || comparison(message, "MINUS_INF")))
			ch = MINUS_INF;

		else if ((comparison(message, "rotx") || comparison(message, "Rotx") || comparison(message, "ROTX")))
			ch = ROTX;

		else if ((comparison(message, "rotx_inf") || comparison(message, "Rotx_inf") || comparison(message, "ROTX_INF")))
			ch = ROTX_INF;

		else if ((comparison(message, "roty") || comparison(message, "Roty") || comparison(message, "ROTY")))
			ch = ROTY;

		else if ((comparison(message, "roty_inf") || comparison(message, "Roty_inf") || comparison(message, "ROTY_INF")))
			ch = ROTY_INF;

		else if ((comparison(message, "rotz") || comparison(message, "Rotz") || comparison(message, "ROTZ")))
			ch = ROTZ;

		else if ((comparison(message, "rotz_inf") || comparison(message, "Rotz_inf") || comparison(message, "ROTZ_INF")))
			ch = ROTZ_INF;

		else if ((comparison(message, "esc") || comparison(message, "Esc") || comparison(message, "ESC")))
			ch = ESC;

		switch (ch)
		{
		case LEFT:
		{
			Commandos::LEFT();
			break;
		}
		case RIGHT:
		{
			Commandos::RIGHT();
			break;
		}
		case UP:
		{
			Commandos::UP();
			break;
		}
		case DOWN:
		{
			Commandos::DOWN();
			break;
		}
		case PLUS:
		{
			Commandos::PLUS();
			break;
		}
		case MINUS:
		{
			Commandos::MINUS();

			break;
		}
		case ROTX:
		{
			Commandos::ROTX();

			break;
		}
		case ROTY:
		{
			Commandos::ROTY();

			break;
		}
		case ROTZ:
		{
			Commandos::ROTZ();

			break;
		}
		case ROTX_INF:
		{
			Commandos::ROTX_INF();

			break;
		}
		case ROTY_INF:
		{
			Commandos::ROTY_INF();

			break;
		}
		case ROTZ_INF:
		{
			Commandos::ROTZ_INF();

			break;
		}
		case LEFT_INF:
		{
			Commandos::LEFT_INF();

			break;
		}
		case RIGHT_INF:
		{
			Commandos::RIGHT_INF();

			break;
		}
		case UP_INF:
		{
			Commandos::UP_INF();

			break;
		}
		case DOWN_INF:
		{
			Commandos::DOWN_INF();

			break;
		}
		case MINUS_INF:
		{
			Commandos::MINUS_INF();

			break;
		}
		case PLUS_INF:
		{
			Commandos::PLUS_INF();

			break;
		}
		default:
			flag = false;
		}

		if (flag)
		{
			Compute();    // вычисляем новые координаты

			DrawPix();    // рисуем

			flag = false;
		}

	} while (ch != ESC);

	////you may know this from space invaders
	//unsigned int rowBuffer[] =
	//{
	//	0b0010000010000000,
	//	0b0001000100000000,
	//	0b0011111110000000,
	//	0b0110111011000000,
	//	0b1111111111100000,
	//	0b1011111110100000,
	//	0b1010000010100000,
	//	0b0001101100000000
	//};

	if (DEBUG_ACTIVE > 0) {
		cout << "Start with space invader animation" << endl;
	}



	//while (1)
	//{
	//	for (int shiftCounter = 0; 31 >= shiftCounter; shiftCounter++)
	//	{
	//		for (int rowCounter = 0; 7 >= rowCounter; rowCounter++)
	//		{
	//			// roll the 16bits...
	//			// The information how to roll is from http://arduino.cc/forum/index.php?topic=124188.0 
	//			rowBuffer[rowCounter] = ((rowBuffer[rowCounter] & 0x8000) ? 0x01 : 0x00) | (rowBuffer[rowCounter] << 1);

	//			// ...and then write them to the two devices
	//			SetData(rowCounter + 1, char(rowBuffer[rowCounter]), 1);
	//			SetData(rowCounter + 1, char(rowBuffer[rowCounter] >> 8), 2);
	//			SetData(rowCounter + 1, char(rowBuffer[rowCounter] >> 16), 3);
	//			SetData(rowCounter + 1, char(rowBuffer[rowCounter] >> 24), 4);
	//		}
	//		delay(100);
	//	}
	//}
}
