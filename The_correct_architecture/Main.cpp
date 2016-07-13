/*
* max7219.cpp
* Author: Thomas Müller
* Copyright 2013 Thomas Müller <tmuel at gmx.net>
* $Id$
* Автор программы КУБ: Туров Виталий
*/


/******************************************************************************
***   Include                                                               ***
******************************************************************************/

#include "Core.h"

#include <stdio.h>
#include <stdlib.h>
#include <iostream>

//#include "max7219.h"
#include <wiringPi.h>
#include <wiringPiSPI.h>
#include <unistd.h>

#include "joystick.hh"

Joystick joystick("/dev/input/js0");
JoystickEvent event;

using namespace std;

/******************************************************************************
***   Defines and Constants                                                 ***
******************************************************************************/

//MAX7219/MAX7221's memory register addresses:
// See Table 2 on page 7 in the Datasheet
const char NoOp = 0x00;
const char Digit0 = 0x01;
const char Digit1 = 0x02;
const char Digit2 = 0x03;
const char Digit3 = 0x04;
const char Digit4 = 0x05;
const char Digit5 = 0x06;
const char Digit6 = 0x07;
const char Digit7 = 0x08;
const char DecodeMode = 0x09;
const char Intensity = 0x0A;
const char ScanLimit = 0x0B;
const char ShutDown = 0x0C;
const char DisplayTest = 0x0F;


const char numOfDevices = 4;

void setup();
void loop();

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
	TxBuffer = new char[1024]; // Buffer for TxData
	RxBuffer = new char[1024]; // Buffer for RxData

	TxBufferIndex = 0;
	RxBufferIndex = 0;
}
RasPiSPI::~RasPiSPI() // DESTRUCTOR
{

	delete[] TxBuffer;
	delete[] RxBuffer;

	close(SpiFd); // Close SPI port
}

RasPiSPI SPI; // Create class SPI

