#pragma once

/*
	�������� ��������� ����� ��� ����������� �����������
*/
const int widthDisplay = 16;// ������� ������� (��� � ��� ����)
const int heightDisplay = 16;// ������ (��� ���� =) )

/*
	����� ���� � ������� ����� ��� �������� ���� � ������ ��� ��� �����������
*/

class Cub
{
// ������
private:

	// ������ ������ ����
	int sizeCathetus;

	// ���� �������� �� XY (� ��������)
	int degreenXY;

	// ���� �������� �� XZ (� ��������)
	int degreenXZ;

	// ���������� �� ������ �� ���� (zoom)
	int zoom;

	// ������� �����������
	bool matrix[heightDisplay][widthDisplay];

// ����������������
public:
	Cub();
	Cub(int sizeCathetus, int degreenXY, int degreenXZ, int zoom);

private:

	// ��������� ���� �� �������
	void paintCub();

	// ������� �������
	void cleanMatrix();
};

