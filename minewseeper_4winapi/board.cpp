#include "board.h"
#include <cstdlib>
#include <fstream>
#include <random>
using namespace std;

const std::string Board::save_filename = "game.txt";

Board::Board()
{
	width = 9, height = 9;
	n_mines = 9;
	game_state = State::NotStarted;
}

bool Board::SetOptions(size_t width, size_t height, size_t n_mines)
{
	if (width<MIN_SIZE || height<MIN_SIZE || width>MAX_SIZE || height>MAX_SIZE)
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

Board::State Board::GetGameState() const
{
	return game_state;
}


void Board::NewGame()
{
	grid.clear();
	game_state = State::NotStarted;

	// Construct the game grid
	for (auto i = 0; i < width; i++)
	{
		grid.emplace_back(vector<Cell>());
		for (auto j = 0; j < height; j++)
			grid[i].emplace_back(Cell());
	}

	// Randomly place mines
	std::random_device rnd;
	mt19937 mt(rnd());
	uniform_int_distribution<size_t> ud_width(0, width - 1);
	uniform_int_distribution<size_t> ud_height(0, height - 1);

	for (auto i = 0;i<n_mines;i++)
	{
		auto rx = ud_width(mt);
		auto ry = ud_height(mt);
		// cannot place two mines in one cell
		if (grid.at(rx).at(ry).IsMine())
		{
			i--;
			continue;
		}
		grid.at(rx).at(ry).SetMine();
	}

	// Put numbers around mines
	for (auto i = 0;i<width;i++)
		for (auto j = 0;j<height;j++)
		{
			if (grid[i][j].IsMine())
			{
				if (i>0)
					if (grid[i - 1][j].IsMine() == false)
						grid[i - 1][j].IncreaseValue();
				if (i<width - 1)
					if (grid[i + 1][j].IsMine() == false)
						grid[i + 1][j].IncreaseValue();
				
				if(j>0)
					if (grid[i][j - 1].IsMine() == false)
						grid[i][j - 1].IncreaseValue();

				if (j<height - 1)
					if (grid[i][j + 1].IsMine() == false)
						grid[i][j + 1].IncreaseValue();

				if (i>0 && j>0)
					if (grid[i - 1][j - 1].IsMine() == false)
						grid[i - 1][j - 1].IncreaseValue();
				if (j<height - 1 && i<width - 1)
					if (grid[i + 1][j + 1].IsMine() == false)
						grid[i + 1][j + 1].IncreaseValue();
				if (i<width - 1 && j>0)
					if (grid[i + 1][j - 1].IsMine() == false)
						grid[i + 1][j - 1].IncreaseValue();
				if (i>0 && j<height - 1)
					if (grid[i - 1][j + 1].IsMine() == false)
						grid[i - 1][j + 1].IncreaseValue();
			}
		}
}

const Cell &Board::GetCell(size_t x, size_t y) const
{
	return grid.at(x).at(y);
}

void Board::Uncover(size_t x, size_t y)
{
	if (game_state == State::NotStarted)
		game_state = State::Playing;

	if (x >= GetWidth() || y >= GetHeight())
		return;

	grid[x][y].SetCovered(false);

	if (grid[x][y].IsMine())
	{
		game_state = State::Lost;
		return;
	}

	uncover_empty_cells(x, y);

	if (is_won())
	{
		game_state = State::Won;
		return;
	}
}

auto Board::GetWidth() const -> decltype(width)
{
	return width;
}

auto Board::GetHeight() const -> decltype(height)
{
	return height;
}


void Board::SaveGame()
{
	ofstream file(save_filename, ios::out);
	if (file)
	{
		file << width << " " << height << " " << n_mines;
		file.close();
	}
}

void Board::LoadGame()
{
	ifstream file(save_filename, ios::in);
	if (file)
	{
		file >> width >> height >> n_mines;
		file.close();
	}
}


// Uncovers empty cells recursively
// Nonrecursive code would have an advantage tha it wouldn't be limited by stack size
void Board::uncover_empty_cells(size_t x, size_t y)
{
	// If cell contains a value, it must not be uncovered
	if (GetCell(x, y).IsMine())
		return;

	grid[x][y].SetCovered(false);

	// If cell contains a value, it must not be uncovered
	if (GetCell(x, y).GetValue())
		return;

	// Recursively uncover surrounding cells
	if (x > 0)
	{
		if (GetCell(x - 1, y).IsCovered())
			uncover_empty_cells(x - 1, y);
	}
	if (x < GetWidth() - 1)
	{
		if (GetCell(x + 1, y).IsCovered())
			uncover_empty_cells(x + 1, y);
	}
	if (y > 0)
	{
		if (GetCell(x, y - 1).IsCovered())
			uncover_empty_cells(x, y - 1);
	}
	if (y < GetHeight() - 1)
	{
		if (GetCell(x, y + 1).IsCovered())
			uncover_empty_cells(x, y + 1);
	}

}

bool Board::is_won() const
{
	for (int i = 0;i<GetWidth();i++)
		for (int j = 0;j<GetHeight();j++)
			if (grid[i][j].IsCovered() && !grid[i][j].IsMine() )
				return false;
	return true;
}
