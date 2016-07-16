#include <iostream>
#include "Core.h"

using namespace std;

// Прототип функции вывода кубика на консоль
void show(Core&);

int main(void)
{
	// Инициализация расчётного модуля
	Core cub;

	// Начало выводящего модуля
	while (true)
	{
		// Начальная отрисовка
		cub.DrawPix();

		// Вывод на консоль
		show(cub);

		int ch(INT_MAX);   // код клавиши
		char message[100] = "\0";

		do
		{
			std::cin >> message;

			if ((Core::comparison(message, "left") || Core::comparison(message, "Left") || Core::comparison(message, "LEFT")))
				ch = LEFT;

			else if ((Core::comparison(message, "left_inf") || Core::comparison(message, "Left_inf") || Core::comparison(message, "LEFT_INF")))
				ch = LEFT_INF;

			else if ((Core::comparison(message, "right") || Core::comparison(message, "Right") || Core::comparison(message, "RIGHT")))
				ch = RIGHT;

			else if ((Core::comparison(message, "right_inf") || Core::comparison(message, "Right_inf") || Core::comparison(message, "RIGHT_INF")))
				ch = RIGHT_INF;

			else if ((Core::comparison(message, "up") || Core::comparison(message, "Up") || Core::comparison(message, "UP")))
				ch = UP;

			else if ((Core::comparison(message, "up_inf") || Core::comparison(message, "Up_inf") || Core::comparison(message, "UP_INF")))
				ch = UP_INF;

			else if ((Core::comparison(message, "down") || Core::comparison(message, "Down") || Core::comparison(message, "DOWN")))
				ch = DOWN;

			else if ((Core::comparison(message, "down_inf") || Core::comparison(message, "Down_inf") || Core::comparison(message, "DOWN_INF")))
				ch = DOWN_INF;

			else if ((Core::comparison(message, "plus") || Core::comparison(message, "Plus") || Core::comparison(message, "PLUS")))
				ch = PLUS;

			else if ((Core::comparison(message, "plus_inf") || Core::comparison(message, "Plus_inf") || Core::comparison(message, "PLUS_INF")))
				ch = PLUS_INF;

			else if ((Core::comparison(message, "minus") || Core::comparison(message, "Minus") || Core::comparison(message, "MINUS")))
				ch = MINUS;

			else if ((Core::comparison(message, "minus_inf") || Core::comparison(message, "Minus_inf") || Core::comparison(message, "MINUS_INF")))
				ch = MINUS_INF;

			else if ((Core::comparison(message, "rotx") || Core::comparison(message, "Rotx") || Core::comparison(message, "ROTX")))
				ch = ROTX;

			else if ((Core::comparison(message, "rotx_inf") || Core::comparison(message, "Rotx_inf") || Core::comparison(message, "ROTX_INF")))
				ch = ROTX_INF;

			else if ((Core::comparison(message, "roty") || Core::comparison(message, "Roty") || Core::comparison(message, "ROTY")))
				ch = ROTY;

			else if ((Core::comparison(message, "roty_inf") || Core::comparison(message, "Roty_inf") || Core::comparison(message, "ROTY_INF")))
				ch = ROTY_INF;

			else if ((Core::comparison(message, "rotz") || Core::comparison(message, "Rotz") || Core::comparison(message, "ROTZ")))
				ch = ROTZ;

			else if ((Core::comparison(message, "rotz_inf") || Core::comparison(message, "Rotz_inf") || Core::comparison(message, "ROTZ_INF")))
				ch = ROTZ_INF;

			else if ((Core::comparison(message, "esc") || Core::comparison(message, "Esc") || Core::comparison(message, "ESC")))
				ch = ESC;

			switch (ch)
			{
				case LEFT:
				{
					cub.LEFT();
					break;
				}
				case RIGHT:
				{
					cub.RIGHT();
					break;
				}
				case UP:
				{
					cub.UP();
					break;
				}
				case DOWN:
				{
					cub.DOWN();
					break;
				}
				case PLUS:
				{
					cub.PLUS();
					break;
				}
				case MINUS:
				{
					cub.MINUS();

					break;
				}
				case ROTX:
				{
					cub.ROTX();

					break;
				}
				case ROTY:
				{
					cub.ROTY();

					break;
				}
				case ROTZ:
				{
					cub.ROTZ();

					break;
				}
				case ROTX_INF:
				{
					for (int i = 0; i < 100; i++)
					{
						cub.ROTX();

						cub.DrawPix();

						show(cub);
					}

					break;
				}
				case ROTY_INF:
				{
					for (int i = 0; i < 100; i++)
					{
						cub.ROTY();

						cub.DrawPix();

						show(cub);
					}

					break;
				}
				case ROTZ_INF:
				{
					for (int i = 0; i < 100; i++)
					{
						cub.ROTZ();

						cub.DrawPix();

						show(cub);
					}

					break;
				}
				case LEFT_INF:
				{
					for (int i = 0; i < 100; i++)
					{
						cub.LEFT();

						cub.DrawPix();

						show(cub);
					}

					break;
				}
				case RIGHT_INF:
				{
					for (int i = 0; i < 100; i++)
					{
						cub.RIGHT();

						cub.DrawPix();

						show(cub);
					}

					break;
				}
				case UP_INF:
				{
					for (int i = 0; i < 100; i++)
					{
						cub.UP();

						cub.DrawPix();

						show(cub);
					}

					break;
				}
				case DOWN_INF:
				{
					for (int i = 0; i < 100; i++)
					{
						cub.DOWN();

						cub.DrawPix();

						show(cub);
					}

					break;
				}
				case MINUS_INF:
				{
					for (int i = 0; i < 100; i++)
					{
						cub.MINUS();

						cub.DrawPix();

						show(cub);
					}

					break;
				}
				case PLUS_INF:
				{
					for (int i = 0; i < 100; i++)
					{
						cub.PLUS();

						cub.DrawPix();

						show(cub);
					}

					break;
				}
			}

			cub.DrawPix();

			show(cub);

		} while (ch != ESC);
	}

	return 0;
}

void show(Core &core)
{
	// Очистка монитора
	system("cls");

	for (int i = 0; i < core.getHeight(); i++)
	{
		for (int j = 0; j < core.getWidth(); j++)
			if (core.getMatrix()[i][j])
				cout << "*";
			else
				cout << " ";

		cout << endl;
	}
}