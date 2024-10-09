#include"Tetris.h"
#define KEY_PRESS(VK) ( (GetAsyncKeyState(VK) & 0x1) ? 1 : 0 )

int main()
{
	Block block = { 0 };
	int matrix[height][width * 2] = { 0 };
	while (1)
	{
		GameRun(&block, matrix);
		while (1)
		{
			Sleep(300);
			if (KEY_PRESS(0x59))
			{
				break;
			}
			if (KEY_PRESS(0x4E))
			{
				return 1;
			}
		}
	}
}