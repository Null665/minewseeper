#define WINVER 0x501

#include <windows.h>
#include <ctime>
#include <cstdlib>
#include <windowsx.h>
#include "resource.h"

#include "board.h"
using namespace std;


Board b;
char szClassName[] = "MinesweeperApp";

LRESULT CALLBACK WindowProcedure(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK DlgOptionsProc(HWND, UINT, WPARAM, LPARAM);

void AdjustWindowSize(HWND hwnd, const Board &b, int cell_size, int border);


void DrawBoard(HDC hdc, Board &b)
{

	HBITMAP hbitmaps[11];
	HBRUSH  hbrushes[11];

	hbitmaps[0] = LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_CELL_EMPTY)); // tuscia
	hbitmaps[1] = LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_CELL_1));
	hbitmaps[2] = LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_CELL_2));
	hbitmaps[3] = LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_CELL_3));
	hbitmaps[4] = LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_CELL_4));

	hbitmaps[5] = LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_CELL_5));
	hbitmaps[6] = LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_CELL_6));
	hbitmaps[7] = LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_CELL_7));
	hbitmaps[8] = LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_CELL_8));

	hbitmaps[9] = LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_CELL));
	hbitmaps[10] = LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_CELL_MINE));


	for (int i = 0;i<11;i++)
		hbrushes[i] = CreatePatternBrush(hbitmaps[i]);


	for (int i = 0;i<b.GetWidth();i++)
	{
		for (int j = 0;j<b.GetHeight();j++)
		{
			if (b.IsCoveredCell(i, j))
				SelectObject(hdc, hbrushes[9]);
			else if (b.GetCellValue(i, j) == C_EMPTY)
				SelectObject(hdc, hbrushes[0]);
			else if (b.GetCellValue(i, j) >= 1 && b.GetCellValue(i, j) <= 8)
				SelectObject(hdc, hbrushes[b.GetCellValue(i, j)]);
			else if (b.GetCellValue(i, j) == C_MINE)
				SelectObject(hdc, hbrushes[10]);

			int size, border;
			size = border = 20;
			Rectangle(hdc, border + (i*size), border + (j*size), border + (i*size) + size, border + (j*size) + size);

		}
	}

	for (int i = 0;i<11;i++)
	{
		SelectObject(hdc, hbrushes[i]);
		DeleteObject(hbrushes[i]);
		DeleteObject(hbitmaps[i]);
	}

}


int WINAPI WinMain(HINSTANCE hThisInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpszArgument,
	int nCmdShow)
{
	srand(time(0));

	HWND hwnd;               /* This is the handle for our window */
	MSG messages;            /* Here messages to the application are saved */
	WNDCLASSEX wincl;        /* Data structure for the windowclass */

							 /* The Window structure */
	wincl.hInstance = hThisInstance;
	wincl.lpszClassName = szClassName;
	wincl.lpfnWndProc = WindowProcedure;      /* This function is called by windows */
	wincl.style = CS_DBLCLKS;                 /* Catch double-clicks */
	wincl.cbSize = sizeof(WNDCLASSEX);

	/* Use default icon and mouse-pointer */
	wincl.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wincl.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	wincl.hCursor = LoadCursor(NULL, IDC_ARROW);
	wincl.lpszMenuName = NULL;                 /* No menu */
	wincl.cbClsExtra = 0;                      /* No extra bytes after the window class */
	wincl.cbWndExtra = 0;                      /* structure or the window instance */

	wincl.hbrBackground = (HBRUSH)COLOR_BACKGROUND;

	if (!RegisterClassEx(&wincl))
		return 0;

	hwnd = CreateWindowEx(
		0,                   /* Extended possibilites for variation */
		szClassName,         /* Classname */
		"Minesweeper",       /* Title Text */
		WS_OVERLAPPEDWINDOW, /* default window */
		CW_USEDEFAULT,       /* Windows decides the position */
		CW_USEDEFAULT,       /* where the window ends up on the screen */
		544,                 /* The programs width */
		375,                 /* and height in pixels */
		HWND_DESKTOP,        /* The window is a child-window to desktop */
		NULL,                /* No menu */
		hThisInstance,       /* Program Instance handler */
		NULL                 /* No Window Creation data */
	);
	HMENU hmenu = LoadMenu(hThisInstance, MAKEINTRESOURCE(IDR_MENU));
	SetMenu(hwnd, hmenu);

	ShowWindow(hwnd, nCmdShow);
	while (GetMessage(&messages, NULL, 0, 0))
	{
		TranslateMessage(&messages);
		DispatchMessage(&messages);
	}
	return messages.wParam;
}

