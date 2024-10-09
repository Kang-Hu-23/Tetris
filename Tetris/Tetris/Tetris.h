#pragma once

#include<stdio.h>
#include<windows.h>
#include<stdbool.h>
#include<locale.h>
#include<stdlib.h>
#include<time.h>
#include<assert.h>

//����
#define WALL L'��'
#define width 12
#define height 22

//��Ϸ״̬
enum Game_Status
{
	Falling,
	Stop,
	END
};

//�������
enum Direction
{
	DOWN = 1,
	ROTE,
	LEFT,
	RIGHT
};

//����״̬
enum Block_Status
{
	Status_1,
	Status_2,
	Status_3,
	Status_4
};

//��������
typedef struct BlockNode
{
	//��¼����
	int x;
	int y;
	struct BlockNode* next;
}BlockNode,* pBlockNode;

//ά����Ϸ�Ľṹ��
typedef struct Block
{
	//��������
	pBlockNode Block1;
	//����
	int Score;
	//����״̬
	enum Block_Status BStatus;
	//��Ϸ״̬
	enum Game_Status GStatus;
	//�������
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