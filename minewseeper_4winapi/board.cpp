#include "board.h"
#include <cstdlib>
#include <Windows.h>
using namespace std;


Board::Board()
{
	width = 9, height = 9;
	n_mines = 9;
	game_state = G_NOTSTARTED;
}

bool Board::SetOptions(int width, int height, int n_mines)
{
	if (width<MIN_WIDTH || height<MIN_HEIGHT || width>MAX_WIDTH || height>MAX_HEIGHT)
		return false;
	if (((width - 1)*(height - 1))<n_mines)
		return false;
	if (n_mines<MIN_MINES)
		return false;

	this->width = width;
	this->height = height;
	this->n_mines = n_mines;

	return true;
}

int Board::GetGameState() const
{
	return game_state;
}


void Board::NewGame()
{
	grid.clear();
	game_state = G_NOTSTARTED;

	for (int i = 0;i<width;i++)
	{
		grid.push_back(vector<Cell>());
		for (int j = 0;j<height;j++)
			grid[i].push_back(Cell());
	}


	for (int i = 0;i<n_mines;i++)
	{
		int rx = rand() % (width - 1);
		int ry = rand() % (height - 1);

		if (grid[rx][ry].value == C_MINE)
		{
			i--;
			continue;
		}

		grid[rx][ry].value = C_MINE;
	}

	// Put numbers around mines
	for (int i = 0;i<width - 1;i++)
		for (int j = 0;j<height - 1;j++)
		{
			if (grid[i][j].value == C_MINE)
			{
				if (i>0)
					if (grid[i - 1][j].value != C_MINE)
						grid[i - 1][j].value++;
				if (i<width - 1)
					if (grid[i + 1][j].value != C_MINE)
						grid[i + 1][j].value++;
				
				if(j>0)
					if (grid[i][j - 1].value != C_MINE)
						grid[i][j - 1].value++;

				if (grid[i][j + 1].value != C_MINE && j<height - 1)
					grid[i][j + 1].value++;

				if (i>0 && j>0)
					if (grid[i - 1][j - 1].value != C_MINE)
						grid[i - 1][j - 1].value++;
				if (j<height - 1 && i<width - 1)
					if (grid[i + 1][j + 1].value != C_MINE)
						grid[i + 1][j + 1].value++;
				if (i<width - 1 && j>0)
					if (grid[i + 1][j - 1].value != C_MINE)
						grid[i + 1][j - 1].value++;
				if (i>0 && j<height - 1)
					if (grid[i - 1][j + 1].value != C_MINE)
						grid[i - 1][j + 1].value++;
			}
		}
}

bool Board::IsCoveredCell(int x, int y) const
{
	return grid.at(x).at(y).covered;
}

int Board::GetCellValue(int x, int y) const
{
	return grid.at(x).at(y).value;
}

void Board::Uncover(int x, int y)
{
	if (game_state == G_NOTSTARTED)
		game_state = G_PLAYING;

	if (x >= GetWidth() || y >= GetHeight() || x<0 || y<0)
		return;

	grid[x][y].covered = false;

	if (grid[x][y].value == C_MINE)
	{
		game_state = G_LOST;
		return;
	}
	if (is_won())
	{
		game_state = G_WON;
		return;
	}


	uncover_empty_cells(x, y);
}

int Board::GetWidth() const
{
	return width;
}

int Board::GetHeight() const
{
	return height;
}


void Board::SaveGame()
{

	HANDLE hFile = CreateFile("game.txt", GENERIC_WRITE, FILE_SHARE_READ, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
	if (hFile == INVALID_HANDLE_VALUE)
		return;

	DWORD dwWritten;
	WriteFile(hFile, &width, sizeof(width), &dwWritten, 0);
	WriteFile(hFile, &height, sizeof(height), &dwWritten, 0);
	WriteFile(hFile, &n_mines, sizeof(n_mines), &dwWritten, 0);

	CloseHandle(hFile);
}



void Board::LoadGame()
{
	HANDLE hFile = CreateFile("game.txt", GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

	DWORD dwRead;
	ReadFile(hFile, &width, sizeof(width), &dwRead, 0);
	ReadFile(hFile, &height, sizeof(height), &dwRead, 0);
	ReadFile(hFile, &n_mines, sizeof(n_mines), &dwRead, 0);

	CloseHandle(hFile);
}



void Board::uncover_empty_cells(int begin_x, int begin_y)
{
	int x = begin_x, y = begin_y;

	grid[x][y].covered = false;

	if (GetCellValue(x, y) != C_EMPTY)
		return;

	if (begin_x>0)
	{
		if (IsCoveredCell(x - 1, y))
			uncover_empty_cells(x - 1, y);
	}
	if (begin_x<GetWidth() - 1)
	{
		if (IsCoveredCell(x + 1, y))
			uncover_empty_cells(x + 1, y);
	}

	if (begin_y>0)
	{
		if (IsCoveredCell(x, y - 1))
			uncover_empty_cells(x, y - 1);
	}
	if (begin_y<GetHeight() - 1)
	{
		if (IsCoveredCell(x, y + 1))
			uncover_empty_cells(x, y + 1);
	}

}

bool Board::is_won() const
{
	for (int i = 0;i<GetWidth();i++)
		for (int j = 0;j<GetHeight();j++)
			if (grid[i][j].covered == true && (grid[i][j].value >= 1 && grid[i][j].value <= 8 || grid[i][j].value == C_EMPTY))
				return false;
	return true;
}
