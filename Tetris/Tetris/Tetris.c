#include"Tetris.h"

#define KEY_PRESS(VK) ( (GetAsyncKeyState(VK) & 0x1) ? 1 : 0 )

//设置命令提示符
void SetCmd()
{
	system("mode con cols=150 lines=40");//设置命令窗口大小150,40
	system("title 俄罗斯方块");//设置窗口标题
}

//设置光标
void Set_Cursor()
{
	HANDLE Houtput = GetStdHandle(STD_OUTPUT_HANDLE);//获取标准输出的句柄(⽤来标识不同设备的数值)

	//隐藏光标
	CONSOLE_CURSOR_INFO CursorInfo;
	GetConsoleCursorInfo(Houtput, &CursorInfo);//获取控制台光标信息
	CursorInfo.bVisible = false;//隐藏光标
	SetConsoleCursorInfo(Houtput, &CursorInfo);//设置控制台光标状态
}

//封装控制光标坐标函数
void SetPos(short x, short y)
{
	COORD pos = { x,y };
	HANDLE Houtput = NULL;
	Houtput = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleCursorPosition(Houtput, pos);
}

//切换到本地环境（为了打印宽字符■）
void SetLoc()
{
	setlocale(LC_ALL, "");//切换到本地环境
}

//打印游戏菜单
void Menu()
{
	SetPos(62,8);
	printf("欢迎来到俄罗斯方块小游戏");
	SetPos(65, 30);
	system("pause");
	system("cls");
	SetPos(60, 8);
	printf("按‘↑’可使方块顺时针旋转");
	SetPos(60, 9);
	printf("按‘↓’可使方块直接触底。");
	SetPos(60, 10);
	printf("按‘←’可使方块向左移动一格。");
	SetPos(60, 11);
	printf("按‘→’可使方块向右移动一格。");
	SetPos(65, 30);
	system("pause");
	system("cls");
}

//创建地图
void CreatMap()
{
	int i = 0;
	//打印上方墙壁
	SetPos(0, 0);
	for (i = 0; i <= (width * 2 - 2); i += 2)
	{
		wprintf(L"%lc", WALL);
	}
	//打印下方墙壁
	SetPos(0, height - 1);
	for (i = 0; i <= (width * 2 - 2); i += 2)
	{
		wprintf(L"%lc", WALL);
	}
	//打印左边墙壁
	SetPos(0, 1);
	for (i = 0; i <= height - 3; i++)
	{
		wprintf(L"%lc\n", WALL);
	}
	//打印右边墙壁
	for (i = 1; i < height - 1; i++)
	{
		SetPos(width * 2 - 2, i);
		wprintf(L"%lc\n", WALL);
	}
}

//初始化二维数组（有实体填‘0’，无实体填‘1’）
void InitMatrix(int matrix[height][width * 2])
{
	int i = 0;
	int j = 0;
	for (i = 1; i <= height - 2; i++)
	{
		for (j = 2; j <= width * 2 - 4; j += 2)
		{
			matrix[i][j] = 1;
		}
	}
}

//游戏结束
void GameOver()
{
	system("cls");
	printf("游戏结束，要再玩一局吗？(Y/N):");
}

//将触底方块设置为边界
void Verge(pBlock ps, int matrix[height][width * 2])//检测并刷新边界
{
	if (ps->GStatus == Stop || ps->GStatus == END)
	{
		pBlockNode cur = ps->Block1;
		while (cur)
		{
			int i = cur->y;
			int j = cur->x;
			if (i <= 0)
			{
				ps->GStatus = END;
			}
			matrix[i][j] = 0;
			cur = cur->next;
		}
	}
}

//将维护触底方块的指针由"Falling"更改为"Stop"
void ChangeStatus(pBlock ps, int matrix[height][width * 2])
{
	pBlockNode cur = ps->Block1;
	while (cur)
	{
		if (matrix[(cur->y) + 1][cur->x] == 0)
		{
			ps->GStatus = Stop;
		}
		cur = cur->next;
	}
}

//游戏暂停
void pause()//暂停
{
	while (1)
	{
		Sleep(300);
		if (KEY_PRESS(VK_SPACE))
		{
			break;
		}
	}
}

//检测按键
void CheckKey(pBlock ps)
{
	if (KEY_PRESS(VK_DOWN))
	{
		ps->Dir = DOWN;
	}
	else if (KEY_PRESS(VK_UP))
	{
		ps->Dir = ROTE;
	}
	else if (KEY_PRESS(VK_LEFT))
	{
		ps->Dir = LEFT;
	}
	else if (KEY_PRESS(VK_RIGHT))
	{
		ps->Dir = RIGHT;
	}
	else if (KEY_PRESS(VK_SPACE))
	{
		pause();
	}
	else if (KEY_PRESS(VK_ESCAPE))
	{
		ps->GStatus = END;
	}
}

//检测到某一行全为实体时，消除该行（二维数组某一行偶数列全为‘0’，将改行以上数字整体下移，并在最顶行填充‘1’）
void Remove_Wall(pBlock ps, int matrix[height][width * 2])
{
	int i = 0;
	int j = 0;
	int count = 0;
	int tem = 0;
	//一次最多消除4行，故循环4次可保证全部消除
	for (int loop = 0; loop < 4; loop++)
	{
		for (i = height - 2; i >= 1; i--)
		{
			count = 0;
			for (j = 2; j <= width * 2 - 4; j += 2)
			{
				if (matrix[i][j] != 0)
				{
					count++;
				}
			}
			if (count == 0)
			{
				//tem记录可消除的行号
				tem = i;
				goto next;
			}
		}
	next:
		if (count == 0)
		{
			//将检测到的可消除行以上行整体向下移动
			for (tem; tem >= 2; tem--)
			{
				for (j = 2; j <= width * 2 - 4; j += 2)
				{
					matrix[tem][j] = matrix[tem - 1][j];
				}
			}
			//在第一行填充1
			for (j = 2; j <= width * 2 - 4; j += 2)
			{
				matrix[1][j] = 1;
			}
			ps->Score += 10;
		}
	}
}

//刷新打印
void Print_Wall(pBlock ps, int matrix[height][width * 2])
{
	int i = 0;
	int j = 0;
	for (i = 1; i <= height - 2; i++)
	{
		for (j = 2; j <= width * 2 - 4; j += 2)
		{
			//数组‘0’处打印‘WALL’
			if (matrix[i][j] == 0)
			{
				SetPos(j, i);
				wprintf(L"%lc", WALL);
			}
			//数组‘1’处打印两个空格（消除）
			if (matrix[i][j] == 1)
			{
				SetPos(j, i);
				printf("  ");
			}
		}
	}
	SetPos(30, 7);
	printf("  ");
	SetPos(32, 7);
	printf("  ");
	SetPos(34, 7);
	printf("  ");
	SetPos(30, 8);
	printf("  ");
	SetPos(32, 8);
	printf("  ");
	SetPos(34, 8);
	printf("  ");
	SetPos(36, 8);
	printf("  ");
}

//创建并打印不同的结构体（只详细注释CreatBlock_1，其余基本相同）
void CreatBlock_1(pBlock ps)
{
	pBlockNode cur = NULL;
	//结构体由4个方块组成，故循环4次
	for (int i = 0; i < 4; i++)
	{
		//为结构体开辟空间
		cur = (pBlockNode)malloc(sizeof(BlockNode));
		//开辟空间失败则打印错误信息
		if (cur == NULL)
		{
			perror("malloc()");
			return;
		}
		// 设置坐标
		cur->next = NULL;
		cur->x = (width - 2) + i * 2;//横向向右延伸
		cur->y = 0;
		//形成单链表
		if (i != 3)
		{
			if (ps->Block1 == NULL)
			{
				ps->Block1 = cur;
			}
			else
			{
				cur->next = ps->Block1;
				ps->Block1 = cur;
			}
		}
		//最后一个方块较特殊，手动设置
		else
		{
			cur->x -= 2;
			cur->y = -1;
			cur->next = ps->Block1;
			ps->Block1 = cur;
		}
		//打印方块
		cur = ps->Block1;
		while (cur)
		{
			if ((ps->Block1->y) > 0)
			{
				SetPos(cur->x, cur->y);
				wprintf(L"%lc", WALL);
			}
			cur = cur->next;
		}
	}
	//初始化方块1
	ps->GStatus = Falling;
}

void CreatBlock_2(pBlock ps)
{
	pBlockNode cur = NULL;
	for (int i = 4; i > 0; i--)
	{
		cur = (pBlockNode)malloc(sizeof(BlockNode));
		if (cur == NULL)
		{
			perror("malloc()");
			return;
		}
		// 设置坐标
		cur->next = NULL;
		cur->x = (width - 6) + i * 2;
		cur->y = 0;
		if (i != 1)
		{
			if (ps->Block1 == NULL)
			{
				ps->Block1 = cur;
			}
			else
			{
				cur->next = ps->Block1;
				ps->Block1 = cur;
			}
		}
		else
		{
			cur->x += 2;
			cur->y = -1;
			cur->next = ps->Block1;
			ps->Block1 = cur;
		}
		//打印方块
		cur = ps->Block1;
		while (cur)
		{
			if ((ps->Block1->y) > 0)
			{
				SetPos(cur->x, cur->y);
				wprintf(L"%lc", WALL);
			}
			cur = cur->next;
		}
	}
	//初始化方块1
	ps->GStatus = Falling;
}

void CreatBlock_3(pBlock ps)
{
	pBlockNode cur = NULL;
	int pos = 0;
	for (int i = 4; i > 0; i--)
	{
		cur = (pBlockNode)malloc(sizeof(BlockNode));
		if (cur == NULL)
		{
			perror("malloc()");
			return;
		}
		// 设置坐标
		cur->next = NULL;
		cur->x = (width - 6) + i * 2;
		cur->y = 0;
		if (i != 1)
		{
			if (ps->Block1 == NULL)
			{
				ps->Block1 = cur;
			}
			else
			{
				cur->next = ps->Block1;
				ps->Block1 = cur;
			}
		}
		else
		{
			cur->x += 4;
			cur->y = -1;
			cur->next = ps->Block1;
			ps->Block1 = cur;
		}
		//打印方块
		cur = ps->Block1;
		while (cur)
		{
			if ((ps->Block1->y) > 0)
			{
				SetPos(cur->x, cur->y);
				wprintf(L"%lc", WALL);
			}
			cur = cur->next;
		}
	}
	//初始化方块1
	ps->GStatus = Falling;
}

