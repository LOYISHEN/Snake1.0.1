//filename:main.c
//author:PenG

#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <stdbool.h>
#include <Windows.h>
#include <time.h>

//#include "output.h"

//定义地图的宽和高
#define WIDTH 26
#define HEIGHT 20

//定义地图中节点类型
#define ISEMPTY 0
#define ISBODY 1
#define ISFOOD 2
#define ISWALL 3

//定义四个方向控制键和一个暂停键
#define KEY_UP '8'
#define KEY_DOWN '2'
#define KEY_LEFT '4'
#define KEY_RIGHT '6'
#define KEY_SUSPEND '5'

//定义墙，蛇，食物和背景的颜色
#define FOOD_COLOR 0x0060
#define SNAKE_BODY_COLOR 0x0010
#define SNAKE_HEAD_COLOR 0x0040
#define WALL_COLOR	0x00f0
#define BG_COLOR 0x0000

//定义游戏速度
const int GAME_SPEED = 24;

//定义转向的方向
enum D { UP, RIGHT, DOWN, LEFT };
//定义当前方向，用于移动转向时比较
enum D tempD = RIGHT;

typedef struct snake   //存储身体信息
{
	int x;  //身体节点 x 坐标
	int y;  //身体节点 y 坐标
	struct snake *last;  //上一身体节点地址
}snake;

//地图记录，只需要记录一个矩形就好了
typedef struct game_map
{
	int startX;
	int startY;
	int endX;
	int endY;
}Map;
Map map = {0, 0, WIDTH-1, HEIGHT-1};

//定义蛇和食物的指针，食物的结构其实跟蛇身体的结构一样 
snake *tail, *body, *head, *food;

//上一尾巴位置
snake last_tail;

int snake_length = 3;  //蛇的长度，初始化为3


HANDLE hOut;
COORD pos = { 0,0 };
CONSOLE_CURSOR_INFO cci;    //定义结构体












//启动游戏
void start(void);

//初始化蛇
void initSnake(void);

////把蛇的信息写到地图中
//void writeSnake(void);

////放一个食物
//void initFood(void);

////初始化墙
//void initWall(void);

//清除地图上蛇的身体
void cleanSnake(void);

//处理方向，返回正确方向
enum D judgeDirection(enum D direction);

//清除多余的输入
void cleanInput(void);

//获取输入的方向
enum D getDirection(void);

//判断有没有吃到食物，吃到了就增加长度
//吃到食物返回 true，否则返回 false
bool eatFood(enum D direction);

//把map数组的数据显示出来
//这里要改一下
//void printMap(void);

//移动蛇（注意不要让蛇向后走了）
void moveSnake(enum D direction);

//检测有没有撞到墙
bool knockWall(void);

//检测有没有撞到自己的身体
bool biteSelf(void);

//释放蛇的内存
void freeSnake(void);

//画墙
//新版中一次游戏只需要画一次墙
void printWall(void);

//画蛇
void printSnake(void);

//画食物
void printFood(void);

//初始化输出设置
void initOutput(void);

//设置颜色
void setColor(int color);

//设置光标位置
void setPos(int x, int y);

//输出一个带颜色值的像素（两个字符）
void printPix(int x, int y, int color);












int main(void)
{
	initOutput();
	start();

	return 0;
}

//启动游戏
void start(void)
{
	int size = (WIDTH - 2) * (HEIGHT - 2);

	initSnake();
	printFood();
	//initWall();
	printWall();
	while (1)
	{
		//clrscr();
		//清屏
		//system("cls");

		
		cleanSnake();
		tempD = judgeDirection(getDirection());
		if (eatFood(tempD))
		{
			printSnake();
			snake_length++;

			if (snake_length >= size)
			{
				//printMap();
				//printSnake();
				break;
			}

			printFood();
		}
		else
		{

			//在这里控制蛇的走向
			moveSnake(tempD);

		}
		printSnake();

		//撞到墙或者咬到自己就重新开始
		if (knockWall() || biteSelf())
		{
			cleanInput();
			Sleep(1);
			setPos(0, map.endY + 1);
			setColor(0x000f);
			printf("\nYou lose!!");
			printf("\nPress any key to play again...");
			_getch();
			//clrscr();
			system("cls");
			cleanSnake();
			freeSnake();
			//map[food->y][food->x] = ISEMPTY;
			free(food);
			initSnake();
			printFood();
			printWall();
			tempD = RIGHT;
			snake_length = 3;
			printWall();
		}  //if  end
		//Sleep(50 / GAME_SPEED);
		Sleep(100);

	}  //while (main cycle) end
	   //clrscr();
	printf("WoW!!\nYou win!!");
}

