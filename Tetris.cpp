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
//const int SPEED_NORMAL = 500;//�����ٶ�500ms
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
		vector<int> mapRow;//����һά����
		for (int j = 0; j < cols; j++) {

			mapRow.push_back(0);
		}
		map.push_back(mapRow);//��һά������ӵ���ά������
	}
	
}

void Tetris::init()
{
	//���ű�������
	mciSendString("play res/bg.mp3 repeat", 0, 0, 0);
	delay = SPEED_NORMAL[0];
	//�����������
	srand(time(NULL));


	initgraph(938, 896);//������Ϸ����

	//���ر���ͼƬ
	loadimage(&imgBg, "res//bg2.png");
	//������Ϸ��ʼ�ͽ�������
	//loadimage(&imgBg, "res/win.png");
	loadimage(&imgOver, "res/over.png");
	//��ʼ����Ϸ��������
	char data[20][10];
	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < cols; j++) {
			map[i][j] = 0;
		}
	}


	score = 0;
	lineCount = 0;
	level = 1;


	//��ʼ����߷�

	ifstream file(RECORED_FILE);
	if (!file.is_open()) {
		cout << RECORED_FILE << "��ʧ��" << endl;
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
	//��Ϸ��ʼ��,һ����Ա������������һ����Ա��������ֱ�ӵ�
	init();
	
	nextBlock = new Block;//����Ԥ�淽��
	curBlock = nextBlock;//���ɵ�ǰ����
	nextBlock = new Block;//������һ������
	int timer = 0;

	while (true) {
		
		//1.�����û�����
		keyEvent();

		//������һ��ѭ�������ʱ��
		timer += getDelay();
		if (timer > delay) {
			//��ʱ������,���¿�ʼ��ʱ
			timer = 0;

			//�����½�,������Ϸ
			drop();
			update = true;
			
		}

		if (update) {
			//��Ⱦ
			update = false;
			updateWindow();
			//������Ϸ����
			clearLine();
		}

		if (gameOver) {
			//�������
			saveScore();

			//������Ϸ��������
			displayOver();

			system("pause");
			init();//���¿���
		}
	}

}

void Tetris::keyEvent()
{
	unsigned char ch;
	bool rotateFlag = false;
	int dx = 0;
	if (_kbhit()) {
		//�а�������,��ȡ�û�����
		ch =_getch();


		//������·����,���Զ����������ַ�
		//������� ���Ϸ����,���Ⱥ󷵻�:224 72
		//������� ���·����,���Ⱥ󷵻�:224 80
		//������� �����ƶ�,���Ⱥ󷵻�:224 75
		//������� �����ƶ�,���Ⱥ󷵻�:224 77
		if (ch == 224) {
			ch = _getch();
			switch (ch) {
			case 72:
				//��ת����
				rotateFlag = true;
				break;
			case 80:
				//�����½�
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
		//��ת
		rotate();
		update = true;
	}

	if (dx != 0) {
		//ʵ�������ƶ�
		moveLeftRight(dx);
		update = true;
	}
}

void Tetris::updateWindow()
{
	//���ñ���ͼ��
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


	drawScore();//���Ʒ���
	EndBatchDraw();
}

//��һ�ε���,����0;
//���ؾ�����һ�ε���,����˶���ms
int Tetris::getDelay()
{
	static unsigned long long lastTime = 0;
	//��ȡ��ǰ��ʱ����
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

	//��齵��λ���Ƿ�Ϸ�
	if (!curBlock->blockInMap(map)) {
		//���鵽����,�����½���,���������"�̻�"
		bakBlock.solidify(map);
		delete curBlock;
		curBlock = nextBlock;
		nextBlock = new Block;

		//�����Ϸ�Ƿ����
		checkOver();

	}

	//�����½�һ�κ�,�ٶȻָ�
	delay = SPEED_NORMAL[level-1];
}

void Tetris::clearLine()
{
	int k = rows - 1;//�洢���ݵ�����
	int lines = 0;
	for (int i = rows - 1; i >= 0; i--) {
		//����i���Ƿ�����
		int count = 0;
		for (int j = 0; j < cols; j++) {
			if (map[i][j]) {
				count++;
			}
			map[k][j] = map[i][j];//һ��ɨ��һ�ߴ洢
		}

		if (count < cols) {//��������

			k--;
		}
		else {//count == cols ����
			lines++;
		}

		
	}

	if (lines > 0) {
		//����÷�

		int addScore[4] = { 10,30,60,80 };
		score += addScore[lines - 1];


		mciSendString("play res/xiaochu1.mp3",0,0,0);//������Ч
		update = true;


		//ÿ100��һ������,0-100 ��һ��  101-200��2��
		level = (score+99) / 100;
		if (level > MAX_LEVEL) {

			//��Ϸ����
			gameOver = true;
		}
		lineCount += lines;


	}

}

void Tetris::moveLeftRight(int offset)
{
	//���ݵ�ǰ����
	bakBlock = *curBlock;
	curBlock->moveLeftRight(offset);

	if (!curBlock->blockInMap(map)) {
		//��ǰλ�ò��ͷ�,��ԭ
		*curBlock = bakBlock;
	}
}

void Tetris::rotate()
{
	if (curBlock->getBlockType() == 7) return;

	bakBlock = *curBlock;
	curBlock->rotate();

	if (!curBlock->blockInMap(map)) {
		//��ǰλ�ò��ͷ�,��ԭ
		*curBlock = bakBlock;
	}
}

void Tetris::drawScore()
{

	char scoreText[32];
	//��ӡ����
	sprintf_s(scoreText, sizeof(scoreText), "%d", score);
	
	//������ɫ
	setcolor(RGB(180, 180, 180));

	LOGFONT f;
	gettextstyle(&f);//��ȡ��ǰ������
	f.lfHeight = 55;
	f.lfWidth = 30;
	f.lfQuality = ANTIALIASED_QUALITY;//�������忹���Ч��
	strcpy_s(f.lfFaceName, sizeof(f.lfFaceName), _T("Segoe UI Black"));
	settextstyle(&f);



	//����͸������
	setbkmode(TRANSPARENT);
	//��ʾ����
	outtextxy(670, 727, scoreText);



	//��ӡ��ȥ�˶�����
	sprintf_s(scoreText, sizeof(scoreText), "%d", lineCount);
	gettextstyle(&f);
	int xPos = 224 - f.lfWidth * strlen(scoreText);
	outtextxy(xPos, 817, scoreText);


	//���Ƶ�ǰ�ڼ���
	sprintf_s(scoreText, sizeof(scoreText), "%d", level);
	outtextxy(224-30, 727, scoreText);


	//��ǰ��߷�

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


	//ʤ������,����ʧ�ܽ���

	if (level <= MAX_LEVEL) {
		putimage(262,361,&imgOver);
		mciSendString("play res/over.mp3", 0, 0, 0);

	}
	else {
		putimage(262, 361, &imgWin);
		mciSendString("play res/win.mp3", 0, 0, 0);
	}
}




