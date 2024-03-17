/*开发日志
1.创建项目
2.先导入素材
3.设计c++的模块
就是设计类

Block 表示方块
Tetris 表示俄罗斯方块游戏相关的

4.设计模块接口的主要接口

5.启动游戏
*/


#include"Tetris.h"


int main(void){

	Tetris game(20,10,263,133,36);
	game.play();
	return 0;
}