void CreatBlock_4(pBlock ps)
{
	pBlockNode cur = NULL;
	int pos = 0;
	for (int i = 0; i < 4; i++)
	{
		cur = (pBlockNode)malloc(sizeof(BlockNode));
		if (cur == NULL)
		{
			perror("malloc()");
			return;
		}
		// 设置坐标
		cur->next = NULL;
		cur->x = (width - 4) + i * 2;
		cur->y = 0;
		if (ps->Block1 == NULL)
		{
			ps->Block1 = cur;
		}
		else
		{
			cur->next = ps->Block1;
			ps->Block1 = cur;
		}
		//打印方块
		cur = ps->Block1;
		while (cur)
		{
			if ((ps->Block1->y) > 0)
			{
				SetPos(cur->x, cur->y);
				wprintf(L"%lc", WALL);
			}
			cur = cur->next;
		}
	}
	//初始化方块1
	ps->GStatus = Falling;
}

void CreatBlock_5(pBlock ps)
{
	pBlockNode cur = NULL;
	for (int i = 0; i < 4; i++)
	{
		cur = (pBlockNode)malloc(sizeof(BlockNode));
		if (cur == NULL)
		{
			perror("malloc()");
			return;
		}
		// 设置坐标
		cur->next = NULL;
		cur->x = (width - 2) + i * 2;
		cur->y = 0;
		if (i < 2)
		{
			if (ps->Block1 == NULL)
			{
				ps->Block1 = cur;
			}
			else
			{
				cur->next = ps->Block1;
				ps->Block1 = cur;
			}
		}
		if(i == 2)
		{
			cur->x -= 2;
			cur->y = -1;
			cur->next = ps->Block1;
			ps->Block1 = cur;
		}
		if (i == 3)
		{
			cur->x -= 6;
			cur->y = -1;
			cur->next = ps->Block1;
			ps->Block1 = cur;
		}
		//打印方块
		cur = ps->Block1;
		while (cur)
		{
			if ((ps->Block1->y) > 0)
			{
				SetPos(cur->x, cur->y);
				wprintf(L"%lc", WALL);
			}
			cur = cur->next;
		}
	}
	//初始化方块1
	ps->GStatus = Falling;
}

void CreatBlock_6(pBlock ps)
{
	pBlockNode cur = NULL;
	for (int i = 0; i < 4; i++)
	{
		cur = (pBlockNode)malloc(sizeof(BlockNode));
		if (cur == NULL)
		{
			perror("malloc()");
			return;
		}
		// 设置坐标
		cur->next = NULL;
		cur->x = (width) + i * 2;
		cur->y = 0;
		if (i < 2)
		{
			if (ps->Block1 == NULL)
			{
				ps->Block1 = cur;
			}
			else
			{
				cur->next = ps->Block1;
				ps->Block1 = cur;
			}
		}
		if (i == 2)
		{
			cur->x -= 4;
			cur->y = -1;
			cur->next = ps->Block1;
			ps->Block1 = cur;
		}
		if (i == 3)
		{
			cur->x -= 8;
			cur->y = -1;
			cur->next = ps->Block1;
			ps->Block1 = cur;
		}
		//打印方块
		cur = ps->Block1;
		while (cur)
		{
			if ((ps->Block1->y) > 0)
			{
				SetPos(cur->x, cur->y);
				wprintf(L"%lc", WALL);
			}
			cur = cur->next;
		}
	}
	//初始化方块1
	ps->GStatus = Falling;
}

void CreatBlock_7(pBlock ps)
{
	pBlockNode cur = NULL;
	int pos = 0;
	for (int i = 0; i < 4; i++)
	{
		cur = (pBlockNode)malloc(sizeof(BlockNode));
		if (cur == NULL)
		{
			perror("malloc()");
			return;
		}
		// 设置坐标
		cur->next = NULL;
		cur->x = (width - 2) + i * 2;
		cur->y = 0;
		if (i < 2)
		{
			if (ps->Block1 == NULL)
			{
				ps->Block1 = cur;
			}
			else
			{
				cur->next = ps->Block1;
				ps->Block1 = cur;
			}
		}
		if (i == 2)
		{
			cur->x -= 2;
			cur->y = -1;
			cur->next = ps->Block1;
			ps->Block1 = cur;
		}
		if (i == 3)
		{
			cur->x -= 2;
			cur->y = -1;
			cur->next = ps->Block1;
			ps->Block1 = cur;
		}
		//打印方块
		cur = ps->Block1;
		while (cur)
		{
			if ((ps->Block1->y) > 0)
			{
				SetPos(cur->x, cur->y);
				wprintf(L"%lc", WALL);
			}
			cur = cur->next;
		}
	}
	//初始化方块1
	ps->GStatus = Falling;
}

