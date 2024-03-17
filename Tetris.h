#pragma once
#include<graphics.h>
#include<vector>
#include"Block.h"
using  namespace std;
class Tetris
{
public:

	//构造函数,设置游戏画面的行列数,方块出现的左侧和上方的距离
	Tetris(int rows,int cols,int left,int top,int blockSize);
	void init();
	void play();//开始游戏
private:
	//用户输入
	void keyEvent();

	//渲染
	void updateWindow();

	//返回距离上一次调用该函数,间隔了多少时间(ms)
	//第一次调用该函数,返回0
	int getDelay();
	void drop();//时间到了,方块向下降
	void clearLine();
	void moveLeftRight(int offset);
	void rotate();//旋转
	void  drawScore();//绘制当前分数
	void checkOver();//检查游戏是否结束
	void saveScore();//保存最高分
	void displayOver();//绘制游戏结束画面
private:
	int delay;
	bool update;//是否更新

	int rows;
	int cols;
	int leftMargin;
	int topMargin;
	int blockSize;
	//0:空白,没有任何方块
	//5:是第五种俄罗斯方块;
	vector<vector<int>> map;
	IMAGE imgBg;


	Block* curBlock;
	Block* nextBlock;//预告方块
	Block bakBlock;//当前方块降落过程中,用来备份上一个合法位置的

	int score;
	int level;//当前关卡
	int lineCount;//当前已经消除了多少行
	int hightstScore;//最高分
	bool gameOver;//游戏是否结束
	IMAGE imgOver;
	IMAGE imgWin;
};





