﻿/*
* max7219.cpp
* Author: Thomas <- Он автор лишь RasberriPI
* Copyright 2013 Thomas
*/


/******************************************************************************
***   Include                                                               ***
******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <iostream>

//#include "max7219.h"
#include <wiringPi.h>
#include <wiringPiSPI.h>
#include <unistd.h>

#include "joystick.hh"

#include "Core.h"

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
	} 
	// default use channel 0 and 1MHz clock speed
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

		if (GLOBAL::comparison(argv[i], (char*)"-init") == 0)
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
// Первый кадр бабочки
void one_cadr()
{
	// Первый монитор
	SetData(Digit0, 0b11000000, 1);
	SetData(Digit1, 0b10110000, 1);
	SetData(Digit2, 0b10001000, 1);
	SetData(Digit3, 0b10000100, 1);
	SetData(Digit4, 0b01000010, 1);
	SetData(Digit5, 0b01000010, 1);
	SetData(Digit6, 0b01000010, 1);
	SetData(Digit7, 0b00111001, 1);

	// Второй монитор
	SetData(Digit0, 0b00000011, 2);
	SetData(Digit1, 0b00001101, 2);
	SetData(Digit2, 0b00010001, 2);
	SetData(Digit3, 0b00100001, 2);
	SetData(Digit4, 0b01000010, 2);
	SetData(Digit5, 0b01000010, 2);
	SetData(Digit6, 0b01000010, 2);
	SetData(Digit7, 0b10011100, 2);

	// Третий монитор
	SetData(Digit0, 0b01100000, 3);
	SetData(Digit1, 0b00011000, 3);
	SetData(Digit2, 0b00000111, 3);
	SetData(Digit3, 0b10000001, 3);
	SetData(Digit4, 0b01000010, 3);
	SetData(Digit5, 0b00110100, 3);
	SetData(Digit6, 0b00001010, 3);
	SetData(Digit7, 0b00000001, 3);

	// Четвёртый монитор
	SetData(Digit0, 0b00000110, 4);
	SetData(Digit1, 0b00011000, 4);
	SetData(Digit2, 0b11100000, 4);
	SetData(Digit3, 0b10000001, 4);
	SetData(Digit4, 0b01000010, 4);
	SetData(Digit5, 0b00101100, 4);
	SetData(Digit6, 0b01010000, 4);
	SetData(Digit7, 0b10000000, 4);
}

// Второй кадр бабочки
void two_cadr()
{
	// Первый монитор
	SetData(Digit0, 0b00110000, 1);
	SetData(Digit1, 0b00110000, 1);
	SetData(Digit2, 0b00101000, 1);
	SetData(Digit3, 0b00100100, 1);
	SetData(Digit4, 0b00010010, 1);
	SetData(Digit5, 0b00010010, 1);
	SetData(Digit6, 0b00001010, 1);
	SetData(Digit7, 0b00000101, 1);

	// Второй монитор
	SetData(Digit0, 0b00001100, 2);
	SetData(Digit1, 0b00001100, 2);
	SetData(Digit2, 0b00010100, 2);
	SetData(Digit3, 0b00100100, 2);
	SetData(Digit4, 0b01001000, 2);
	SetData(Digit5, 0b01001000, 2);
	SetData(Digit6, 0b01010000, 2);
	SetData(Digit7, 0b10100000, 2);

	// Третий монитор
	SetData(Digit0, 0b01000000, 3);
	SetData(Digit1, 0b00110000, 3);
	SetData(Digit2, 0b00001100, 3);
	SetData(Digit3, 0b10000100, 3);
	SetData(Digit4, 0b01001000, 3);
	SetData(Digit5, 0b00101000, 3);
	SetData(Digit6, 0b00011000, 3);
	SetData(Digit7, 0b00000001, 3);

	// Четвёртый монитор
	SetData(Digit0, 0b00000010, 4);
	SetData(Digit1, 0b00001100, 4);
	SetData(Digit2, 0b00110000, 4);
	SetData(Digit3, 0b00100001, 4);
	SetData(Digit4, 0b00010010, 4);
	SetData(Digit5, 0b00010100, 4);
	SetData(Digit6, 0b00011000, 4);
	SetData(Digit7, 0b00100000, 4);
}

// Третий кадр бабочки
void three_cadr()
{
	// Доделать

	// Первый монитор
	SetData(Digit0, 0b00001000, 1);
	SetData(Digit1, 0b00000100, 1);
	SetData(Digit2, 0b00001100, 1);
	SetData(Digit3, 0b00000100, 1);
	SetData(Digit4, 0b00000100, 1);
	SetData(Digit5, 0b00000010, 1);
	SetData(Digit6, 0b00000010, 1);
	SetData(Digit7, 0b00000101, 1);

	// Второй монитор
	SetData(Digit0, 0b00010000, 2);
	SetData(Digit1, 0b00100000, 2);
	SetData(Digit2, 0b00110000, 2);
	SetData(Digit3, 0b00100000, 2);
	SetData(Digit4, 0b00100000, 2);
	SetData(Digit5, 0b01000000, 2);
	SetData(Digit6, 0b01000000, 2);
	SetData(Digit7, 0b10100000, 2);

	// Третий монитор
	SetData(Digit0, 0b00100000, 3);
	SetData(Digit1, 0b01010000, 3);
	SetData(Digit2, 0b00010000, 3);
	SetData(Digit3, 0b11010000, 3);
	SetData(Digit4, 0b00110000, 3);
	SetData(Digit5, 0b00000000, 3);
	SetData(Digit6, 0b00010000, 3);
	SetData(Digit7, 0b00000000, 3);

	// Четвёртый монитор
	SetData(Digit0, 0b00000100, 4);
	SetData(Digit1, 0b00001010, 4);
	SetData(Digit2, 0b00001000, 4);
	SetData(Digit3, 0b00001011, 4);
	SetData(Digit4, 0b00001100, 4);
	SetData(Digit5, 0b00000000, 4);
	SetData(Digit6, 0b00001000, 4);
	SetData(Digit7, 0b00000000, 4);
}

// Функция анимации
void animation()
{
	for (int i = 0; i < 10; i++)
	{
		one_cadr();

		delay(300);

		two_cadr();

		delay(300);

		three_cadr();

		delay(300);

		two_cadr();

		delay(300);
	}
        
        // One Screen
        SetData(Digit0, 0b11111111, 1);
        SetData(Digit1, 0b10000000, 1);
        SetData(Digit2, 0b10111101, 1);
        SetData(Digit3, 0b10100000, 1);
        SetData(Digit4, 0b10010000, 1);
        SetData(Digit5, 0b10001000, 1);
        SetData(Digit6, 0b10111100, 1);
        SetData(Digit7, 0b10000000, 1);
        
        // Two Screen
        SetData(Digit0, 0b11111111, 2);
        SetData(Digit1, 0b00000001, 2);
        SetData(Digit2, 0b11110001, 2);
        SetData(Digit3, 0b01000001, 2);
        SetData(Digit4, 0b01000001, 2);
        SetData(Digit5, 0b01000001, 2);
        SetData(Digit6, 0b01000001, 2);
        SetData(Digit7, 0b00000001, 2);
        
        // Three Screen
        SetData(Digit0, 0b00000001, 3);
        SetData(Digit1, 0b11001111, 3);
        SetData(Digit2, 0b10100101, 3);
        SetData(Digit3, 0b11100101, 3);
        SetData(Digit4, 0b10100101, 3);
        SetData(Digit5, 0b10100101, 3);
        SetData(Digit6, 0b00000001, 3);
        SetData(Digit7, 0b11111111, 3);
        
        // Four Screen
        SetData(Digit0, 0b10000000, 4);
        SetData(Digit1, 0b10001000, 4);
        SetData(Digit2, 0b10010100, 4);
        SetData(Digit3, 0b10010100, 4);
        SetData(Digit4, 0b10101010, 4);
        SetData(Digit5, 0b10100010, 4);
        SetData(Digit6, 0b10000000, 4);
        SetData(Digit7, 0b11111111, 4);
        
        delay(10000);
}

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
	SetData(Digit0, 0b01111000, 1);
	SetData(Digit1, 0b11111000, 1);
	SetData(Digit2, 0b11011000, 1);
	SetData(Digit3, 0b11011000, 1);
	SetData(Digit4, 0b00011000, 1);
	SetData(Digit5, 0b00011000, 1);
	SetData(Digit6, 0b11111111, 1);
	SetData(Digit7, 0b11111111, 1);

	SetData(Digit0, 0b00111000, 2);
	SetData(Digit1, 0b11111110, 2);
	SetData(Digit2, 0b11000110, 2);
	SetData(Digit3, 0b11000110, 2);
	SetData(Digit4, 0b00001100, 2);
	SetData(Digit5, 0b00001100, 2);
	SetData(Digit6, 0b11111111, 2);
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

	// Ожидание 1000 мс
	delay(1000);
        
        animation();

}
/******************************************************************************
***   Loop                                                                  ***
******************************************************************************/
// width - столбцы, ширина
// height - строки, высота
#define width 16
#define height 16

