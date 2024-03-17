#pragma once
#include<graphics.h>
#include<vector>
using namespace std;
struct Point {
	int row;
	int col;

};

class Block
{

public:

	Block();
	void drop();
	void moveLeftRight(int offset);
	void retate();//��ת
	void draw(int leftMargin,int topMargin);//���ķ�������ҷ���
	static IMAGE** getImages();
	//���ظ�ֵ�����
	Block& operator=(const Block& other);
	bool blockInMap(const vector<vector<int>>&map);
	//Point* getSmallBlocks();
	void solidify(vector<vector<int>>& map);

	int getBlockType();
	void rotate();//��ת
private:
	int blockType;
	//�ṹ�����������洢Ҫչʾ��4��С�����λ��
	Point smallBlocks[4];
	IMAGE* img;
	static IMAGE* imgs[7];
	static int size;


};