//初始化蛇
void initSnake(void)
{
	//蛇初始的位置
	int x, y;
	x = 2;
	y = 1;

	//获取内存空间
	tail = (snake *)malloc(sizeof(snake));
	body = (snake *)malloc(sizeof(snake));
	head = (snake *)malloc(sizeof(snake));

	//把位置信息写入链表中
	tail->x = x - 1;
	tail->y = y;
	tail->last = body;
	body->x = x;
	body->y = y;
	body->last = head;
	head->x = x + 1;
	head->y = y;
	head->last = NULL;

	printSnake();
}

//判断该点是否已经避开蛇的位置，避开返回真
bool avoidSnake(int x, int y)
{
	body = tail;
	while (body != NULL)
	{
		if (body->x == x && body->y == y)
		{
			return false;
		}
		body = body->last;
	}
	return true;
}

//打印蛇
void printSnake(void)
{
	body = tail;	//不能动tail
	while (body != NULL)
	{
		if (body == head)
		{
			printPix(body->x, body->y, SNAKE_HEAD_COLOR);
		}
		else
		{
			printPix(body->x, body->y, SNAKE_BODY_COLOR);
		}
		
		body = body->last;
	}  //while  end
}

//记录并打印一个食物
void printFood(void)
{
	int tx, ty;
	food = (snake *)malloc(sizeof(snake));

	//获取一个随机的位置，而且要避开墙和蛇的位置
	srand((unsigned)time(NULL));
	do
	{
		tx = (rand() % (WIDTH - 2)) + 1;
		ty = (rand() % (HEIGHT - 2)) + 1;
	} while (!avoidSnake(tx, ty)	//避开蛇
				|| tx == 0 || ty == 0 || tx == map.endX || ty == map.endY);	//避开墙

	food->x = tx;
	food->y = ty;
	food->last = NULL;
	printPix(tx, ty, FOOD_COLOR);
}

//画墙
void printWall(void)
{
	//画上下边缘
	for (int i = 0; i < WIDTH; i++)
	{
		printPix(i, 0, WALL_COLOR);
		printPix(i, map.endY, WALL_COLOR);
	}
	//画左右边缘，重复画了四个角也没关系
	for (int i = 0; i < HEIGHT; i++)
	{
		printPix(0, i, WALL_COLOR);
		printPix(map.endX, i, WALL_COLOR);
	}
}

//清除地图上蛇的身体
void cleanSnake(void)
{
	body = tail;
	while (body != NULL)
	{
		printPix(body->x, body->y, BG_COLOR);
		body = body->last;
	}  //while  end
}

//处理方向，返回正确方向
enum D judgeDirection(enum D direction)
{
	switch (direction)
	{
		case LEFT:
		case RIGHT:
			if (tempD != LEFT && tempD != RIGHT)
			{
				return direction;
			}
			else
			{
				return tempD;
			}
			break;

		case UP:
		case DOWN:
			if (tempD != UP && tempD != DOWN)
			{
				return direction;
			}
			else
			{
				return tempD;
			}
			break;

		default:
			printf("error! at judgeDirection()");
			//assert(0);
			break;
	}  //switch  end
}

//清除多余的输入
void cleanInput(void)
{
	while (_kbhit())
	{
		_getch();
	}
}

//获取输入的方向
enum D getDirection(void)
{
	while (_kbhit())
	{
		switch (getch())
		{
			case KEY_UP:
				cleanInput();
				return UP;
				break;

			case KEY_DOWN:
				cleanInput();
				return DOWN;
				break;

			case KEY_LEFT:
				cleanInput();
				return LEFT;
				break;

			case KEY_RIGHT:
				cleanInput();
				return RIGHT;
				break;

			/*default:
				printf("error! in getDirection()");
				assert(0);
				break;
			*/
		}  //switch  end
	}  //while  end

	return tempD;
}