// ---------------------------------------------------------------------------


LRESULT CALLBACK WindowProcedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	HDC         hdc;
	PAINTSTRUCT ps;

	const int size = 20;
	const int border = 20;

	switch (message)
	{
	case WM_CREATE:
	{
		//b.SetOptions(15, 15, 10);
		b.LoadGame();
		b.NewGame();

		AdjustWindowSize(hwnd, b, size, border);
		return true;
	}
	case WM_PAINT:
		hdc = BeginPaint(hwnd, &ps);
		DrawBoard(hdc, b);
		EndPaint(hwnd, &ps);
		break;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDM_QUIT:
			PostQuitMessage(0);
			break;
		case IDM_NEW_GAME:
			b.NewGame();
			InvalidateRect(hwnd, NULL, TRUE);
			UpdateWindow(hwnd);
			break;
		case IDM_OPTIONS:
			DialogBox(GetModuleHandle(0), MAKEINTRESOURCE(IDD_OPTIONS), hwnd, DlgOptionsProc);
			b.NewGame();

			AdjustWindowSize(hwnd, b, size, border);
			InvalidateRect(hwnd, NULL, TRUE);
			UpdateWindow(hwnd);
			break;
		}
		break;
	case WM_LBUTTONDOWN:
		int cx, cy;
		cx = GET_X_LPARAM(lParam);
		cy = GET_Y_LPARAM(lParam);

		cx = (cx - border) / size;
		cy = (cy - border) / size;

		b.Uncover(cx, cy);


		RECT r;
		r.top = (cy + border)*size;
		r.left = (cx + border)*size;
		r.bottom = size;
		r.right = size;

		InvalidateRect(hwnd, &r, TRUE);
		UpdateWindow(hwnd);

		if (b.GetGameState() == G_LOST)
		{
			if (MessageBox(hwnd, "You lost! Play again?", "Boom", 64 | MB_YESNO) != IDYES)
				DestroyWindow(hwnd);
			else
			{
				b.NewGame();
				InvalidateRect(hwnd, NULL, TRUE);
				UpdateWindow(hwnd);
			}
		}
		else if (b.GetGameState() == G_WON)
		{
			if (MessageBox(hwnd, "You won! Play again?", "Congrats", 64 | MB_YESNO) != IDYES)
				DestroyWindow(hwnd);
			else
			{
				b.NewGame();
				InvalidateRect(hwnd, NULL, TRUE);
				UpdateWindow(hwnd);
			}
		}
		break;

	case WM_DESTROY:
		b.SaveGame();
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hwnd, message, wParam, lParam);
	}

	return 0;
}

INT_PTR CALLBACK DlgOptionsProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_INITDIALOG:
	{
		SetDlgItemInt(hwnd, EDT_WIDTH, 10, 0);
		SetDlgItemInt(hwnd, EDT_HEIGHT, 10, 0);
		SetDlgItemInt(hwnd, EDT_MINES, 9, 0);
	}
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDCANCEL:
			EndDialog(hwnd, IDD_OPTIONS);
			break;
		case IDOK:
		{
			int width = GetDlgItemInt(hwnd, EDT_WIDTH, 0, 0);
			int height = GetDlgItemInt(hwnd, EDT_HEIGHT, 0, 0);
			int n_mines = GetDlgItemInt(hwnd, EDT_MINES, 0, 0);

			if (b.SetOptions(width, height, n_mines) == false)
			{
				MessageBox(hwnd, "Width and Height must be less than 21."
					"Maximum number of mines is (width-1)*(height-1)", "Error", 48);
				break;
			}

			b.NewGame();

			EndDialog(hwnd, IDD_OPTIONS);
			break;
		}
		}
		break;
	}
	return false;
}

void AdjustWindowSize(HWND hwnd, const Board &b, int cell_size, int border)
{
	RECT r;
	GetWindowRect(hwnd, &r);
	int width = (b.GetWidth()*cell_size) + (border * 3);
	int height = (b.GetHeight()*cell_size) + (border * 5);
	MoveWindow(hwnd, r.left, r.top, width, height, 0);
}