//控制不同结构体的下落，移动，旋转（只详细注释BlockFalling_1，其余只有旋转逻辑不同）
void BlockFalling_1(pBlock ps, int matrix[height][width * 2])
{
	pBlockNode cur = ps->Block1;
	do
	{
	again: cur = ps->Block1;
		ps->Dir = 0;
		//按键检测函数
		if (ps->Block1 != NULL && ps->Block1->y > 0)
		{
			CheckKey(ps);
		}
		switch (ps->Dir)
		{
		//表示按下‘↓’后，使方块直接触底
		case DOWN:
		{
			//清除原本的打印
			while (cur != NULL)
			{
				SetPos(cur->x, cur->y);
				printf("  ");
				cur = cur->next;
			}
			//先检测是否触底，再向下移动一格，直至触底
			do
			{
				cur = ps->Block1;
				//改变结构体状态
				ChangeStatus(ps, matrix);
				//触底后的操作，即打印结构体
				if (ps->GStatus == Stop)
				{
					while (cur)
					{
						SetPos(cur->x, cur->y);
						wprintf(L"%lc", WALL);
						cur = cur->next;
					}
					Sleep(300);
					ps->GStatus = Falling;
					//这里是为了触底之后依旧可以左右移动方块
					goto again;
				}
				//若未触底的操作，把每个方块下移一格
				cur = ps->Block1;
				while (cur != NULL)
				{
					(cur->y)++;
					cur = cur->next;
				}
				cur = ps->Block1;
			} while (ps->GStatus == Falling);
		}
		//表示按下‘←’后，向左移动一格
		case LEFT:
		{
			cur = ps->Block1;
			while (cur)
			{
				if (matrix[cur->y][(cur->x) - 2] == 0)
				{
					Sleep(300);
					goto again;
				}
				cur = cur->next;
			}
			cur = ps->Block1;
			while (cur != NULL)
			{
					SetPos(cur->x, cur->y);
					printf("  ");
					cur = cur->next;
			}
			cur = ps->Block1;
			while (cur != NULL)
			{
				(cur->x) -= 2;
				SetPos(cur->x, cur->y);
				cur = cur->next;
				wprintf(L"%lc", WALL);
			}
			cur = ps->Block1;
			//这里是为了减缓或者暂停结构体下落给玩家更多反应时间
			Sleep(300);
			goto again;
		}
		//这里表示按下‘→’后，向右边移动一格
		case RIGHT:
		{
			cur = ps->Block1;
			while (cur)
			{
				if (matrix[cur->y][(cur->x) + 2] == 0)
				{
					Sleep(300);
					goto again;
				}
				cur = cur->next;
			}
			cur = ps->Block1;
			while (cur != NULL)
			{
				SetPos(cur->x, cur->y);
				printf("  ");
				cur = cur->next;
			}
			cur = ps->Block1;
			while (cur != NULL)
			{
				(cur->x) += 2;
				SetPos(cur->x, cur->y);
				cur = cur->next;
				wprintf(L"%lc", WALL);
			}
			cur = ps->Block1;
			//这里是为了减缓或者暂停结构体下落给玩家更多反应时间
			Sleep(300);
			goto again;
		}
		case ROTE:
		{
			//记录方块状态（每个方块可以通过4次旋转变回初始状态，故每个方块一共有4种状态，4种旋转逻辑）
			int tem = 0;
		
			if(ps->BStatus == Status_1)
			{
				cur = ps->Block1;
				//检测旋转后是否有与实体重合的部分
				if (matrix[cur->y+2][cur->x] == 0 || matrix[cur->next->y + 1][cur->next->x - 2] == 0 || matrix[cur->next->next->y][cur->next->next->x] == 0 || matrix[cur->next->next->next->y - 1][cur->next->next->next->x + 2] == 0)
				{
					goto again;
				}
				//清除原本的打印
				cur = ps->Block1;
				while (cur != NULL)
				{
					SetPos(cur->x, cur->y);
					printf("  ");
					cur = cur->next;
				}
				cur = ps->Block1;
				//旋转逻辑1
				//1：
				cur->y += 2;
				SetPos(cur->x, cur->y);
				cur = cur->next;
				wprintf(L"%lc", WALL);
				//2：
				cur->x -= 2;
				cur->y += 1;
				SetPos(cur->x, cur->y);
				cur = cur->next;
				wprintf(L"%lc", WALL);
				//3:
				SetPos(cur->x, cur->y);
				cur = cur->next;
				wprintf(L"%lc", WALL);
				//4:
				cur->x += 2;
				cur->y -= 1;
				SetPos(cur->x, cur->y);
				wprintf(L"%lc", WALL);
				cur = ps->Block1;
				ps->Dir = 0;
				tem = 1;
			}
			if(ps->BStatus == Status_2)
			{
				cur = ps->Block1;
				//检测旋转后是否有与实体重合的部分
				if (matrix[cur->y][cur->x - 4] == 0 || matrix[cur->next->y - 1][cur->next->x - 2] == 0 || matrix[cur->next->next->y][cur->next->next->x] == 0 || matrix[cur->next->next->next->y + 1][cur->next->next->next->x + 2] == 0)
				{
					goto again;
				}
				//清除原本的打印
				cur = ps->Block1;
				while (cur != NULL)
				{
					SetPos(cur->x, cur->y);
					printf("  ");
					cur = cur->next;
				}
				cur = ps->Block1;
				//旋转逻辑2
				//1：
				cur->x -= 4;
				SetPos(cur->x, cur->y);
				cur = cur->next;
				wprintf(L"%lc", WALL);
				//2：
				cur->x -= 2;
				cur->y -= 1;
				SetPos(cur->x, cur->y);
				cur = cur->next;
				wprintf(L"%lc", WALL);
				//3:
				SetPos(cur->x, cur->y);
				cur = cur->next;
				wprintf(L"%lc", WALL);
				//4:
				cur->x += 2;
				cur->y += 1;
				SetPos(cur->x, cur->y);
				wprintf(L"%lc", WALL);
				cur = ps->Block1;
				ps->Dir = 0;
				tem = 2;
			}
			if(ps->BStatus == Status_3)
			{
				cur = ps->Block1;
				//检测旋转后是否有与实体重合的部分
				if (matrix[cur->y - 2][cur->x] == 0 || matrix[cur->next->y - 1][cur->next->x + 2] == 0 || matrix[cur->next->next->y][cur->next->next->x] == 0 || matrix[cur->next->next->next->y + 1][cur->next->next->next->x - 2] == 0)
				{
					goto again;
				}
				//清除原本的打印
				cur = ps->Block1;
				while (cur != NULL)
				{
					SetPos(cur->x, cur->y);
					printf("  ");
					cur = cur->next;
				}
				cur = ps->Block1;
				//旋转逻辑3
				//1：
				cur->y -= 2;
				SetPos(cur->x, cur->y);
				cur = cur->next;
				wprintf(L"%lc", WALL);
				//2：
				cur->x += 2;
				cur->y -= 1;
				SetPos(cur->x, cur->y);
				cur = cur->next;
				wprintf(L"%lc", WALL);
				//3:}
				SetPos(cur->x, cur->y);
				cur = cur->next;
				wprintf(L"%lc", WALL);
				//4:
				cur->x -= 2;
				cur->y += 1;
				SetPos(cur->x, cur->y);
				wprintf(L"%lc", WALL);
				cur = ps->Block1;
				ps->Dir = 0;
				tem = 3;
			}
			if(ps->BStatus == Status_4)
			{
				cur = ps->Block1;
				//检测旋转后是否有与实体重合的部分
				if (matrix[cur->y][cur->x + 4] == 0 || matrix[cur->next->y + 1][cur->next->x + 2] == 0 || matrix[cur->next->next->y][cur->next->next->x] == 0 || matrix[cur->next->next->next->y - 1][cur->next->next->next->x - 2] == 0)
				{
					goto again;
				}
				//清除原本的打印
				cur = ps->Block1;
				while (cur != NULL)
				{
					SetPos(cur->x, cur->y);
					printf("  ");
					cur = cur->next;
				}
				cur = ps->Block1;
				//旋转逻辑4
				//1：
				cur->x += 4;
				SetPos(cur->x, cur->y);
				cur = cur->next;
				wprintf(L"%lc", WALL);
				//2：
				cur->x += 2;
				cur->y += 1;
				SetPos(cur->x, cur->y);
				cur = cur->next;
				wprintf(L"%lc", WALL);
				//3:
				SetPos(cur->x, cur->y);
				cur = cur->next;
				wprintf(L"%lc", WALL);
				//4:
				cur->x -= 2;
				cur->y -= 1;
				SetPos(cur->x, cur->y);
				wprintf(L"%lc", WALL);
				cur = ps->Block1;
				ps->Dir = 0;
				tem = 4;
			}
			//更改结构体经过旋转后的状态
			if (tem == 1)
			{
				ps->BStatus = Status_2;
			}
			else if (tem == 2)
			{
				ps->BStatus = Status_3;
			}
			else if (tem == 3)
			{
				ps->BStatus = Status_4;
			}
			else if(tem == 4)
			{
				ps->BStatus = Status_1;
			}
			//这里是为了减缓或者暂停结构体下落给玩家更多反应时间
			Sleep(300);
			goto again;
		}
		}
		cur = ps->Block1;
		if (ps->Block1 != NULL && ps->Block1->y+1 > 0)
		{
			//改变结构体状态
			ChangeStatus(ps, matrix);
			//确定边界
			Verge(ps, matrix);
		}
		if (ps->GStatus == Stop)
		{
			goto end;
		}
		if (ps->GStatus == END)
		{
			break;
		}
		//这里是没有检测到任何按键的正常下落状态
		cur = ps->Block1;
		while (cur != NULL)
		{
			if (cur->y > 0)
			{
				SetPos(cur->x, cur->y);
				printf("  ");
			}
			cur = cur->next;
		}
		cur = ps->Block1;
		while (cur != NULL)
		{
			(cur->y)++;
			SetPos(cur->x, cur->y);
			cur = cur->next;
			wprintf(L"%lc", WALL);
		}
		Sleep(300);
	} while (ps->GStatus == Falling);
	//释放结构体空间（方块触底后成为边界，信息记录在二维数组中，不需要空间存储，直接释放并将结构体初始化为{0}）
end:cur = ps->Block1;
	ps->Dir = 0;
	while (cur != NULL)
	{
		cur = cur->next;
		free(ps->Block1);
		ps->Block1 = cur;
	}
ps->BStatus = Status_1;
}

