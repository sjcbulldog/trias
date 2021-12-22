#pragma once

#include <QObject>
#include <array>

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

	void initBoard();


	MoveError move(int from, int to, bool domove = false);

	int maxPosNumber() const {
		return WhiteOffMaxPosition;
	}

	bool isOccupied(int pos) const {
		return isValidPieceNumber(board_[pos]);
	}

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

	static const int CenterLocation = 8;

private:
	std::array<int, 15> board_;
	std::array<int, 6> previous_;
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
