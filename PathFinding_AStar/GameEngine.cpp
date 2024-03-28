#include <iostream>
#include <Windows.h>

using namespace std;

int main()
{

	wchar_t* screen = new wchar_t[80 * 30];
	HANDLE hConsole = CreateConsoleScreenBuffer(GENERIC_READ|GENERIC_WRITE,0,NULL,CONSOLE_TEXTMODE_BUFFER,NULL);
	SetConsoleActiveScreenBuffer(hConsole);
	DWORD dwBytesWritten = 0;
	

	while (1)
	{
		screen[10 * 80 + 15] = L'P';
	}
}