void BlockFalling_2(pBlock ps, int matrix[height][width * 2])
{
	pBlockNode cur = ps->Block1;
	do
	{
	again: cur = ps->Block1;
		ps->Dir = 0;
		if (ps->Block1 != NULL && ps->Block1->y > 0)
		{
			CheckKey(ps);
		}
		switch (ps->Dir)
		{
		case DOWN:
		{
			while (cur != NULL)
			{
				SetPos(cur->x, cur->y);
				printf("  ");
				cur = cur->next;
			}
			do
			{
				cur = ps->Block1;
				//改变结构体状态
				ChangeStatus(ps, matrix);
				if (ps->GStatus == Stop)
				{
					while (cur)
					{
						SetPos(cur->x, cur->y);
						wprintf(L"%lc", WALL);
						cur = cur->next;
					}
					Sleep(300);
					ps->GStatus = Falling;
					goto again;
				}
				cur = ps->Block1;
				while (cur != NULL)
				{
					(cur->y)++;
					cur = cur->next;
				}
				cur = ps->Block1;
			} while (ps->GStatus == Falling);
		}
		case LEFT:
		{
			cur = ps->Block1;
			while (cur)
			{
				if (matrix[cur->y][(cur->x) - 2] == 0)
				{
					Sleep(300);
					goto again;
				}
				cur = cur->next;
			}
			cur = ps->Block1;
			while (cur != NULL)
			{
				SetPos(cur->x, cur->y);
				printf("  ");
				cur = cur->next;
			}
			cur = ps->Block1;
			while (cur != NULL)
			{
				(cur->x) -= 2;
				SetPos(cur->x, cur->y);
				cur = cur->next;
				wprintf(L"%lc", WALL);
			}
			cur = ps->Block1;
			Sleep(300);
			goto again;
		}
		case RIGHT:
		{
			cur = ps->Block1;
			while (cur)
			{
				if (matrix[cur->y][(cur->x) + 2] == 0)
				{
					Sleep(300);
					goto again;
				}
				cur = cur->next;
			}
			cur = ps->Block1;
			while (cur != NULL)
			{
				SetPos(cur->x, cur->y);
				printf("  ");
				cur = cur->next;
			}
			cur = ps->Block1;
			while (cur != NULL)
			{
				(cur->x) += 2;
				SetPos(cur->x, cur->y);
				cur = cur->next;
				wprintf(L"%lc", WALL);
			}
			cur = ps->Block1;
			Sleep(300);
			goto again;
		}
		case ROTE:
		{
			//记录方块状态
			int tem = 0;

			if (ps->BStatus == Status_1)
			{
				cur = ps->Block1;
				if (matrix[cur->y][cur->x + 4] == 0 || matrix[cur->next->y - 1][cur->next->x + 2] == 0 || matrix[cur->next->next->y][cur->next->next->x] == 0 || matrix[cur->next->next->next->y + 1][cur->next->next->next->x - 2] == 0)
				{
					goto again;
				}
				//清除原本的打印
				cur = ps->Block1;
				while (cur != NULL)
				{
					SetPos(cur->x, cur->y);
					printf("  ");
					cur = cur->next;
				}
				cur = ps->Block1;
				//旋转逻辑1
				//1：
				cur->x += 4;
				SetPos(cur->x, cur->y);
				cur = cur->next;
				wprintf(L"%lc", WALL);
				//2：
				cur->x += 2;
				cur->y -= 1;
				SetPos(cur->x, cur->y);
				cur = cur->next;
				wprintf(L"%lc", WALL);
				//3:
				SetPos(cur->x, cur->y);
				cur = cur->next;
				wprintf(L"%lc", WALL);
				//4:
				cur->x -= 2;
				cur->y += 1;
				SetPos(cur->x, cur->y);
				wprintf(L"%lc", WALL);
				cur = ps->Block1;
				ps->Dir = 0;
				tem = 1;
			}
			if (ps->BStatus == Status_2)
			{
				cur = ps->Block1;
				if (matrix[cur->y + 2][cur->x] == 0 || matrix[cur->next->y + 1][cur->next->x + 2] == 0 || matrix[cur->next->next->y][cur->next->next->x] == 0 || matrix[cur->next->next->next->y - 1][cur->next->next->next->x - 2] == 0)
				{
					goto again;
				}
				//清除原本的打印
				cur = ps->Block1;
				while (cur != NULL)
				{
					SetPos(cur->x, cur->y);
					printf("  ");
					cur = cur->next;
				}
				cur = ps->Block1;
				//旋转逻辑2
				//1：
				cur->y += 2;
				SetPos(cur->x, cur->y);
				cur = cur->next;
				wprintf(L"%lc", WALL);
				//2：
				cur->x += 2;
				cur->y += 1;
				SetPos(cur->x, cur->y);
				cur = cur->next;
				wprintf(L"%lc", WALL);
				//3:
				SetPos(cur->x, cur->y);
				cur = cur->next;
				wprintf(L"%lc", WALL);
				//4:
				cur->x -= 2;
				cur->y -= 1;
				SetPos(cur->x, cur->y);
				wprintf(L"%lc", WALL);
				cur = ps->Block1;
				ps->Dir = 0;
				tem = 2;
			}
			if (ps->BStatus == Status_3)
			{
				cur = ps->Block1;
				if (matrix[cur->y][cur->x - 4] == 0 || matrix[cur->next->y + 1][cur->next->x - 2] == 0 || matrix[cur->next->next->y][cur->next->next->x] == 0 || matrix[cur->next->next->next->y - 1][cur->next->next->next->x + 2] == 0)
				{
					goto again;
				}
				//清除原本的打印
				cur = ps->Block1;
				while (cur != NULL)
				{
					SetPos(cur->x, cur->y);
					printf("  ");
					cur = cur->next;
				}
				cur = ps->Block1;
				//旋转逻辑3
				//1：
				cur->x -= 4;
				SetPos(cur->x, cur->y);
				cur = cur->next;
				wprintf(L"%lc", WALL);
				//2：
				cur->x -= 2;
				cur->y += 1;
				SetPos(cur->x, cur->y);
				cur = cur->next;
				wprintf(L"%lc", WALL);
				//3:
				SetPos(cur->x, cur->y);
				cur = cur->next;
				wprintf(L"%lc", WALL);
				//4:
				cur->x += 2;
				cur->y -= 1;
				SetPos(cur->x, cur->y);
				wprintf(L"%lc", WALL);
				cur = ps->Block1;
				ps->Dir = 0;
				tem = 3;
			}
			if (ps->BStatus == Status_4)
			{
				cur = ps->Block1;
				if (matrix[cur->y - 2][cur->x] == 0 || matrix[cur->next->y - 1][cur->next->x - 2] == 0 || matrix[cur->next->next->y][cur->next->next->x] == 0 || matrix[cur->next->next->next->y + 1][cur->next->next->next->x + 2] == 0)
				{
					goto again;
				}
				//清除原本的打印
				cur = ps->Block1;
				while (cur != NULL)
				{
					SetPos(cur->x, cur->y);
					printf("  ");
					cur = cur->next;
				}
				cur = ps->Block1;
				//旋转逻辑4
				//1：
				cur->y -= 2;
				SetPos(cur->x, cur->y);
				cur = cur->next;
				wprintf(L"%lc", WALL);
				//2：
				cur->x -= 2;
				cur->y -= 1;
				SetPos(cur->x, cur->y);
				cur = cur->next;
				wprintf(L"%lc", WALL);
				//3:
				SetPos(cur->x, cur->y);
				cur = cur->next;
				wprintf(L"%lc", WALL);
				//4:
				cur->x += 2;
				cur->y += 1;
				SetPos(cur->x, cur->y);
				wprintf(L"%lc", WALL);
				cur = ps->Block1;
				ps->Dir = 0;
				tem = 4;
			}
			if (tem == 1)
			{
				ps->BStatus = Status_2;
			}
			else if (tem == 2)
			{
				ps->BStatus = Status_3;
			}
			else if (tem == 3)
			{
				ps->BStatus = Status_4;
			}
			else if (tem == 4)
			{
				ps->BStatus = Status_1;
			}
			Sleep(300);
			goto again;
		}
		}
		cur = ps->Block1;
		if (ps->Block1 != NULL && ps->Block1->y+1 > 0)
		{
			//改变结构体状态
			ChangeStatus(ps, matrix);
			//确定边界
			Verge(ps, matrix);
		}
		if (ps->GStatus == Stop)
		{
			goto end;
		}
		if (ps->GStatus == END)
		{
			break;
		}
		//这里是没有检测到任何按键的正常下落状态
		cur = ps->Block1;
		while (cur != NULL)
		{
			if (cur->y > 0)
			{
				SetPos(cur->x, cur->y);
				printf("  ");
			}
			cur = cur->next;
		}
		cur = ps->Block1;
		while (cur != NULL)
		{
			(cur->y)++;
			SetPos(cur->x, cur->y);
			cur = cur->next;
			wprintf(L"%lc", WALL);
		}
		Sleep(300);
	} while (ps->GStatus == Falling);
end:cur = ps->Block1;
	while (cur != NULL)
	{
		cur = cur->next;
		free(ps->Block1);
		ps->Block1 = cur;
	}
ps->BStatus = Status_1;
}

void BlockFalling_3(pBlock ps, int matrix[height][width * 2])
{
	pBlockNode cur = ps->Block1;
	do
	{
	again: cur = ps->Block1;
		ps->Dir = 0;
		if (ps->Block1 != NULL && ps->Block1->y > 0)
		{
			CheckKey(ps);
		}
		switch (ps->Dir)
		{
		case DOWN:
		{
			while (cur != NULL)
			{
				SetPos(cur->x, cur->y);
				printf("  ");
				cur = cur->next;
			}
			do
			{
				cur = ps->Block1;
				//改变结构体状态
				ChangeStatus(ps, matrix);
				if (ps->GStatus == Stop)
				{
					while (cur)
					{
						SetPos(cur->x, cur->y);
						wprintf(L"%lc", WALL);
						cur = cur->next;
					}
					Sleep(300);
					ps->GStatus = Falling;
					goto again;
				}
				cur = ps->Block1;
				while (cur != NULL)
				{
					(cur->y)++;
					cur = cur->next;
				}
				cur = ps->Block1;
			} while (ps->GStatus == Falling);
		}
		case LEFT:
		{
			cur = ps->Block1;
			while (cur)
			{
				if (matrix[cur->y][(cur->x) - 2] == 0)
				{
					Sleep(300);
					goto again;
				}
				cur = cur->next;
			}
			cur = ps->Block1;
			while (cur != NULL)
			{
				SetPos(cur->x, cur->y);
				printf("  ");
				cur = cur->next;
			}
			cur = ps->Block1;
			while (cur != NULL)
			{
				(cur->x) -= 2;
				SetPos(cur->x, cur->y);
				cur = cur->next;
				wprintf(L"%lc", WALL);
			}
			cur = ps->Block1;
			Sleep(300);
			goto again;
		}
		case RIGHT:
		{
			cur = ps->Block1;
			while (cur)
			{
				if (matrix[cur->y][(cur->x) + 2] == 0)
				{
					Sleep(300);
					goto again;
				}
				cur = cur->next;
			}
			cur = ps->Block1;
			while (cur != NULL)
			{
				SetPos(cur->x, cur->y);
				printf("  ");
				cur = cur->next;
			}
			cur = ps->Block1;
			while (cur != NULL)
			{
				(cur->x) += 2;
				SetPos(cur->x, cur->y);
				cur = cur->next;
				wprintf(L"%lc", WALL);
			}
			cur = ps->Block1;
			Sleep(300);
			goto again;
		}
		case ROTE:
		{
			//记录方块状态
			int tem = 0;

			if (ps->BStatus == Status_1)
			{
				cur = ps->Block1;
				if (matrix[cur->y + 1][cur->x + 2] == 0 || matrix[cur->next->y - 1][cur->next->x + 2] == 0 || matrix[cur->next->next->y][cur->next->next->x] == 0 || matrix[cur->next->next->next->y + 1][cur->next->next->next->x - 2] == 0)
				{
					goto again;
				}
				//清除原本的打印
				cur = ps->Block1;
				while (cur != NULL)
				{
					SetPos(cur->x, cur->y);
					printf("  ");
					cur = cur->next;
				}
				cur = ps->Block1;
				//旋转逻辑1
				//1：
				cur->x += 2;
				cur->y += 1;
				SetPos(cur->x, cur->y);
				cur = cur->next;
				wprintf(L"%lc", WALL);
				//2：
				cur->x += 2;
				cur->y -= 1;
				SetPos(cur->x, cur->y);
				cur = cur->next;
				wprintf(L"%lc", WALL);
				//3:
				SetPos(cur->x, cur->y);
				cur = cur->next;
				wprintf(L"%lc", WALL);
				//4:
				cur->x -= 2;
				cur->y += 1;
				SetPos(cur->x, cur->y);
				wprintf(L"%lc", WALL);
				cur = ps->Block1;
				ps->Dir = 0;
				tem = 1;
			}
			if (ps->BStatus == Status_2)
			{
				cur = ps->Block1;
				if (matrix[cur->y + 1][cur->x - 2] == 0 || matrix[cur->next->y + 1][cur->next->x + 2] == 0 || matrix[cur->next->next->y][cur->next->next->x] == 0 || matrix[cur->next->next->next->y - 1][cur->next->next->next->x - 2] == 0)
				{
					goto again;
				}
				//清除原本的打印
				cur = ps->Block1;
				while (cur != NULL)
				{
					SetPos(cur->x, cur->y);
					printf("  ");
					cur = cur->next;
				}
				cur = ps->Block1;
				//旋转逻辑2
				//1：
				cur->x -= 2;
				cur->y += 1;
				SetPos(cur->x, cur->y);
				cur = cur->next;
				wprintf(L"%lc", WALL);
				//2：
				cur->x += 2;
				cur->y += 1;
				SetPos(cur->x, cur->y);
				cur = cur->next;
				wprintf(L"%lc", WALL);
				//3:
				SetPos(cur->x, cur->y);
				cur = cur->next;
				wprintf(L"%lc", WALL);
				//4:
				cur->x -= 2;
				cur->y -= 1;
				SetPos(cur->x, cur->y);
				wprintf(L"%lc", WALL);
				cur = ps->Block1;
				ps->Dir = 0;
				tem = 2;
			}
			if (ps->BStatus == Status_3)
			{
				cur = ps->Block1;
				if (matrix[cur->y - 1][cur->x - 2] == 0 || matrix[cur->next->y + 1][cur->next->x - 2] == 0 || matrix[cur->next->next->y][cur->next->next->x] == 0 || matrix[cur->next->next->next->y - 1][cur->next->next->next->x + 2] == 0)
				{
					goto again;
				}
				//清除原本的打印
				cur = ps->Block1;
				while (cur != NULL)
				{
					SetPos(cur->x, cur->y);
					printf("  ");
					cur = cur->next;
				}
				cur = ps->Block1;
				//旋转逻辑3
				//1：
				cur->x -= 2;
				cur->y -= 1;
				SetPos(cur->x, cur->y);
				cur = cur->next;
				wprintf(L"%lc", WALL);
				//2：
				cur->x -= 2;
				cur->y += 1;
				SetPos(cur->x, cur->y);
				cur = cur->next;
				wprintf(L"%lc", WALL);
				//3:
				SetPos(cur->x, cur->y);
				cur = cur->next;
				wprintf(L"%lc", WALL);
				//4:
				cur->x += 2;
				cur->y -= 1;
				SetPos(cur->x, cur->y);
				wprintf(L"%lc", WALL);
				cur = ps->Block1;
				ps->Dir = 0;
				tem = 3;
			}
			if (ps->BStatus == Status_4)
			{
				cur = ps->Block1;
				if (matrix[cur->y - 1][cur->x + 2] == 0 || matrix[cur->next->y - 1][cur->next->x - 2] == 0 || matrix[cur->next->next->y][cur->next->next->x] == 0 || matrix[cur->next->next->next->y + 1][cur->next->next->next->x + 2] == 0)
				{
					goto again;
				}
				//清除原本的打印
				cur = ps->Block1;
				while (cur != NULL)
				{
					SetPos(cur->x, cur->y);
					printf("  ");
					cur = cur->next;
				}
				cur = ps->Block1;
				//旋转逻辑4
				//1：
				cur->x += 2;
				cur->y -= 1;
				SetPos(cur->x, cur->y);
				cur = cur->next;
				wprintf(L"%lc", WALL);
				//2：
				cur->x -= 2;
				cur->y -= 1;
				SetPos(cur->x, cur->y);
				cur = cur->next;
				wprintf(L"%lc", WALL);
				//3:
				SetPos(cur->x, cur->y);
				cur = cur->next;
				wprintf(L"%lc", WALL);
				//4:
				cur->x += 2;
				cur->y += 1;
				SetPos(cur->x, cur->y);
				wprintf(L"%lc", WALL);
				cur = ps->Block1;
				ps->Dir = 0;
				tem = 4;
			}
			if (tem == 1)
			{
				ps->BStatus = Status_2;
			}
			else if (tem == 2)
			{
				ps->BStatus = Status_3;
			}
			else if (tem == 3)
			{
				ps->BStatus = Status_4;
			}
			else if (tem == 4)
			{
				ps->BStatus = Status_1;
			}
			Sleep(300);
			goto again;
		}
		}
		cur = ps->Block1;
		if (ps->Block1 != NULL && ps->Block1->y+1 > 0)
		{
			//改变结构体状态
			ChangeStatus(ps, matrix);
			//确定边界
			Verge(ps, matrix);
		}
		if (ps->GStatus == Stop)
		{
			goto end;
		}
		if (ps->GStatus == END)
		{
			break;
		}
		//这里是没有检测到任何按键的正常下落状态
		cur = ps->Block1;
		while (cur != NULL)
		{
			if (cur->y > 0)
			{
				SetPos(cur->x, cur->y);
				printf("  ");
			}
			cur = cur->next;
		}
		cur = ps->Block1;
		while (cur != NULL)
		{
			(cur->y)++;
			SetPos(cur->x, cur->y);
			cur = cur->next;
			wprintf(L"%lc", WALL);
		}
		Sleep(300);
	} while (ps->GStatus == Falling);
end:cur = ps->Block1;
	while (cur != NULL)
	{
		cur = cur->next;
		free(ps->Block1);
		ps->Block1 = cur;
	}
ps->BStatus = Status_1;
}

