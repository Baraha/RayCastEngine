#include <Windows.h>
#include <string>
#include <chrono>
using namespace std;


int nScreenWidth = 120;
int nScreenHeight = 40;

int nMapWidth = 16;
int nMapHeight = 16;

float playerX = 8.0f;
float playerY = 8.0f; 
float playerA = 0.0f;

float fFOV = 3.14159f / 4.0f; // пи на 4 = 40 градусов, поле нашего обзора
float fZDepth = 16.0;

float speed = 2.0f; // просто скорость передвижения


int main() 
{
	wchar_t* screen = new wchar_t[nScreenWidth * nScreenHeight];
	HANDLE hConsole = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
	SetConsoleActiveScreenBuffer(hConsole);
	DWORD dwBytesWritten = 0;

	wstring map; // создаем карту, площадь по которой будем ходить.
	map += L"################";
	map += L"#..............#";
	map += L"#..............#";
	map += L"#..............#";
	map += L"#....#.........#";
	map += L"#..............#";
	map += L"#..............#";
	map += L"#.........#....#";
	map += L"#..............#";
	map += L"#..............#";
	map += L"#..............#";
	map += L"#.....#........#";
	map += L"#..............#";
	map += L"#..............#";
	map += L"#..............#";
	map += L"################";



	auto tp1 = chrono::system_clock::now(); // подключил chrono для улучшения картинки, ускорения вычислений
	auto tp2 = chrono::system_clock::now();


	while (1) {//доработать

		tp2 = chrono::system_clock::now();
		chrono::duration<float> elapsed_time = tp2 - tp1;
		tp1 = tp2;
		float fElapsedTime = elapsed_time.count();


		// Управление героем (глянул как делать в интернете)
		if (GetAsyncKeyState((unsigned short)'A') & 0x8000) {
			playerA -= (0.92f) * fElapsedTime;

		}
		if (GetAsyncKeyState((unsigned short)'D') & 0x8000)
			playerA += (0.92f) * fElapsedTime;

		if (GetAsyncKeyState((unsigned short)'W') & 0x8000)
		{
			playerX += sinf(playerA) * speed * fElapsedTime;
			playerY += cosf(playerA) * speed * fElapsedTime;
			if (map.c_str()[(int)playerX * nMapWidth + (int)playerY] == '#')
			{
				playerX -= sinf(playerA) * speed * fElapsedTime;
				playerY -= cosf(playerA) * speed * fElapsedTime;
			}
		}
		if (GetAsyncKeyState((unsigned short)'S') & 0x8000)
		{
			playerX -= sinf(playerA) * speed * fElapsedTime;
			playerY -= cosf(playerA) * speed * fElapsedTime;
			if (map.c_str()[(int)playerX * nMapWidth + (int)playerY] == '#')
			{
				playerX += sinf(playerA) * speed * fElapsedTime;
				playerY += cosf(playerA) * speed * fElapsedTime;
			}
		}


		for (int x = 0; x < nScreenWidth; x++)
		{
			float fRayAngle = (playerA - fFOV / 2.0f) + ((float)x / (float)nScreenWidth) * fFOV; // делим наше поле обзора на два, создавая луч посередине угла 
			float distToWall = 0.0f;
			bool bHitWall = false; // Детект соприкносновения со стеной 

			float fEyeX = sinf(fRayAngle);// высчитываем углы обзора
			float fEyeY = cosf(fRayAngle);
			while (!bHitWall && distToWall < fZDepth) {
				distToWall += 0.1f;
				int nTestX = (int)(playerX + fEyeX * distToWall);
				int nTestY = (int)(playerY + fEyeY * distToWall);
				if (nTestX < 0 || nTestX >= nMapWidth || nTestY < 0 || nTestY >= nMapWidth) {
					bHitWall = true;	// Если мы выходим за границы, то делаем расстояние равное максимальной глубине.
					distToWall = fZDepth;
				}
				else {
					if (map.c_str()[nTestY * nMapWidth + nTestX] == '#') {
						bHitWall = true;
					}
				}
			}
			int nCeiling = (float)(nScreenHeight / 2.0f) - nScreenHeight / ((float)distToWall);
			int nFloor = nScreenHeight - nCeiling;

			short farShade = ' ';// высчитываем тени в зависимости от того, как далеко мы от стены
			if (distToWall <= fZDepth / 4.0f)
				farShade = 0x2588;
			else if (distToWall < fZDepth / 3.0f)
				farShade = 0x2593;
			else if (distToWall < fZDepth / 1.0f)
				farShade = 0x2592;
			else if (distToWall < fZDepth)
				farShade = 0x2591;
			else
				farShade = ' ';

			for (int y = 0; y < nScreenHeight; y++)
			{
				if (y < nCeiling)
					screen[y * nScreenWidth + x] = ' '; // потолок
				else if (y > nCeiling && y <= nFloor)
					screen[y * nScreenWidth + x] = farShade; // стены
				else
					screen[y * nScreenWidth + x] = ' '; // пол


			}

			
		}
	screen[nScreenWidth * nScreenHeight - 1] = '\0';
	WriteConsoleOutputCharacter(hConsole, screen, nScreenWidth * nScreenHeight, { 0,0 }, &dwBytesWritten);
	}
	return 0;

}