void RasPiSPI::begin(int channel, int speed)
{
	if ((SpiFd = wiringPiSPISetup(channel, speed)) < 0)
	{    // Open port for reading and writing
		cout << "Failed to open SPI port " << channel << "! Please try with sudo" << endl;
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
	TxBufferIndex = 0; // ...and reset the index
}

/******************************************************************************
***   Main                                                                  ***
******************************************************************************/
int main(int argc, char **argv)
{
	int initOnly = 0;

	cout << argc << '\n';
	for (int i = 0;i<argc;i++)
	{

		if (strcmp(argv[i], "-init") == 0)
		{
			initOnly = 1;
		}
	}

	if (!joystick.isFound())
	{
		printf("open failed.\n");
		exit(1);
	}

	setup();
	while (1) loop();

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

void SetShutDown(char Mode) {
	SetData(ShutDown, !Mode);
}
void SetScanLimit(char Digits) {
	SetData(ScanLimit, Digits);
}
void SetIntensity(char intense) {
	SetData(Intensity, intense);
}
void SetDecodeMode(char Mode) {
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
	SetDecodeMode(false);
	// Set the number of digits; start to count at 0
	SetScanLimit(7);
	// Set the intensity between 0 and 15. Attention 0 is not off!
	SetIntensity(0);
	// Disable shutdown mode
	SetShutDown(false);


	// Write some patterns
	SetData(Digit0, 0b11111111, 1);
	SetData(Digit1, 0b00011000, 1);
	SetData(Digit2, 0b00011000, 1);
	SetData(Digit3, 0b00011000, 1);
	SetData(Digit4, 0b00011000, 1);
	SetData(Digit5, 0b00011000, 1);
	SetData(Digit6, 0b00011000, 1);
	SetData(Digit7, 0b11111111, 1);

	SetData(Digit0, 0b11111111, 2);
	SetData(Digit1, 0b00100100, 2);
	SetData(Digit2, 0b00100100, 2);
	SetData(Digit3, 0b00100100, 2);
	SetData(Digit4, 0b00100100, 2);
	SetData(Digit5, 0b00100100, 2);
	SetData(Digit6, 0b00100100, 2);
	SetData(Digit7, 0b11111111, 2);

	SetData(Digit0, 0b11111111, 3);
	SetData(Digit1, 0b01011010, 3);
	SetData(Digit2, 0b01011010, 3);
	SetData(Digit3, 0b01011010, 3);
	SetData(Digit4, 0b01011010, 3);
	SetData(Digit5, 0b01011010, 3);
	SetData(Digit6, 0b01011010, 3);
	SetData(Digit7, 0b11111111, 3);

	SetData(Digit0, 0b11111111, 4);
	SetData(Digit1, 0b10100101, 4);
	SetData(Digit2, 0b10100101, 4);
	SetData(Digit3, 0b10100101, 4);
	SetData(Digit4, 0b10100101, 4);
	SetData(Digit5, 0b10100101, 4);
	SetData(Digit6, 0b10100101, 4);
	SetData(Digit7, 0b11111111, 4);

	/*
		Расположение дисплеев
		1 | 2
		4 | 3
	*/

	// Ожидание в 10000 микросекунд
	delay(10000);

}
/******************************************************************************
***   Loop                                                                  ***
******************************************************************************/

// simplified display where 1 is lit and 0 is not lit
// x,y
char display[16][16];

// Очистка экрана и консоли
void clean() 
{
	system("clear");

	for (int i = 0; i < 16; i++)
		for (int j = 0; j < 16; j++)
			display[i][j] = 0;
}

// Заполнение данными матрицу
void SetMatrix()
{
	// Очистка
	clean();

	for (int i = 0; i < 16; i++)
		for (int j = 0; j < 16; j++)
			display[i][j] = (int)(Turov_Vitaly::matrix[i][j]);
}

// Вывод на экран
void show() 
{
	// we have 4 displays, this is row buffer for each cycle
	char row[4];

	// cycle through rows
	for (int rowCounter = 0; rowCounter < 8; ++rowCounter) 
	{
		row[0] = row[1] = row[2] = row[3] = 0;

		for (int i = 0; i < 8; ++i) 
		{
			row[0] = row[0] << 1;
			row[0] |= display[i][rowCounter];

			row[1] = row[1] << 1;
			row[1] |= display[i + 8][rowCounter];

			row[2] = row[2] << 1;
			row[2] |= display[i + 8][rowCounter + 8];

			row[3] = row[3] << 1;
			row[3] |= display[i][rowCounter + 8];
		}

		SetData(rowCounter + 1, row[0], 1);
		SetData(rowCounter + 1, row[1], 2);
		SetData(rowCounter + 1, row[2], 3);
		SetData(rowCounter + 1, row[3], 4);
	}
}

void loop()
{
	// Расчёт для отображения
	GLOBAL::Compute();

	// Отрисовка куба на виртуальном экране
	GLOBAL::DrawPix();

	// Заполнение матрицы для вывода на светодиоды
	SetMatrix();

	// Вывод на светодиоды
	show();

	//// Работа с джостиком
	//if (joystick.sample(&event)) 
	//{
	//	
	//}

	int ch;   // код клавиши
	char message[100] = "\0";

	/*
		Это временно.
		Пока с джостиком не разберусь.
	*/
	// Получаем команду с консоли
	std::cin >> message;

	if ((GLOBAL::comparison(message, "left") || GLOBAL::comparison(message, "Left") || GLOBAL::comparison(message, "LEFT")))
		ch = LEFT;

	else if ((GLOBAL::comparison(message, "left_inf") || GLOBAL::comparison(message, "Left_inf") || GLOBAL::comparison(message, "LEFT_INF")))
		ch = LEFT_INF;

	else if ((GLOBAL::comparison(message, "right") || GLOBAL::comparison(message, "Right") || GLOBAL::comparison(message, "RIGHT")))
		ch = RIGHT;

	else if ((GLOBAL::comparison(message, "right_inf") || GLOBAL::comparison(message, "Right_inf") || GLOBAL::comparison(message, "RIGHT_INF")))
		ch = RIGHT_INF;

	else if ((GLOBAL::comparison(message, "up") || GLOBAL::comparison(message, "Up") || GLOBAL::comparison(message, "UP")))
		ch = UP;

	else if ((GLOBAL::comparison(message, "up_inf") || GLOBAL::comparison(message, "Up_inf") || GLOBAL::comparison(message, "UP_INF")))
		ch = UP_INF;

	else if ((GLOBAL::comparison(message, "down") || GLOBAL::comparison(message, "Down") || GLOBAL::comparison(message, "DOWN")))
		ch = DOWN;

	else if ((GLOBAL::comparison(message, "down_inf") || GLOBAL::comparison(message, "Down_inf") || GLOBAL::comparison(message, "DOWN_INF")))
		ch = DOWN_INF;

	else if ((GLOBAL::comparison(message, "plus") || GLOBAL::comparison(message, "Plus") || GLOBAL::comparison(message, "PLUS")))
		ch = PLUS;

	else if ((GLOBAL::comparison(message, "plus_inf") || GLOBAL::comparison(message, "Plus_inf") || GLOBAL::comparison(message, "PLUS_INF")))
		ch = PLUS_INF;

	else if ((GLOBAL::comparison(message, "minus") || GLOBAL::comparison(message, "Minus") || GLOBAL::comparison(message, "MINUS")))
		ch = MINUS;

	else if ((GLOBAL::comparison(message, "minus_inf") || GLOBAL::comparison(message, "Minus_inf") || GLOBAL::comparison(message, "MINUS_INF")))
		ch = MINUS_INF;

	else if ((GLOBAL::comparison(message, "rotx") || GLOBAL::comparison(message, "Rotx") || GLOBAL::comparison(message, "ROTX")))
		ch = ROTX;

	else if ((GLOBAL::comparison(message, "rotx_inf") || GLOBAL::comparison(message, "Rotx_inf") || GLOBAL::comparison(message, "ROTX_INF")))
		ch = ROTX_INF;

	else if ((GLOBAL::comparison(message, "roty") || GLOBAL::comparison(message, "Roty") || GLOBAL::comparison(message, "ROTY")))
		ch = ROTY;

	else if ((GLOBAL::comparison(message, "roty_inf") || GLOBAL::comparison(message, "Roty_inf") || GLOBAL::comparison(message, "ROTY_INF")))
		ch = ROTY_INF;

	else if ((GLOBAL::comparison(message, "rotz") || GLOBAL::comparison(message, "Rotz") || GLOBAL::comparison(message, "ROTZ")))
		ch = ROTZ;

	else if ((GLOBAL::comparison(message, "rotz_inf") || GLOBAL::comparison(message, "Rotz_inf") || GLOBAL::comparison(message, "ROTZ_INF")))
		ch = ROTZ_INF;

	else if ((GLOBAL::comparison(message, "esc") || GLOBAL::comparison(message, "Esc") || GLOBAL::comparison(message, "ESC")))
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
	}
}