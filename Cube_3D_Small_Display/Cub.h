#pragma once

/*
	Основные константы экран для отображения изображения
*/
const int widthDisplay = 16;// столбцы матрицы (это я для себя)
const int heightDisplay = 16;// строки (для себя =) )

/*
	Класс Куба в котором будут все свойства куба и методы для его отображения
*/

class Cub
{
// Данные
private:

	// Размер катета куба
	int sizeCathetus;

	// Угол поворота по XY (в градусах)
	int degreenXY;

	// Угол поворота по XZ (в градусах)
	int degreenXZ;

	// Расстояние от экрана до куба (zoom)
	int zoom;

	// Матрица отображения
	bool matrix[heightDisplay][widthDisplay];

// Функциональность
public:
	Cub();
	Cub(int sizeCathetus, int degreenXY, int degreenXZ, int zoom);

private:

	// Отрисовка куба на матрице
	void paintCub();

	// Очистка матрицы
	void cleanMatrix();
};