// дисплей для дальнейшего отображения
char display[height][width];

// Очистка экрана
void clean() 
{
	for (int i = 0; i < height; i++)
		for (int j = 0; j < width; j++)
			display[i][j] = 0;
}

// Функция заполнения матрицы
void SetMatrix()
{
	// Предварительная очистка виртуального экрана
	clean();

	for (int i = 0; i < height; i++)
		for (int j = 0; j < width; j++)
			display[i][j] = (int)(matrix[i][j]);
}

// Функция вывода
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

    /*
    // Show console
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
            if (matrix[i][j])
                cout << "*";
            else
                cout << " ";
                    
        cout << endl;
    }
    */
    
}

using namespace GLOBAL;

void loop()
{        
    // Button and Axis
    // botton joystick
    static bool left(false);// Сдвиг куба влево
    static bool right(false);// Сдвиг куба вправо
    static bool up(false);// Сдвиг куба вверх
    static bool down(false);// Сдвиг куба вниз
    static bool rotx(false);// Поворот вокруг оси X
    static bool roty(false);// Поворот вокруг оси Y
    static bool rotz(false);// Поворот воруг оси Z
    static bool plus(false);// Увеличение масштаба
    static bool minus(false);// Уменьшение масштаба
    static bool select(false);// Смена режима ввода комманд
    static bool start(false);// Начать "игру"

    do
    {
	if(joystick.sample(&event))
		if ((event.isButton() || event.isAxis()) && (event.value!=0))
			{                        
				switch (event.number)
				{
					case 0:
					{
						switch (event.value)
						{
							case -32767:
                            {
								left = true;
								break;
                            }
							case 32767:
                            {
								right = true;
								break;
                            }
							case 1:
                            {
								rotx = true;
                            }
						}

                        break;
					}

					case 1:
					{
						switch (event.value)
						{
							case -32767:
							{
								up = true;
								break;
							}
							case 32767:
							{
								down = true;
								break;
							}
							case 1:
							{
								rotz = true;
							}
                        }

                    break;
					}

					case 3:
					{
						if (event.value == 1)
							roty = true;
                        
						break;
					}

					case 4:
					{
						if (event.value == 1)
							minus = true;

                        break;
					}

					case 5:
					{
						if (event.value == 1)
							plus = true;

                        break;
					}
                                        
                                        case 8:
                                        {
                                            if (event.value == 1)
                                            {
                                                (select) ? (select = false) : (select = true);
                                                
                                                cout << endl << "Activate console" << endl;
                                            }
                                            
                                            break;
                                        }
                                        
                                        case 9:
                                        {
                                            if (event.value == 1 && !start)
                                               start = true;
                                        }
				}
				
		}

		else
		{
			left = right = up = down = rotx = roty = rotz = plus = minus = false;
		}
        
        } while(!start);
        
	// Преобразования куба
	Compute();

	// Отрисовка куба
	DrawPix();

	// Заполнение виртуальной матрицы
	SetMatrix();

	// Вывод виртуального экранчика на светодиоды
	show();
            
	int ch;
        static const int sizeMessage = 100;
	char message[sizeMessage] = "\0";
    
	if (select)
        {
                cout << endl << "Enter command: ";
		cin >> message;
        }
        
	if ((GLOBAL::comparison(message, (char*)"left") || GLOBAL::comparison(message, (char*)"Left") || GLOBAL::comparison(message, (char*)"LEFT")) || left)
		ch = LEFT;

	else if ((GLOBAL::comparison(message, (char*)"left_inf") || GLOBAL::comparison(message, (char*)"Left_inf") || GLOBAL::comparison(message, (char*)"LEFT_INF")))
		ch = LEFT_INF;

	else if ((GLOBAL::comparison(message, (char*)"right") || GLOBAL::comparison(message, (char*)"Right") || GLOBAL::comparison(message, (char*)"RIGHT")) || right)
		ch = RIGHT;

	else if ((GLOBAL::comparison(message, (char*)"right_inf") || GLOBAL::comparison(message, (char*)"Right_inf") || GLOBAL::comparison(message, (char*)"RIGHT_INF")))
		ch = RIGHT_INF;

	else if ((GLOBAL::comparison(message, (char*)"up") || GLOBAL::comparison(message, (char*)"Up") || GLOBAL::comparison(message, (char*)"UP")) || up)
		ch = UP;

	else if ((GLOBAL::comparison(message, (char*)"up_inf") || GLOBAL::comparison(message, (char*)"Up_inf") || GLOBAL::comparison(message, (char*)"UP_INF")))
		ch = UP_INF;

	else if ((GLOBAL::comparison(message, (char*)"down") || GLOBAL::comparison(message, (char*)"Down") || GLOBAL::comparison(message, (char*)"DOWN")) || down)
		ch = DOWN;

	else if ((GLOBAL::comparison(message, (char*)"down_inf") || GLOBAL::comparison(message, (char*)"Down_inf") || GLOBAL::comparison(message, (char*)"DOWN_INF")))
		ch = DOWN_INF;

	else if ((GLOBAL::comparison(message, (char*)"plus") || GLOBAL::comparison(message, (char*)"Plus") || GLOBAL::comparison(message, (char*)"PLUS")) || plus)
		ch = PLUS;

	else if ((GLOBAL::comparison(message, (char*)"plus_inf") || GLOBAL::comparison(message, (char*)"Plus_inf") || GLOBAL::comparison(message, (char*)"PLUS_INF")))
		ch = PLUS_INF;

	else if ((GLOBAL::comparison(message, (char*)"minus") || GLOBAL::comparison(message, (char*)"Minus") || GLOBAL::comparison(message, (char*)"MINUS")) || minus)
		ch = MINUS;

	else if ((GLOBAL::comparison(message, (char*)"minus_inf") || GLOBAL::comparison(message, (char*)"Minus_inf") || GLOBAL::comparison(message, (char*)"MINUS_INF")))
		ch = MINUS_INF;

	else if ((GLOBAL::comparison(message, (char*)"rotx") || GLOBAL::comparison(message, (char*)"Rotx") || GLOBAL::comparison(message, (char*)"ROTX")) || rotx)
		ch = ROTX;

	else if ((GLOBAL::comparison(message, (char*)"rotx_inf") || GLOBAL::comparison(message, (char*)"Rotx_inf") || GLOBAL::comparison(message, (char*)"ROTX_INF")))
		ch = ROTX_INF;

	else if ((GLOBAL::comparison(message, (char*)"roty") || GLOBAL::comparison(message, (char*)"Roty") || GLOBAL::comparison(message, (char*)"ROTY")) || roty)
		ch = ROTY;

	else if ((GLOBAL::comparison(message, (char*)"roty_inf") || GLOBAL::comparison(message, (char*)"Roty_inf") || GLOBAL::comparison(message, (char*)"ROTY_INF")))
		ch = ROTY_INF;

	else if ((GLOBAL::comparison(message, (char*)"rotz") || GLOBAL::comparison(message, (char*)"Rotz") || GLOBAL::comparison(message, (char*)"ROTZ")) || rotz)
		ch = ROTZ;

	else if ((GLOBAL::comparison(message, (char*)"rotz_inf") || GLOBAL::comparison(message, (char*)"Rotz_inf") || GLOBAL::comparison(message, (char*)"ROTZ_INF")))
		ch = ROTZ_INF;

	else if ((GLOBAL::comparison(message, (char*)"esc") || GLOBAL::comparison(message, (char*)"Esc") || GLOBAL::comparison(message, (char*)"ESC")) || GLOBAL::comparison(message, (char*)"!consol") || GLOBAL::comparison(message, (char*)"!console"))
		ch = ESC;
        else if (GLOBAL::comparison(message, (char*)"restart"))
            ch = 100;

        else
        ch = -1;

	switch (ch)
	{
		case LEFT:
		{
			Commandos::UP();
			break;
		}
		case RIGHT:
		{
			Commandos::DOWN();
			break;
		}
		case UP:
		{
			Commandos::LEFT();
			break;
		}
		case DOWN:
		{
			Commandos::RIGHT();
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
			while(true)
			{
				Commandos::ROTX();
				
				Compute();
				
				DrawPix();
				
				SetMatrix();
				
				show();
			}

			break;
		}
		case ROTY_INF:
		{
			while(true)
			{
				Commandos::ROTY();
				
				Compute();
				
				DrawPix();
				
				SetMatrix();
				
				show();
			}

			break;
		}
		case ROTZ_INF:
		{
			while(true)
			{
				Commandos::ROTZ();
				
				Compute();
				
				DrawPix();
				
				SetMatrix();
				
				show();
			}

			break;
		}
		case LEFT_INF:
		{
			while(true)
			{
				Commandos::UP();
				
				Compute();
				
				DrawPix();
				
				SetMatrix();
				
				show();
			}

			break;
		}
		case RIGHT_INF:
		{
			while(true)
			{
				Commandos::DOWN();
				
				Compute();
				
				DrawPix();
				
				SetMatrix();
				
				show();
			}

			break;
		}
		case UP_INF:
		{
			while(true)
			{
				Commandos::LEFT();
				
				Compute();
				
				DrawPix();
				
				SetMatrix();
				
				show();
			}

			break;
		}
		case DOWN_INF:
		{
			while(true)
			{
				Commandos::RIGHT();
				
				Compute();
				
				DrawPix();
				
				SetMatrix();
				
				show();
			}

			break;
		}
		case MINUS_INF:
		{
			while(true)
			{
				Commandos::MINUS();
				
				Compute();
				
				DrawPix();
				
				SetMatrix();
				
				show();
			}

			break;
		}
		case PLUS_INF:
		{
			while(true)
			{
				Commandos::PLUS();
				
				Compute();
				
				DrawPix();
				
				SetMatrix();
				
				show();
			}

			break;
		}
                
                case ESC:
                {
                    select = false;
                    
                    cout << endl << "Activate joystick" << endl;
                    
                    break;
                }
                
                case 100:
                {
                    start = false;
                    
                    setup();
                    
                    system("clear");
                    
                    break;
                }
	}
}
