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
	void retate();//旋转
	void draw(int leftMargin,int topMargin);//花的方块的左右方向
	static IMAGE** getImages();
	//重载赋值运算符
	Block& operator=(const Block& other);
	bool blockInMap(const vector<vector<int>>&map);
	//Point* getSmallBlocks();
	void solidify(vector<vector<int>>& map);

	int getBlockType();
	void rotate();//旋转
private:
	int blockType;
	//结构体数据用来存储要展示的4个小方块的位置
	Point smallBlocks[4];
	IMAGE* img;
	static IMAGE* imgs[7];
	static int size;


};

