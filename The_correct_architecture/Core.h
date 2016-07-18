#ifndef _CORE_H_

#define _CORE_H_

// Математическое округление
// Просто было лень искать где-то чужое
#define CEIL(X) ((int)(X + 0.5))
#define ABS(X) (((X) > 0) ? ((X)) : (-(X)))

// width - столбцы, ширина
// height - строки, высота
#define width 16
#define height 16

// Перечисление с доступными типами команд
enum MessageID
{
	ESC, // Закончить программу
	LEFT, // Сдвинуть куб влево
	RIGHT, // Сдвинуть куб вправо
	UP, // Сдвинуть куб вверх
	DOWN, // Сдвинуть куб вниз
	ROTX, // Повернуть куб вокруг оси X
	ROTY, // Повернуть куб вокруг оси Y
	ROTZ, // Повернуть куб вокруг оси Z
	PLUS, // Увеличить масштаб
	MINUS, // Уменьшить масштаб
	ROTX_INF, // Бесконечное кручение куба вокруг оси X
	ROTY_INF, // Бесконечное кручение куба вокруг оси Y
	ROTZ_INF, // Бесконечное кручение куба вокруг оси Z
	LEFT_INF, 
	RIGHT_INF,
	UP_INF,
	DOWN_INF,
	PLUS_INF,
	MINUS_INF
};

// Пространство имён с главными глобальными "объектами"
namespace Turov_Vitaly 
{
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
}

extern bool matrix[height][width];

// Глобальное пространство имён со всеми функциями построения
namespace GLOBAL 
{
    int len(char*);

	// Сравнение слов (так как не корректно работает strcmp)
	bool comparison(char*, char *);

	// Очистка матрицы отображения
	void clearMatrix();

	// Отрисовка линии в матрице
	void line(int, int, int, int);

	/*
		Вычисление фронтальной геометрической (изометрической) проекции
		трехмерных точек на двумерную плоскость
	*/
	void Perspect();

	/*
		Использование аналитической геометрии. Хоть где-то она пригодилась)
	*/
	// 3-D преобразования в 2D
	void Compute();

	/*
		Это маленький костыль, но зато пришлось меньше мучаться.
		Объясню на защите)
	*/
	// Функция для приведения "здоровых размеров" куба к размеру нашего экрана
	void bringingScreenSize();

	/*
		Наша главная функция в которой происходить ("магия") рисование
	*/
	// Рисование кубика
	void DrawPix();
}

/*
	Это наш интерфейс, который мы предоставляем для дальнейшего отображения кубика 
*/
// Пространство имён с командами
namespace Commandos 
{
	// Функция для сдвига влево
	void LEFT();

	// Функция для сдвига вправо
	void RIGHT();

	// Функция для сдвига вверх
	void UP();

	// Функция для сдвига вниз
	void DOWN();

	// Функция для поворота по оси X
	void ROTX();

	// Функция для поворота по оси Y
	void ROTY();

	// Функция для поворота по оси Z
	void ROTZ();

	// Функция для увеличения масштаба
	void PLUS();

	// Функция для уменьшения масштаба
	void MINUS();

	// Функция бесконечного вращения по X
	void ROTX_INF();

	// Функция бесконечного вращения по Y
	void ROTY_INF();

	// Функция бесконечного вращения по Z
	void ROTZ_INF();

	// Функция бесконечного смещения влево
	void LEFT_INF();

	// Функция бесконечного смещения вправо
	void RIGHT_INF();

	// Функция бесконечного смещения вверх
	void UP_INF();

	// Функция бесконечного смещения вниз
	void DOWN_INF();

	// Функция бесконечного уменьшения масштаба
	void MINUS_INF();

	// Функция бесконечного увеличения масштаба
	void PLUS_INF();
}

#endif
