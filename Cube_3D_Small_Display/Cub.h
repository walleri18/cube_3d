#pragma once
#include <string>

/*
	�������� ��������� ����� ��� ����������� �����������
*/
const int widthDisplay = 16;// ������� ������� (��� � ��� ����)
const int heightDisplay = 16;// ������ (��� ���� =) )

/*
	������� ����������� ���� (��� desktop ������ ���������)
*/
enum MessageID : int
{
	codeHelp,
	codeChangesObject,
	codeExit
};

// ������� �� �������� ������ � ����������� ���� ���������
MessageID discernmentMessage(std::string);

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