//判断有没有吃到食物，吃到了就增加长度
//吃到食物返回 true，否则返回 flase
bool eatFood(enum D direction)
{
	enum D t_dire = direction;

	int tx, ty;
	tx = head->x;
	ty = head->y;

	switch (t_dire)
	{
		case UP:
			ty--;
			break;

		case DOWN:
			ty++;
			break;

		case LEFT:
			tx--;
			break;

		case RIGHT:
			tx++;
			break;

		default:
			printf("error! in addSnake()");
			//assert(0);
			break;
	}  //switch end

	//判断此时是否吃到食物
	if (food->x == tx && food->y == ty)
	{
		head->last = food;
		head = head->last;
		return true;
		//printf("\nadd\n");
	}
	//return FALSE;
	return false;
}

//把map数组的数据显示出来
//这里要改一下
//void printMap2(void)
//{
//	for (int y = 0; y<HEIGHT; y++)
//	{
//		for (int x = 0; x<WIDTH; x++)
//		{
//			switch (map[y][x])
//			{
//				case ISWALL:
//					printf("##");
//					break;
//
//				case ISEMPTY:
//					printf("  ");
//					break;
//
//				case ISBODY:
//					if (y == head->y && x == head->x)
//					{
//						printf("@@");
//					}
//					else if (y == tail->y && x == tail->x)
//					{
//						printf("oo");
//					}
//					else
//					{
//						printf("OO");
//					}
//					break;
//
//				case ISFOOD:
//					printf("$$");
//					break;
//				default:
//					NULL;
//					break;
//			}
//		}  //inside for  end
//		printf("\n");
//	}  //outside for  end
//}

//移动蛇（注意不要让蛇向后走了）
void moveSnake(enum D direction)
{
	int tx, ty;

	tempD = judgeDirection(direction);

	//尾推法移动蛇
	body = tail;	//下面要用到tail
	last_tail.x = tail->x;
	last_tail.y = tail->y;
	while (body->last != NULL)
	{
		body = body->last;
	}  //while  end
	//重复利用空间，别浪费了！
	//下面这几行顺序不能变
	head->last = tail;
	tail = tail->last;
	head = head->last;
	head->last = NULL;

	//下面只需要根据方向设置蛇头的位置就行了
	//此时body指向head后第一个body
	tx = body->x;
	ty = body->y;
	switch (tempD)
	{
		case UP:
			ty--;
			break;

		case DOWN:
			ty++;
			break;

		case LEFT:
			tx--;
			break;

		case RIGHT:
			tx++;
			break;

		default:
			printf("error! at moveSnake()");
			//assert(0);
			break;
	}  //switch  end
	head->x = tx;
	head->y = ty;

	//printSnake();
}

//检测有没有撞到墙
bool knockWall(void)
{
	int tx = head->x, ty = head->y;
	if (tx == 0 || tx == (WIDTH - 1) || ty == 0 || ty == (HEIGHT - 1))
	{
		return true;
	}
	else
	{
		return false;
	}
}

//检测有没有撞到自己的身体
bool biteSelf(void)
{
	int tx = head->x, ty = head->y;
	body = tail;
	while (body->last != NULL)
	{
		if (body->x == tx && body->y == ty)
		{
			return true;
		}
		body = body->last;
	}  //while  end
	return false;
}

//释放蛇的内存
void freeSnake(void)
{
	body = tail;
	while (body != NULL)
	{
		tail = tail->last;
		free(body);
		body = tail;
	}  //while  end
	//free(tail);
	tail = NULL;
	body = NULL;
	head = NULL;
}



//初始化输出设置
void initOutput(void)
{
	hOut = GetStdHandle(STD_OUTPUT_HANDLE);

	GetConsoleCursorInfo(hOut, &cci);    //获取光标信息
	cci.dwSize = 1;    //设置光标大小
	cci.bVisible = 0; //设置光标不可见 FALSE
	SetConsoleCursorInfo(hOut, &cci);    //设置(应用)光标信息
	//SetConsoleTextAttribute(hOut, 0x0004 | 0x0008 | 0x8000);    //设置字体属性
	pos.X = 0;
	pos.Y = 0;
	SetConsoleCursorPosition(hOut, pos);    //设置光标坐标
}

//设置颜色
void setColor(int color)
{
	SetConsoleTextAttribute(hOut, color);
}

//设置光标位置
void setPos(int x, int y)
{
	pos.X = x;
	pos.Y = y;
	SetConsoleCursorPosition(hOut, pos);
}

//输出一个带颜色值的像素（两个字符）
void printPix(int x, int y, int color)
{
	setPos(2 * x, y);
	setColor(color);
	printf("  ");
}