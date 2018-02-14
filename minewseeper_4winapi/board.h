#ifndef BOARD_HPP
#define BOARD_HPP

#include <vector>

class Cell
{
private:
	// bits 0-3 : value (0 - 8)
	// upper bits - flags
	unsigned char m_state;

	// Attributes of a cell, excluding the number of surrounding mines
	static constexpr int Covered = 0x10;
	static constexpr int Flagged = 0x20;
	static constexpr int    Mine = 0x40;

public:
	// value of a cell, that is the number of mines around it
	typedef unsigned char value_t;

	Cell()
	{
		Reset();
	}

	auto GetValue() const noexcept -> value_t
	{
		return m_state & 0x0F;
	}

	auto SetValue(value_t value) -> void
	{
		if (value > 8)
			throw(std::logic_error("Cell value cannot be greater than 8, at least not in a 2D world"));
		m_state = (m_state & 0xF0) | value;
	}

	auto IncreaseValue() -> void
	{
		SetValue(GetValue() + 1);
	}


	// Reset back to 0 value and make covered
	auto Reset() noexcept -> void
	{
		m_state = Covered;
	}
	
	auto IsMine() const noexcept -> bool { return m_state & Mine; }
	auto IsFlagged() const noexcept -> bool { return m_state & Flagged; }
	auto IsCovered() const noexcept-> bool { return m_state & Covered; }

	void SetMine() { m_state |= Mine; }
	void SetCovered(bool covered)
	{ 
		if (covered)
			m_state |= Covered;
		else
			m_state &= ~Covered;
	}

};



class Board
{
public:
	enum class State { Lost, NotStarted, Playing, Won };
	static const size_t MIN_SIZE= 9;
	static const size_t MIN_MINES = 9;
	static const size_t MAX_SIZE = 20;

protected:
	std::vector<std::vector<Cell> >grid;

	size_t width;
	size_t height;
	size_t n_mines;
	State game_state;

	void uncover_empty_cells(size_t begin_x, size_t begin_y);
	bool is_won() const;

	static const std::string save_filename;

public:
	Board();

	Board::State GetGameState() const;

	void NewGame();
	bool SetOptions(size_t width, size_t height, size_t n_mines);

	// Throw std::out_of_range exeception if x or y  value is too large
	const Cell &GetCell(size_t x, size_t y) const;

	void Uncover(size_t x, size_t y);

	auto GetWidth() const -> decltype(width);
	auto GetHeight() const -> decltype(height);

	void SaveGame();
	void LoadGame();

};



#endif
