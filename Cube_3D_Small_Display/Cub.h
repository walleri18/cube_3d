#pragma once

/*
	Класс Куба в котором будут все свойства куба и методы для его отображения
*/

class Cub
{
// Данные
private:

	// Размеры экрана
	int width;// Ширина (или столбцы)
	int height;// Высота (или строки)

	// Размер катета куба
	int sizeCathetus;

	// Угол поворота по XY (в градусах)
	int degreenXY;

	// Угол поворота по XZ (в градусах)
	int degreenXZ;

	// Расстояние от экрана до куба (zoom)
	int zoom;

	// Матрица отображения
	bool **matrix;

// Функциональность
public:
	Cub();
	Cub(int sizeCathetus, int degreenXY, int degreenXZ, int zoom);
	~Cub();

private:

	// Создание матрицы
	void createMatrix();

	// Отрисовка куба на матрице
	void paintCub();

	// Очистка матрицы
	void cleanMatrix();
};

