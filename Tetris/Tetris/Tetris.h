#pragma once

#include<stdio.h>
#include<windows.h>
#include<stdbool.h>
#include<locale.h>
#include<stdlib.h>
#include<time.h>
#include<assert.h>

//符号
#define WALL L'■'
#define width 12
#define height 22

//游戏状态
enum Game_Status
{
	Falling,
	Stop,
	END
};

//按键检测
enum Direction
{
	DOWN = 1,
	ROTE,
	LEFT,
	RIGHT
};

//方块状态
enum Block_Status
{
	Status_1,
	Status_2,
	Status_3,
	Status_4
};

//方块链表
typedef struct BlockNode
{
	//记录坐标
	int x;
	int y;
	struct BlockNode* next;
}BlockNode,* pBlockNode;

//维护游戏的结构体
typedef struct Block
{
	//方块链表
	pBlockNode Block1;
	//分数
	int Score;
	//方块状态
	enum Block_Status BStatus;
	//游戏状态
	enum Game_Status GStatus;
	//按键检测
	enum Direction Dir;
}Block,* pBlock;

void SetCmd();

void Set_Cursor();

void SetPos(short x, short y);

void SetLoc();

void Menu();

void CreatMap();

void InitMatrix(int matrix[height][width * 2]);

void CreatBlock_1(pBlock ps);

void CreatBlock_2(pBlock ps);

void CreatBlock_3(pBlock ps);

void CreatBlock_4(pBlock ps);

void CreatBlock_5(pBlock ps);

void CreatBlock_6(pBlock ps);

void CreatBlock_7(pBlock ps);

void BlockFalling_1(pBlock ps, int matrix[height][width * 2]);

void BlockFalling_2(pBlock ps, int matrix[height][width * 2]);

void BlockFalling_3(pBlock ps, int matrix[height][width * 2]);

void BlockFalling_4(pBlock ps, int matrix[height][width * 2]);

void BlockFalling_5(pBlock ps, int matrix[height][width * 2]);

void BlockFalling_6(pBlock ps, int matrix[height][width * 2]);

void BlockFalling_7(pBlock ps, int matrix[height][width * 2]);

void Verge(pBlock ps, int matrix[height][width * 2]);

void ChangeStatus(pBlock ps, int[height][width * 2]);

void GameRun(pBlock ps, int matrix[height][width * 2]);

void CheckKey(pBlock ps);

void pause();

void Remove_Wall(pBlock ps, int matrix[height][width * 2]);

void Print_Wall(pBlock ps, int matrix[height][width * 2]);

void CreatBlock_Random(pBlock ps, int matrix[height][width * 2]);

void GameOver();