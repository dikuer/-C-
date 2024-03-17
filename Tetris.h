#pragma once
#include<graphics.h>
#include<vector>
#include"Block.h"
using  namespace std;
class Tetris
{
public:

	//���캯��,������Ϸ�����������,������ֵ������Ϸ��ľ���
	Tetris(int rows,int cols,int left,int top,int blockSize);
	void init();
	void play();//��ʼ��Ϸ
private:
	//�û�����
	void keyEvent();

	//��Ⱦ
	void updateWindow();

	//���ؾ�����һ�ε��øú���,����˶���ʱ��(ms)
	//��һ�ε��øú���,����0
	int getDelay();
	void drop();//ʱ�䵽��,�������½�
	void clearLine();
	void moveLeftRight(int offset);
	void rotate();//��ת
	void  drawScore();//���Ƶ�ǰ����
	void checkOver();//�����Ϸ�Ƿ����
	void saveScore();//������߷�
	void displayOver();//������Ϸ��������
private:
	int delay;
	bool update;//�Ƿ����

	int rows;
	int cols;
	int leftMargin;
	int topMargin;
	int blockSize;
	//0:�հ�,û���κη���
	//5:�ǵ����ֶ���˹����;
	vector<vector<int>> map;
	IMAGE imgBg;


	Block* curBlock;
	Block* nextBlock;//Ԥ�淽��
	Block bakBlock;//��ǰ���齵�������,����������һ���Ϸ�λ�õ�

	int score;
	int level;//��ǰ�ؿ�
	int lineCount;//��ǰ�Ѿ������˶�����
	int hightstScore;//��߷�
	bool gameOver;//��Ϸ�Ƿ����
	IMAGE imgOver;
	IMAGE imgWin;
};





