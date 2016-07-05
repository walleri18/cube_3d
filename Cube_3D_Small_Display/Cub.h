#pragma once

/*
	����� ���� � ������� ����� ��� �������� ���� � ������ ��� ��� �����������
*/

class Cub
{
// ������
private:

	// ������� ������
	int width;// ������ (��� �������)
	int height;// ������ (��� ������)

	// ������ ������ ����
	int sizeCathetus;

	// ���� �������� �� XY (� ��������)
	int degreenXY;

	// ���� �������� �� XZ (� ��������)
	int degreenXZ;

	// ���������� �� ������ �� ���� (zoom)
	int zoom;

	// ������� �����������
	bool **matrix;

// ����������������
public:
	Cub();
	Cub(int sizeCathetus, int degreenXY, int degreenXZ, int zoom);
	~Cub();

private:

	// �������� �������
	void createMatrix();

	// ��������� ���� �� �������
	void paintCub();

	// ������� �������
	void cleanMatrix();
};