void BlockFalling_4(pBlock ps, int matrix[height][width * 2])
{
	pBlockNode cur = ps->Block1;
	do
	{
	again: cur = ps->Block1;
		ps->Dir = 0;
		if (ps->Block1 != NULL && ps->Block1->y > 0)
		{
			CheckKey(ps);
		}
		switch (ps->Dir)
		{
		case DOWN:
		{
			while (cur != NULL)
			{
				SetPos(cur->x, cur->y);
				printf("  ");
				cur = cur->next;
			}
			do
			{
				cur = ps->Block1;
				//改变结构体状态
				ChangeStatus(ps, matrix);
				if (ps->GStatus == Stop)
				{
					while (cur)
					{
						SetPos(cur->x, cur->y);
						wprintf(L"%lc", WALL);
						cur = cur->next;
					}
					Sleep(300);
					ps->GStatus = Falling;
					goto again;
				}
				cur = ps->Block1;
				while (cur != NULL)
				{
					(cur->y)++;
					cur = cur->next;
				}
				cur = ps->Block1;
			} while (ps->GStatus == Falling);
		}
		case LEFT:
		{
			cur = ps->Block1;
			while (cur)
			{
				if (matrix[cur->y][(cur->x) - 2] == 0)
				{
					Sleep(300);
					goto again;
				}
				cur = cur->next;
			}
			cur = ps->Block1;
			while (cur != NULL)
			{
				SetPos(cur->x, cur->y);
				printf("  ");
				cur = cur->next;
			}
			cur = ps->Block1;
			while (cur != NULL)
			{
				(cur->x) -= 2;
				SetPos(cur->x, cur->y);
				cur = cur->next;
				wprintf(L"%lc", WALL);
			}
			cur = ps->Block1;
			Sleep(300);
			goto again;
		}
		case RIGHT:
		{
			cur = ps->Block1;
			while (cur)
			{
				if (matrix[cur->y][(cur->x) + 2] == 0)
				{
					Sleep(300);
					goto again;
				}
				cur = cur->next;
			}
			cur = ps->Block1;
			while (cur != NULL)
			{
				SetPos(cur->x, cur->y);
				printf("  ");
				cur = cur->next;
			}
			cur = ps->Block1;
			while (cur != NULL)
			{
				(cur->x) += 2;
				SetPos(cur->x, cur->y);
				cur = cur->next;
				wprintf(L"%lc", WALL);
			}
			cur = ps->Block1;
			Sleep(300);
			goto again;
		}
		case ROTE:
		{
			//记录方块状态
			int tem = 0;

			if (ps->BStatus == Status_1)
			{
				cur = ps->Block1;
				if (matrix[cur->y + 2][cur->x - 4] == 0 || matrix[cur->next->y + 1][cur->next->x - 2] == 0 || matrix[cur->next->next->y][cur->next->next->x] == 0 || matrix[cur->next->next->next->y - 1][cur->next->next->next->x + 2] == 0)
				{
					goto again;
				}
				cur = ps->Block1;
				//清除原本的打印
				while (cur != NULL)
				{
					SetPos(cur->x, cur->y);
					printf("  ");
					cur = cur->next;
				}
				cur = ps->Block1;
				//旋转逻辑1
				//1：
				cur->x -= 4;
				cur->y += 2;
				SetPos(cur->x, cur->y);
				cur = cur->next;
				wprintf(L"%lc", WALL);
				//2：
				cur->x -= 2;
				cur->y += 1;
				SetPos(cur->x, cur->y);
				cur = cur->next;
				wprintf(L"%lc", WALL);
				//3:
				SetPos(cur->x, cur->y);
				cur = cur->next;
				wprintf(L"%lc", WALL);
				//4:
				cur->x += 2;
				cur->y -= 1;
				SetPos(cur->x, cur->y);
				wprintf(L"%lc", WALL);
				cur = ps->Block1;
				ps->Dir = 0;
				tem = 1;
			}
			if (ps->BStatus == Status_2)
			{
				cur = ps->Block1;
				if (matrix[cur->y - 2][cur->x + 4] == 0 || matrix[cur->next->y - 1][cur->next->x + 2] == 0 || matrix[cur->next->next->y][cur->next->next->x] == 0 || matrix[cur->next->next->next->y + 1][cur->next->next->next->x - 2] == 0)
				{
					goto again;
				}
				//清除原本的打印
				cur = ps->Block1;
				while (cur != NULL)
				{
					SetPos(cur->x, cur->y);
					printf("  ");
					cur = cur->next;
				}
				cur = ps->Block1;
				//旋转逻辑2
				//1：
				cur->x += 4;
				cur->y -= 2;
				SetPos(cur->x, cur->y);
				cur = cur->next;
				wprintf(L"%lc", WALL);
				//2：
				cur->x += 2;
				cur->y -= 1;
				SetPos(cur->x, cur->y);
				cur = cur->next;
				wprintf(L"%lc", WALL);
				//3:
				SetPos(cur->x, cur->y);
				cur = cur->next;
				wprintf(L"%lc", WALL);
				//4:
				cur->x -= 2;
				cur->y += 1;
				SetPos(cur->x, cur->y);
				wprintf(L"%lc", WALL);
				cur = ps->Block1;
				ps->Dir = 0;
				tem = 2;
			}
			if (tem == 1)
			{
				ps->BStatus = Status_2;
			}
			else if (tem == 2)
			{
				ps->BStatus = Status_1;
			}
			Sleep(300);
			goto again;
		}
		}
		cur = ps->Block1;
		if (ps->Block1 != NULL && ps->Block1->y+1 > 0)
		{
			//改变结构体状态
			ChangeStatus(ps, matrix);
			//确定边界
			Verge(ps, matrix);
		}
		if (ps->GStatus == Stop)
		{
			goto end;
		}
		if (ps->GStatus == END)
		{
			break;
		}
		//这里是没有检测到任何按键的正常下落状态
		cur = ps->Block1;
		while (cur != NULL)
		{
			if (cur->y > 0)
			{
				SetPos(cur->x, cur->y);
				printf("  ");
			}
			cur = cur->next;
		}
		cur = ps->Block1;
		while (cur != NULL)
		{
			(cur->y)++;
			SetPos(cur->x, cur->y);
			cur = cur->next;
			wprintf(L"%lc", WALL);
		}
		Sleep(300);
	} while (ps->GStatus == Falling);
end:cur = ps->Block1;
	while (cur != NULL)
	{
		cur = cur->next;
		free(ps->Block1);
		ps->Block1 = cur;
	}
ps->BStatus = Status_1;
}

