#include "Tetris.h"
#include<iostream>
#include<time.h>
#include<stdlib.h>
#include"Block.h"
#include<conio.h>
#include<mmsystem.h>
#include<fstream>
using namespace std;
#define MAX_LEVEL 5
#define RECORED_FILE  "recorder.txt"
#pragma comment(lib,"winmm.lib")
//const int SPEED_NORMAL = 500;//正常速度500ms
const int SPEED_NORMAL[MAX_LEVEL] = { 500,300,150,100,80 };
const int SPEED_QUICK = 30;
Tetris::Tetris(int rows, int cols, int left, int top,int blockSize)
{
	this->rows = rows;
	this->cols = cols;
	this->leftMargin = left;
	this->topMargin = top;
	this->blockSize = blockSize;

	for (int i = 0; i < rows; i++) {
		vector<int> mapRow;//创建一维数组
		for (int j = 0; j < cols; j++) {

			mapRow.push_back(0);
		}
		map.push_back(mapRow);//将一维数组添加到二维数组中
	}
	
}

void Tetris::init()
{
	//播放背景音乐
	mciSendString("play res/bg.mp3 repeat", 0, 0, 0);
	delay = SPEED_NORMAL[0];
	//配置随机种子
	srand(time(NULL));


	initgraph(938, 896);//创建游戏窗口

	//加载背景图片
	loadimage(&imgBg, "res//bg2.png");
	//加载游戏开始和结束画面
	//loadimage(&imgBg, "res/win.png");
	loadimage(&imgOver, "res/over.png");
	//初始化游戏区的数据
	char data[20][10];
	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < cols; j++) {
			map[i][j] = 0;
		}
	}


	score = 0;
	lineCount = 0;
	level = 1;


	//初始化最高分

	ifstream file(RECORED_FILE);
	if (!file.is_open()) {
		cout << RECORED_FILE << "打开失败" << endl;
		hightstScore = 0;
	}
	else {
		file >> hightstScore;
	}
	file.close();


	gameOver = false;
}

void Tetris::play()
{
	//游戏初始化,一个成员函数调用另外一个成员函数可以直接掉
	init();
	
	nextBlock = new Block;//生成预告方块
	curBlock = nextBlock;//生成当前方块
	nextBlock = new Block;//生成下一个方块
	int timer = 0;

	while (true) {
		
		//1.接受用户输入
		keyEvent();

		//距离上一次循环经理的时间
		timer += getDelay();
		if (timer > delay) {
			//定时器清零,重新开始及时
			timer = 0;

			//方块下降,更新游戏
			drop();
			update = true;
			
		}

		if (update) {
			//渲染
			update = false;
			updateWindow();
			//更改游戏数据
			clearLine();
		}

		if (gameOver) {
			//保存分数
			saveScore();

			//更新游戏结束界面
			displayOver();

			system("pause");
			init();//重新开局
		}
	}

}

void Tetris::keyEvent()
{
	unsigned char ch;
	bool rotateFlag = false;
	int dx = 0;
	if (_kbhit()) {
		//有按键输入,读取用户输入
		ch =_getch();


		//如果按下方向键,会自动返回两个字符
		//如果按下 向上方向键,会先后返回:224 72
		//如果按下 向下方向键,会先后返回:224 80
		//如果按下 向左移动,会先后返回:224 75
		//如果按下 向右移动,会先后返回:224 77
		if (ch == 224) {
			ch = _getch();
			switch (ch) {
			case 72:
				//旋转处理
				rotateFlag = true;
				break;
			case 80:
				//快速下降
				delay = SPEED_QUICK;
				break;
			case 75:
				dx = -1;
				break;
			case 77:
				dx = 1;
				break;
			default:
				break;
			}
		}
	}
	
	if (rotateFlag) {
		//旋转
		rotate();
		update = true;
	}

	if (dx != 0) {
		//实现左右移动
		moveLeftRight(dx);
		update = true;
	}
}

void Tetris::updateWindow()
{
	//放置背景图像
	putimage(0, 0, &imgBg);

	IMAGE** imgs = Block::getImages();

	BeginBatchDraw();
	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < cols; j++) {

			if (map[i][j] == 0)continue;

			int x = j * blockSize + leftMargin;
			int y = i * blockSize + topMargin;
			putimage(x, y, imgs[map[i][j] -1 ]);
		}
	}


	curBlock->draw(leftMargin, topMargin);
	nextBlock->draw(689, 150);


	drawScore();//绘制分数
	EndBatchDraw();
}

