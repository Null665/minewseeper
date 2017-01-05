#ifndef BOARD_HPP
#define BOARD_HPP

#include <vector>

#define C_EMPTY     0
#define C_MINE     -1

#define G_LOST        0
#define G_PLAYING     1
#define G_WON         2
#define G_NOTSTARTED  3


#define MIN_WIDTH   9
#define MIN_HEIGHT  9
#define MIN_MINES   8

#define MAX_WIDTH   20
#define MAX_HEIGHT  20


class Cell
{
public:
	bool covered;
	int value;
	bool flagged;

	Cell()
	{
		covered = true;
		flagged = false;
		value = C_EMPTY;
	}

};



class Board
{
protected:
	std::vector<std::vector<Cell> >grid;

	int width;
	int height;
	int n_mines;
	int game_state;

	void uncover_empty_cells(int begin_x, int begin_y);
	bool is_won() const;

public:
	Board();

	int GetGameState() const;

	void NewGame();
	bool SetOptions(int width, int height, int n_mines);

	// these 2 functions throw std::out_of_range exeception if x or y  value is too large
	int GetCellValue(int x, int y) const;
	bool IsCoveredCell(int x, int y) const;

	void Uncover(int x, int y);

	int GetWidth() const;
	int GetHeight() const;

	void SaveGame();
	void LoadGame();
};



#endif