void BlockFalling_5(pBlock ps, int matrix[height][width * 2])
{
	pBlockNode cur = ps->Block1;
	do
	{
	again: cur = ps->Block1;
		ps->Dir = 0;
		if (ps->Block1 != NULL && ps->Block1->y > 0)
		{
			CheckKey(ps);
		}
		switch (ps->Dir)
		{
		case DOWN:
		{
			while (cur != NULL)
			{
				SetPos(cur->x, cur->y);
				printf("  ");
				cur = cur->next;
			}
			do
			{
				cur = ps->Block1;
				//改变结构体状态
				ChangeStatus(ps, matrix);
				if (ps->GStatus == Stop)
				{
					while (cur)
					{
						SetPos(cur->x, cur->y);
						wprintf(L"%lc", WALL);
						cur = cur->next;
					}
					Sleep(300);
					ps->GStatus = Falling;
					goto again;
				}
				cur = ps->Block1;
				while (cur != NULL)
				{
					(cur->y)++;
					cur = cur->next;
				}
				cur = ps->Block1;
			} while (ps->GStatus == Falling);
		}
		case LEFT:
		{
			cur = ps->Block1;
			while (cur)
			{
				if (matrix[cur->y][(cur->x) - 2] == 0)
				{
					Sleep(300);
					goto again;
				}
				cur = cur->next;
			}
			cur = ps->Block1;
			while (cur != NULL)
			{
				SetPos(cur->x, cur->y);
				printf("  ");
				cur = cur->next;
			}
			cur = ps->Block1;
			while (cur != NULL)
			{
				(cur->x) -= 2;
				SetPos(cur->x, cur->y);
				cur = cur->next;
				wprintf(L"%lc", WALL);
			}
			cur = ps->Block1;
			Sleep(300);
			goto again;
		}
		case RIGHT:
		{
			cur = ps->Block1;
			while (cur)
			{
				if (matrix[cur->y][(cur->x) + 2] == 0)
				{
					Sleep(300);
					goto again;
				}
				cur = cur->next;
			}
			cur = ps->Block1;
			while (cur != NULL)
			{
				SetPos(cur->x, cur->y);
				printf("  ");
				cur = cur->next;
			}
			cur = ps->Block1;
			while (cur != NULL)
			{
				(cur->x) += 2;
				SetPos(cur->x, cur->y);
				cur = cur->next;
				wprintf(L"%lc", WALL);
			}
			cur = ps->Block1;
			Sleep(300);
			goto again;
		}
		}
		cur = ps->Block1;
		if (ps->Block1 != NULL && ps->Block1->y+1 > 0)
		{
			//改变结构体状态
			ChangeStatus(ps, matrix);
			//确定边界
			Verge(ps, matrix);
		}
		if (ps->GStatus == Stop)
		{
			goto end;
		}
		if (ps->GStatus == END)
		{
			break;
		}
		//这里是没有检测到任何按键的正常下落状态
		cur = ps->Block1;
		while (cur != NULL)
		{
			if (cur->y > 0)
			{
				SetPos(cur->x, cur->y);
				printf("  ");
			}
			cur = cur->next;
		}
		cur = ps->Block1;
		while (cur != NULL)
		{
			(cur->y)++;
			SetPos(cur->x, cur->y);
			cur = cur->next;
			wprintf(L"%lc", WALL);
		}
		Sleep(300);
	} while (ps->GStatus == Falling);
end:cur = ps->Block1;
	while (cur != NULL)
	{
		cur = cur->next;
		free(ps->Block1);
		ps->Block1 = cur;
	}
ps->BStatus = Status_1;
}

void BlockFalling_6(pBlock ps, int matrix[height][width * 2])
{
	pBlockNode cur = ps->Block1;
	do
	{
	again: cur = ps->Block1;
		ps->Dir = 0;
		if (ps->Block1 != NULL && ps->Block1->y > 0)
		{
			CheckKey(ps);
		}
		switch (ps->Dir)
		{
		case DOWN:
		{
			while (cur != NULL)
			{
				SetPos(cur->x, cur->y);
				printf("  ");
				cur = cur->next;
			}
			do
			{
				cur = ps->Block1;
				//改变结构体状态
				ChangeStatus(ps, matrix);
				if (ps->GStatus == Stop)
				{
					while (cur)
					{
						SetPos(cur->x, cur->y);
						wprintf(L"%lc", WALL);
						cur = cur->next;
					}
					Sleep(300);
					ps->GStatus = Falling;
					goto again;
				}
				cur = ps->Block1;
				while (cur != NULL)
				{
					(cur->y)++;
					cur = cur->next;
				}
				cur = ps->Block1;
			} while (ps->GStatus == Falling);
		}
		case LEFT:
		{
			cur = ps->Block1;
			while (cur)
			{
				if (matrix[cur->y][(cur->x) - 2] == 0)
				{
					Sleep(300);
					goto again;
				}
				cur = cur->next;
			}
			cur = ps->Block1;
			while (cur != NULL)
			{
				SetPos(cur->x, cur->y);
				printf("  ");
				cur = cur->next;
			}
			cur = ps->Block1;
			while (cur != NULL)
			{
				(cur->x) -= 2;
				SetPos(cur->x, cur->y);
				cur = cur->next;
				wprintf(L"%lc", WALL);
			}
			cur = ps->Block1;
			Sleep(300);
			goto again;
		}
		case RIGHT:
		{
			cur = ps->Block1;
			while (cur)
			{
				if (matrix[cur->y][(cur->x) + 2] == 0)
				{
					Sleep(300);
					goto again;
				}
				cur = cur->next;
			}
			cur = ps->Block1;
			while (cur != NULL)
			{
				SetPos(cur->x, cur->y);
				printf("  ");
				cur = cur->next;
			}
			cur = ps->Block1;
			while (cur != NULL)
			{
				(cur->x) += 2;
				SetPos(cur->x, cur->y);
				cur = cur->next;
				wprintf(L"%lc", WALL);
			}
			cur = ps->Block1;
			Sleep(300);
			goto again;
		}
		case ROTE:
		{
			//记录方块状态
			int tem = 0;
			if (ps->BStatus == Status_1)
			{
				cur = ps->Block1;
				if (matrix[cur->y - 1][cur->x + 2] == 0 || matrix[cur->next->y][cur->next->x] == 0 || matrix[cur->next->next->y][cur->next->next->x - 4] == 0 || matrix[cur->next->next->next->y - 1][cur->next->next->next->x - 2] == 0)
				{
					goto again;
				}
				//清除原本的打印
				cur = ps->Block1;
				while (cur != NULL)
				{
					SetPos(cur->x, cur->y);
					printf("  ");
					cur = cur->next;
				}
				cur = ps->Block1;
				//旋转逻辑1
				//1：
				cur->x += 2;
				cur->y -= 1;
				SetPos(cur->x, cur->y);
				cur = cur->next;
				wprintf(L"%lc", WALL);
				//2：
				SetPos(cur->x, cur->y);
				cur = cur->next;
				wprintf(L"%lc", WALL);
				//3:
				cur->x -= 4;
				SetPos(cur->x, cur->y);
				cur = cur->next;
				wprintf(L"%lc", WALL);
				//4:
				cur->x -= 2;
				cur->y -= 1;
				SetPos(cur->x, cur->y);
				wprintf(L"%lc", WALL);
				cur = ps->Block1;
				ps->Dir = 0;
				tem = 1;
			}
			if (ps->BStatus == Status_2)
			{
				cur = ps->Block1;
				if (matrix[cur->y + 2][cur->x + 2] == 0 || matrix[cur->next->y + 1][cur->next->x] == 0 || matrix[cur->next->next->y - 1][cur->next->next->x] == 0 || matrix[cur->next->next->next->y][cur->next->next->next->x + 2] == 0)
				{
					goto again;
				}
				//清除原本的打印
				cur = ps->Block1;
				while (cur != NULL)
				{
					SetPos(cur->x, cur->y);
					printf("  ");
					cur = cur->next;
				}
				cur = ps->Block1;
				//旋转逻辑2
				//1：
				cur->x += 2;
				cur->y += 2;
				SetPos(cur->x, cur->y);
				cur = cur->next;
				wprintf(L"%lc", WALL);
				//2：
				cur->y += 1;
				SetPos(cur->x, cur->y);
				cur = cur->next;
				wprintf(L"%lc", WALL);
				//3:
				cur->y -= 1;
				SetPos(cur->x, cur->y);
				cur = cur->next;
				wprintf(L"%lc", WALL);
				//4:
				cur->x += 2;
				SetPos(cur->x, cur->y);
				wprintf(L"%lc", WALL);
				cur = ps->Block1;
				ps->Dir = 0;
				tem = 2;
			}
			if (ps->BStatus == Status_3)
			{
				cur = ps->Block1;
				if (matrix[cur->y][cur->x - 4] == 0 || matrix[cur->next->y - 1][cur->next->x - 2] == 0 || matrix[cur->next->next->y - 1][cur->next->next->x + 2] == 0 || matrix[cur->next->next->next->y][cur->next->next->next->x] == 0)
				{
					goto again;
				}
				//清除原本的打印
				cur = ps->Block1;
				while (cur != NULL)
				{
					SetPos(cur->x, cur->y);
					printf("  ");
					cur = cur->next;
				}
				cur = ps->Block1;
				//旋转逻辑3
				//1：
				cur->x -= 4;
				SetPos(cur->x, cur->y);
				cur = cur->next;
				wprintf(L"%lc", WALL);
				//2：
				cur->x -= 2;
				cur->y -= 1;
				SetPos(cur->x, cur->y);
				cur = cur->next;
				wprintf(L"%lc", WALL);
				//3:
				cur->x += 2;
				cur->y -= 1;
				SetPos(cur->x, cur->y);
				cur = cur->next;
				wprintf(L"%lc", WALL);
				//4:
				SetPos(cur->x, cur->y);
				wprintf(L"%lc", WALL);
				cur = ps->Block1;
				ps->Dir = 0;
				tem = 3;
			}
			if (ps->BStatus == Status_4)
			{
				cur = ps->Block1;
				if (matrix[cur->y - 1][cur->x] == 0 || matrix[cur->next->y][cur->next->x + 2] == 0 || matrix[cur->next->next->y + 2][cur->next->next->x + 2] == 0 || matrix[cur->next->next->next->y + 1][cur->next->next->next->x] == 0)
				{
					goto again;
				}
				//清除原本的打印
				cur = ps->Block1;
				while (cur != NULL)
				{
					SetPos(cur->x, cur->y);
					printf("  ");
					cur = cur->next;
				}
				cur = ps->Block1;
				//旋转逻辑4
				//1：
				cur->y -= 1;
				SetPos(cur->x, cur->y);
				cur = cur->next;
				wprintf(L"%lc", WALL);
				//2：
				cur->x += 2;
				SetPos(cur->x, cur->y);
				cur = cur->next;
				wprintf(L"%lc", WALL);
				//3:
				cur->x += 2;
				cur->y += 2;
				SetPos(cur->x, cur->y);
				cur = cur->next;
				wprintf(L"%lc", WALL);
				//4:
				cur->y += 1;
				SetPos(cur->x, cur->y);
				wprintf(L"%lc", WALL);
				cur = ps->Block1;
				ps->Dir = 0;
				tem = 4;
			}
			if (tem == 1)
			{
				ps->BStatus = Status_2;
			}
			else if (tem == 2)
			{
				ps->BStatus = Status_3;
			}
			else if (tem == 3)
			{
				ps->BStatus = Status_4;
			}
			else if (tem == 4)
			{
				ps->BStatus = Status_1;
			}
			Sleep(300);
			goto again;
		}
		}
		cur = ps->Block1;
		if (ps->Block1 != NULL && ps->Block1->y+1 > 0)
		{
			//改变结构体状态
			ChangeStatus(ps, matrix);
			//确定边界
			Verge(ps, matrix);
		}
		if (ps->GStatus == Stop)
		{
			goto end;
		}
		if (ps->GStatus == END)
		{
			break;
		}
		//这里是没有检测到任何按键的正常下落状态
		cur = ps->Block1;
		while (cur != NULL)
		{
			if (cur->y > 0)
			{
				SetPos(cur->x, cur->y);
				printf("  ");
			}
			cur = cur->next;
		}
		cur = ps->Block1;
		while (cur != NULL)
		{
			(cur->y)++;
			SetPos(cur->x, cur->y);
			cur = cur->next;
			wprintf(L"%lc", WALL);
		}
		Sleep(300);
	} while (ps->GStatus == Falling);
end:cur = ps->Block1;
while (cur != NULL)
{
	cur = cur->next;
	free(ps->Block1);
	ps->Block1 = cur;
}
ps->BStatus = Status_1;
}