//第一次调用,返回0;
//返回距离上一次调用,间隔了多少ms
int Tetris::getDelay()
{
	static unsigned long long lastTime = 0;
	//获取当前的时钟数
	unsigned long long currentTime = GetTickCount();


	if (lastTime == 0) {
		lastTime = currentTime;
		return 0;
	}
	else {
		int	ret  = currentTime - lastTime;
		lastTime = currentTime;
		return ret;
	}
	
}

void Tetris::drop()
{
	bakBlock = *curBlock;
	curBlock->drop();

	//检查降落位置是否合法
	if (!curBlock->blockInMap(map)) {
		//方块到底了,不能下降了,把这个方块"固化"
		bakBlock.solidify(map);
		delete curBlock;
		curBlock = nextBlock;
		nextBlock = new Block;

		//检查游戏是否结束
		checkOver();

	}

	//快速下降一次后,速度恢复
	delay = SPEED_NORMAL[level-1];
}

void Tetris::clearLine()
{
	int k = rows - 1;//存储数据的行数
	int lines = 0;
	for (int i = rows - 1; i >= 0; i--) {
		//检查第i行是否满行
		int count = 0;
		for (int j = 0; j < cols; j++) {
			if (map[i][j]) {
				count++;
			}
			map[k][j] = map[i][j];//一边扫描一边存储
		}

		if (count < cols) {//不是满行

			k--;
		}
		else {//count == cols 满行
			lines++;
		}

		
	}

	if (lines > 0) {
		//计算得分

		int addScore[4] = { 10,30,60,80 };
		score += addScore[lines - 1];


		mciSendString("play res/xiaochu1.mp3",0,0,0);//播放音效
		update = true;


		//每100分一个级别,0-100 第一关  101-200第2关
		level = (score+99) / 100;
		if (level > MAX_LEVEL) {

			//游戏过关
			gameOver = true;
		}
		lineCount += lines;


	}

}

void Tetris::moveLeftRight(int offset)
{
	//备份当前方块
	bakBlock = *curBlock;
	curBlock->moveLeftRight(offset);

	if (!curBlock->blockInMap(map)) {
		//当前位置不和法,还原
		*curBlock = bakBlock;
	}
}

void Tetris::rotate()
{
	if (curBlock->getBlockType() == 7) return;

	bakBlock = *curBlock;
	curBlock->rotate();

	if (!curBlock->blockInMap(map)) {
		//当前位置不和法,还原
		*curBlock = bakBlock;
	}
}

void Tetris::drawScore()
{

	char scoreText[32];
	//打印份数
	sprintf_s(scoreText, sizeof(scoreText), "%d", score);
	
	//设置颜色
	setcolor(RGB(180, 180, 180));

	LOGFONT f;
	gettextstyle(&f);//获取当前的字体
	f.lfHeight = 55;
	f.lfWidth = 30;
	f.lfQuality = ANTIALIASED_QUALITY;//设置字体抗锯齿效果
	strcpy_s(f.lfFaceName, sizeof(f.lfFaceName), _T("Segoe UI Black"));
	settextstyle(&f);



	//设置透明背景
	setbkmode(TRANSPARENT);
	//显示分数
	outtextxy(670, 727, scoreText);



	//打印消去了多少行
	sprintf_s(scoreText, sizeof(scoreText), "%d", lineCount);
	gettextstyle(&f);
	int xPos = 224 - f.lfWidth * strlen(scoreText);
	outtextxy(xPos, 817, scoreText);


	//绘制当前第几关
	sprintf_s(scoreText, sizeof(scoreText), "%d", level);
	outtextxy(224-30, 727, scoreText);


	//当前最高分

	sprintf_s(scoreText, sizeof(scoreText), "%d", hightstScore);
	outtextxy(670, 817, scoreText);

}

void Tetris::checkOver()
{
	gameOver  = (curBlock->blockInMap(map) == false);


}

void Tetris::saveScore()
{
	if (score > hightstScore) {
		hightstScore = score;

		ofstream file(RECORED_FILE);
		file << hightstScore;
		file.close();
	}
}

void Tetris::displayOver()
{
	mciSendString("stop res/bg.mp3", 0, 0, 0);


	//胜利结束,还是失败结束

	if (level <= MAX_LEVEL) {
		putimage(262,361,&imgOver);
		mciSendString("play res/over.mp3", 0, 0, 0);

	}
	else {
		putimage(262, 361, &imgWin);
		mciSendString("play res/win.mp3", 0, 0, 0);
	}
}




