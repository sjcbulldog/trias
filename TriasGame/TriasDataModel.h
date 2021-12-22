#pragma once

#include <QObject>
#include <array>
#include <stack>

class TriasDataModel : public QObject
{
	Q_OBJECT

public:
	TriasDataModel(QObject *parent);
	~TriasDataModel();

public:
	enum class Piece {
		None,
		Black,
		White
	};

	enum class MoveError {
		Good,
		InvalidDestination,
		LocationOccupied,
		NotAdjacent,
		BackAgain
	};

	void push() {
		board_stack_.push(board_);
		previous_stack_.push(previous_);
	}

	void pop() {
		board_ = board_stack_.top();
		board_stack_.pop();

		previous_ = previous_stack_.top();
		previous_stack_.pop();
	}

	int findPiece(int piece) const {
		for (int pos = 0; pos <= maxPosNumber(); pos++)
		{
			if (board(pos) == piece)
				return pos;
		}

		return -1;
	}

	int board(int pos) const {
		return board_[pos];
	}

	Piece boardPiece(int pos) const {
		if (board_[pos] == -1)
			return Piece::None;

		return pieceNumberToType(board_[pos]);
	}

	std::vector<int> adjacent(int from) {
		std::vector<int> ret;

		if (from == 8)
		{
			for (int i = 0; i < 8; i++)
				ret.push_back(i);
		}
		else
		{
			ret.push_back((from == 0) ? 7 : from - 1);
			ret.push_back((from == 7) ? 0 : from + 1);
			ret.push_back(8);
		}
		return ret;
	}

	void initBoard();


	MoveError move(int from, int to, bool domove = false);

	int maxPosNumber() const {
		return WhiteOffMaxPosition;
	}

	bool isOccupied(int pos) const {
		return isValidPieceNumber(board_[pos]);
	}

	static std::vector<std::array<int, 3>>& winningCombinations() {
		return winning_combinations_;
	}

	bool win(TriasDataModel::Piece pc);

public:
	static constexpr const int BoardMaxPosition = 8;
	static constexpr const int BlackOffMinPosition = 9;
	static constexpr const int BlackOffMaxPosition = 11;
	static constexpr const int WhiteOffMinPosition = 12;
	static constexpr const int WhiteOffMaxPosition = 14;

	static constexpr const int NoPieceNumber = -1;
	static constexpr const int FirstBlackPieceNumber = 0;
	static constexpr const int LastBlackPieceNumber = 2;
	static constexpr const int FirstWhitePieceNumber = 3;
	static constexpr const int LastWhitePieceNumber = 5;

signals:
	void changed();

private:
	static const int CenterLocation = 8;

private:
	static Piece pieceNumberToType(int num) {
		Piece p = Piece::None;

		assert(num >= 0 && num <= 5);

		if (num >= FirstBlackPieceNumber && num <= LastBlackPieceNumber)
			p = Piece::Black;
		else if (num >= FirstWhitePieceNumber && num <= LastWhitePieceNumber)
			p = Piece::White;

		return p;
	}

	static bool isValidPieceNumber(int num) {
		return (num >= FirstBlackPieceNumber && num <= LastBlackPieceNumber) ||
			(num >= FirstWhitePieceNumber && num <= LastWhitePieceNumber);
	}

	bool isAdjacent(int from, int to) const;

	static bool isOff(int pos) {
		return (pos >= WhiteOffMinPosition && pos <= WhiteOffMaxPosition) ||
			(pos >= BlackOffMinPosition && pos <= BlackOffMaxPosition);
	}

private:
	std::array<int, 15> board_;
	std::array<int, 6> previous_;

	std::stack<std::array<int, 15>> board_stack_;
	std::stack<std::array<int, 6>> previous_stack_;

	static std::vector<std::array<int, 3>> winning_combinations_;

};

static QString toString(TriasDataModel::Piece pc)
{
	QString ret;

	switch(pc)
	{ 
	case TriasDataModel::Piece::None:
		ret = "None";
		break;
	case TriasDataModel::Piece::Black:
		ret = "Black";
		break;
	case TriasDataModel::Piece::White:
		ret = "White";
		break;
	}

	return ret;
}