void BlockFalling_7(pBlock ps, int matrix[height][width * 2])
{
	pBlockNode cur = ps->Block1;
	do
	{
	again: cur = ps->Block1;
		ps->Dir = 0;
		if (ps->Block1 != NULL && ps->Block1->y > 0)
		{
			CheckKey(ps);
		}
		switch (ps->Dir)
		{
		case DOWN:
		{
			while (cur != NULL)
			{
				SetPos(cur->x, cur->y);
				printf("  ");
				cur = cur->next;
			}
			do
			{
				cur = ps->Block1;
				//改变结构体状态
				ChangeStatus(ps, matrix);
				if (ps->GStatus == Stop)
				{
					while (cur)
					{
						SetPos(cur->x, cur->y);
						wprintf(L"%lc", WALL);
						cur = cur->next;
					}
					Sleep(300);
					ps->GStatus = Falling;
					goto again;
				}
				cur = ps->Block1;
				while (cur != NULL)
				{
					(cur->y)++;
					cur = cur->next;
				}
				cur = ps->Block1;
			} while (ps->GStatus == Falling);
		}
		case LEFT:
		{
			cur = ps->Block1;
			while (cur)
			{
				if (matrix[cur->y][(cur->x) - 2] == 0)
				{
					Sleep(300);
					goto again;
				}
				cur = cur->next;
			}
			cur = ps->Block1;
			while (cur != NULL)
			{
				SetPos(cur->x, cur->y);
				printf("  ");
				cur = cur->next;
			}
			cur = ps->Block1;
			while (cur != NULL)
			{
				(cur->x) -= 2;
				SetPos(cur->x, cur->y);
				cur = cur->next;
				wprintf(L"%lc", WALL);
			}
			cur = ps->Block1;
			Sleep(300);
			goto again;
		}
		case RIGHT:
		{
			cur = ps->Block1;
			while (cur)
			{
				if (matrix[cur->y][(cur->x) + 2] == 0)
				{
					Sleep(300);
					goto again;
				}
				cur = cur->next;
			}
			cur = ps->Block1;
			while (cur != NULL)
			{
				SetPos(cur->x, cur->y);
				printf("  ");
				cur = cur->next;
			}
			cur = ps->Block1;
			while (cur != NULL)
			{
				(cur->x) += 2;
				SetPos(cur->x, cur->y);
				cur = cur->next;
				wprintf(L"%lc", WALL);
			}
			cur = ps->Block1;
			Sleep(300);
			goto again;
		}
		case ROTE:
		{
			//记录方块状态
			int tem = 0;
			if (ps->BStatus == Status_1)
			{
				cur = ps->Block1;
				if (matrix[cur->y + 1][cur->x - 2] == 0 || matrix[cur->next->y][cur->next->x] == 0 || matrix[cur->next->next->y - 1][cur->next->next->x - 2] == 0 || matrix[cur->next->next->next->y - 2][cur->next->next->next->x] == 0)
				{
					goto again;
				}
				//清除原本的打印
				cur = ps->Block1;
				while (cur != NULL)
				{
					SetPos(cur->x, cur->y);
					printf("  ");
					cur = cur->next;
				}
				cur = ps->Block1;
				//旋转逻辑1
				//1：
				cur->x -= 2;
				cur->y += 1;
				SetPos(cur->x, cur->y);
				cur = cur->next;
				wprintf(L"%lc", WALL);
				//2：
				SetPos(cur->x, cur->y);
				cur = cur->next;
				wprintf(L"%lc", WALL);
				//3:
				cur->x -= 2;
				cur->y -= 1;
				SetPos(cur->x, cur->y);
				cur = cur->next;
				wprintf(L"%lc", WALL);
				//4:
				cur->y -= 2;
				SetPos(cur->x, cur->y);
				wprintf(L"%lc", WALL);
				cur = ps->Block1;
				ps->Dir = 0;
				tem = 1;
			}
			if (ps->BStatus == Status_2)
			{
				cur = ps->Block1;
				if (matrix[cur->y][cur->x - 2] == 0 || matrix[cur->next->y + 1][cur->next->x] == 0 || matrix[cur->next->next->y][cur->next->next->x + 2] == 0 || matrix[cur->next->next->next->y][cur->next->next->next->x + 4] == 0)
				{
					goto again;
				}
				cur = ps->Block1;
				//清除原本的打印
				while (cur != NULL)
				{
					SetPos(cur->x, cur->y);
					printf("  ");
					cur = cur->next;
				}
				cur = ps->Block1;
				//旋转逻辑2
				//1：
				cur->x -= 2;
				SetPos(cur->x, cur->y);
				cur = cur->next;
				wprintf(L"%lc", WALL);
				//2：
				cur->y += 1;
				SetPos(cur->x, cur->y);
				cur = cur->next;
				wprintf(L"%lc", WALL);
				//3:
				cur->x += 2;
				SetPos(cur->x, cur->y);
				cur = cur->next;
				wprintf(L"%lc", WALL);
				//4:
				cur->x += 4;
				cur->y += 1;
				SetPos(cur->x, cur->y);
				wprintf(L"%lc", WALL);
				cur = ps->Block1;
				ps->Dir = 0;
				tem = 2;
			}
			if (ps->BStatus == Status_3)
			{
				cur = ps->Block1;
				if (matrix[cur->y - 2][cur->x] == 0 || matrix[cur->next->y - 1][cur->next->x - 2] == 0 || matrix[cur->next->next->y][cur->next->next->x] == 0 || matrix[cur->next->next->next->y + 1][cur->next->next->next->x - 2] == 0)
				{
					goto again;
				}
				//清除原本的打印
				while (cur != NULL)
				{
					SetPos(cur->x, cur->y);
					printf("  ");
					cur = cur->next;
				}
				cur = ps->Block1;
				//旋转逻辑3
				//1：
				cur->y -= 2;
				SetPos(cur->x, cur->y);
				cur = cur->next;
				wprintf(L"%lc", WALL);
				//2：
				cur->x -= 2;
				cur->y -= 1;
				SetPos(cur->x, cur->y);
				cur = cur->next;
				wprintf(L"%lc", WALL);
				//3:
				SetPos(cur->x, cur->y);
				cur = cur->next;
				wprintf(L"%lc", WALL);
				//4:
				cur->x -= 2;
				cur->y += 1;
				SetPos(cur->x, cur->y);
				wprintf(L"%lc", WALL);
				cur = ps->Block1;
				ps->Dir = 0;
				tem = 3;
			}
			if (ps->BStatus == Status_4)
			{
				cur = ps->Block1;
				if (matrix[cur->y + 1][cur->x + 4] == 0 || matrix[cur->next->y][cur->next->x + 2] == 0 || matrix[cur->next->next->y + 1][cur->next->next->x] == 0 || matrix[cur->next->next->next->y][cur->next->next->next->x - 2] == 0)
				{
					goto again;
				}
				//清除原本的打印
				while (cur != NULL)
				{
					SetPos(cur->x, cur->y);
					printf("  ");
					cur = cur->next;
				}
				cur = ps->Block1;
				//旋转逻辑4
				//1：
				cur->x += 4;
				cur->y += 1;
				SetPos(cur->x, cur->y);
				cur = cur->next;
				wprintf(L"%lc", WALL);
				//2：
				cur->x += 2;
				SetPos(cur->x, cur->y);
				cur = cur->next;
				wprintf(L"%lc", WALL);
				//3:
				cur->y += 1;
				SetPos(cur->x, cur->y);
				cur = cur->next;
				wprintf(L"%lc", WALL);
				//4:
				cur->x -= 2;
				SetPos(cur->x, cur->y);
				wprintf(L"%lc", WALL);
				cur = ps->Block1;
				ps->Dir = 0;
				tem = 4;
			}
			if (tem == 1)
			{
				ps->BStatus = Status_2;
			}
			else if (tem == 2)
			{
				ps->BStatus = Status_3;
			}
			else if (tem == 3)
			{
				ps->BStatus = Status_4;
			}
			else if (tem == 4)
			{
				ps->BStatus = Status_1;
			}
			Sleep(300);
			goto again;
		}
		}
		cur = ps->Block1;
		if (ps->Block1 != NULL && ps->Block1->y+1 > 0)
		{
			//改变结构体状态
			ChangeStatus(ps, matrix);
			//确定边界
			Verge(ps, matrix);
		}
		if (ps->GStatus == Stop)
		{
			goto end;
		}
		if (ps->GStatus == END)
		{
			break;
		}
		//这里是没有检测到任何按键的正常下落状态
		cur = ps->Block1;
		while (cur != NULL)
		{
			if (cur->y > 0)
			{
				SetPos(cur->x, cur->y);
				printf("  ");
			}
			cur = cur->next;
		}
		cur = ps->Block1;
		while (cur != NULL)
		{
			(cur->y)++;
			SetPos(cur->x, cur->y);
			cur = cur->next;
			wprintf(L"%lc", WALL);
		}
		Sleep(300);
	} while (ps->GStatus == Falling);
end:cur = ps->Block1;
	ps->Dir = 0;
	while (cur != NULL)
	{
		cur = cur->next;
		free(ps->Block1);
		ps->Block1 = cur;
	}
ps->BStatus = Status_1;
}

//随机生成一个1-7的数字后调用不同的函数
void CreatBlock_Random(pBlock ps, int matrix[height][width * 2])
{
	ps->GStatus = Falling;
	unsigned int seed = (unsigned int)clock();
	srand(seed); // 用当前时间初始化随机数生成器
	int ran1 = rand() % 7 + 1;
	int ran2 = rand() % 7 + 1;
	SetPos(30, 1);
	printf("您当前的得分为：%d分", ps->Score);
	SetPos(30, 5);
	printf("下一个方块为：");
	if (ran2 == 1)
	{
		SetPos(30,8);
		wprintf(L"%lc" ,WALL);
		SetPos(32,8);
		wprintf(L"%lc" ,WALL);
		SetPos(34,8);
		wprintf(L"%lc" ,WALL);
		SetPos(34,7);
		wprintf(L"%lc" ,WALL);
	}
	if (ran2 == 2)
	{
		SetPos(30, 8);
		wprintf(L"%lc", WALL);
		SetPos(32, 8);
		wprintf(L"%lc", WALL);
		SetPos(34, 8);
		wprintf(L"%lc", WALL);
		SetPos(30, 7);
		wprintf(L"%lc", WALL);
	}
	if (ran2 == 3)
	{
		SetPos(30, 8);
		wprintf(L"%lc", WALL);
		SetPos(32, 8);
		wprintf(L"%lc", WALL);
		SetPos(34, 8);
		wprintf(L"%lc", WALL);
		SetPos(32, 7);
		wprintf(L"%lc", WALL);
	}
	if (ran2 == 4)
	{
		SetPos(30, 8);
		wprintf(L"%lc", WALL);
		SetPos(32, 8);
		wprintf(L"%lc", WALL);
		SetPos(34, 8);
		wprintf(L"%lc", WALL);
		SetPos(36, 8);
		wprintf(L"%lc", WALL);
	}
	if (ran2 == 5)
	{
		SetPos(30, 8);
		wprintf(L"%lc", WALL);
		SetPos(32, 8);
		wprintf(L"%lc", WALL);
		SetPos(30, 7);
		wprintf(L"%lc", WALL);
		SetPos(32, 7);
		wprintf(L"%lc", WALL);
	}
	if (ran2 == 6)
	{
		SetPos(32, 8);
		wprintf(L"%lc", WALL);
		SetPos(34, 8);
		wprintf(L"%lc", WALL);
		SetPos(30, 7);
		wprintf(L"%lc", WALL);
		SetPos(32, 7);
		wprintf(L"%lc", WALL);
	}
	if (ran2 == 7)
	{
		SetPos(30, 8);
		wprintf(L"%lc", WALL);
		SetPos(32, 8);
		wprintf(L"%lc", WALL);
		SetPos(32, 7);
		wprintf(L"%lc", WALL);
		SetPos(34, 7);
		wprintf(L"%lc", WALL);
	}
	if (ran1 == 1)
	{
		CreatBlock_1(ps);
		BlockFalling_1(ps, matrix);
	}
	if (ran1 == 2)
	{
		CreatBlock_2(ps);
		BlockFalling_2(ps, matrix);
	}
	if (ran1 == 3)
	{
		CreatBlock_3(ps);
		BlockFalling_3(ps, matrix);
	}
	if (ran1 == 4)
	{
		CreatBlock_4(ps);
		BlockFalling_4(ps, matrix);
	}
	if (ran1 == 5)
	{
		CreatBlock_5(ps);
		BlockFalling_5(ps, matrix);
	}
	if (ran1 == 6)
	{
		CreatBlock_6(ps);
		BlockFalling_6(ps, matrix);
	}
	if (ran1 == 7)
	{
		CreatBlock_7(ps);
		BlockFalling_7(ps, matrix);
	}
	Remove_Wall(ps, matrix);
	Print_Wall(ps, matrix);
	do
	{
		ps->GStatus = Falling;
		unsigned int seed = (unsigned int)clock();
		srand(seed);
		ran1 = ran2;
		ran2 = rand() % 7 + 1;
		SetPos(30, 1);
		printf("您当前的得分为：%d分", ps->Score);
		SetPos(30, 5);
		printf("下一个方块为：");
		if (ran2 == 1)
		{
			SetPos(30, 8);
			wprintf(L"%lc", WALL);
			SetPos(32, 8);
			wprintf(L"%lc", WALL);
			SetPos(34, 8);
			wprintf(L"%lc", WALL);
			SetPos(34, 7);
			wprintf(L"%lc", WALL);
		}
		if (ran2 == 2)
		{
			SetPos(30, 8);
			wprintf(L"%lc", WALL);
			SetPos(32, 8);
			wprintf(L"%lc", WALL);
			SetPos(34, 8);
			wprintf(L"%lc", WALL);
			SetPos(30, 7);
			wprintf(L"%lc", WALL);
		}
		if (ran2 == 3)
		{
			SetPos(30, 8);
			wprintf(L"%lc", WALL);
			SetPos(32, 8);
			wprintf(L"%lc", WALL);
			SetPos(34, 8);
			wprintf(L"%lc", WALL);
			SetPos(32, 7);
			wprintf(L"%lc", WALL);
		}
		if (ran2 == 4)
		{
			SetPos(30, 8);
			wprintf(L"%lc", WALL);
			SetPos(32, 8);
			wprintf(L"%lc", WALL);
			SetPos(34, 8);
			wprintf(L"%lc", WALL);
			SetPos(36, 8);
			wprintf(L"%lc", WALL);
		}
		if (ran2 == 5)
		{
			SetPos(30, 8);
			wprintf(L"%lc", WALL);
			SetPos(32, 8);
			wprintf(L"%lc", WALL);
			SetPos(30, 7);
			wprintf(L"%lc", WALL);
			SetPos(32, 7);
			wprintf(L"%lc", WALL);
		}
		if (ran2 == 6)
		{
			SetPos(32, 8);
			wprintf(L"%lc", WALL);
			SetPos(34, 8);
			wprintf(L"%lc", WALL);
			SetPos(30, 7);
			wprintf(L"%lc", WALL);
			SetPos(32, 7);
			wprintf(L"%lc", WALL);
		}
		if (ran2 == 7)
		{
			SetPos(30, 8);
			wprintf(L"%lc", WALL);
			SetPos(32, 8);
			wprintf(L"%lc", WALL);
			SetPos(32, 7);
			wprintf(L"%lc", WALL);
			SetPos(34, 7);
			wprintf(L"%lc", WALL);
		}
		if (ran1 == 1)
		{
			CreatBlock_1(ps);
			BlockFalling_1(ps, matrix);
		}
		if (ran1 == 2)
		{
			CreatBlock_2(ps);
			BlockFalling_2(ps, matrix);
		}
		if (ran1 == 3)
		{
			CreatBlock_3(ps);
			BlockFalling_3(ps, matrix);
		}
		if (ran1 == 4)
		{
			CreatBlock_4(ps);
			BlockFalling_4(ps, matrix);
		}
		if (ran1 == 5)
		{
			CreatBlock_5(ps);
			BlockFalling_5(ps, matrix);
		}
		if (ran1 == 6)
		{
			CreatBlock_6(ps);
			BlockFalling_6(ps, matrix);
		}
		if (ran1 == 7)
		{
			CreatBlock_7(ps);
			BlockFalling_7(ps, matrix);
		}
		Remove_Wall(ps, matrix);
		Print_Wall(ps, matrix);
	} while (ps->GStatus==Stop);
}

//游戏运行
void GameRun(pBlock ps, int matrix[height][width * 2])
{
	SetCmd();
	Set_Cursor();
	SetLoc();
	Menu();
	CreatMap();
	InitMatrix(matrix);
	CreatBlock_Random(ps, matrix);
	GameOver();
}