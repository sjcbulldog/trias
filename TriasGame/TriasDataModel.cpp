#include "TriasDataModel.h"

std::vector<std::array<int, 3>> TriasDataModel::winning_combinations_ =
{
	{0, 1, 2},
	{1, 2, 3},
	{2, 3, 4},
	{3, 4, 5},
	{4, 5, 6},
	{5, 6, 7},
	{6, 7, 0},
	{7, 0, 1},
	{0, 8, 4},
	{1, 8, 5},
	{2, 8, 6},
	{3, 8, 7}
};

TriasDataModel::TriasDataModel(QObject *parent) : QObject(parent)
{
	initBoard();
}

TriasDataModel::~TriasDataModel()
{
}

QString TriasDataModel::toString()
{
	QString ret;

	for (int i = 0; i < board_.size(); i++)
	{
		if (i != 0)
			ret += " ";
		ret += QString::number(i) + ":" + QString::number(board_[i]);
	}

	return ret;
}

bool TriasDataModel::hasPiecesOffBoard(Piece pc)
{
	int first, last;

	getPieceRange(pc, first, last);
	for (int i = first; i <= last; i++)
	{
		int pos = findPiece(i);
		if (isOff(pos))
			return true;
	}

	return false;
}

std::vector<std::array<int, 3>> TriasDataModel::searchForTwo(Piece pc)
{
	std::vector<std::array<int, 3>> result;

	for (const std::array<int, 3> &one : winning_combinations_)
	{
		int sum = 0;
		for (int i = 0; i < one.size(); i++)
		{
			if (boardPiece(one[i]) == pc)
				sum++;
			else if (isOccupied(one[i]))
				sum--;
		}

		if (sum == 2)
		{
			std::array<int, 3> entry;

			if (boardPiece(one[0]) == Piece::None)
			{
				entry[0] = one[0];
				entry[1] = one[1];
				entry[2] = one[2];
			}
			else if (boardPiece(one[1]) == Piece::None)
			{
				entry[0] = one[1];
				entry[1] = one[0];
				entry[2] = one[2];
			}
			else if (boardPiece(one[2]) == Piece::None)
			{
				entry[0] = one[2];
				entry[1] = one[0];
				entry[2] = one[1];
			}
			result.push_back(entry);
		}
	}

	return result;
}

void TriasDataModel::getPieceRange(Piece pc, int& first, int& last)
{
	if (pc == Piece::Black)
	{
		first = FirstBlackPieceNumber;
		last = LastBlackPieceNumber;
	}
	else if (pc == Piece::White)
	{
		first = FirstWhitePieceNumber;
		last = LastWhitePieceNumber;
	}
	else
	{
		assert(false);
	}
}

bool TriasDataModel::isValidMove(int pc, int from, int to)
{
	if (isOccupied(to))
		return false;

	if (from < BlackOffMinPosition && !isAdjacent(from, to))
		return false;

	const std::pair<int, int> prev = previous_[pieceNumberToIndex(pc)];
	if (prev.first == pc && prev.second == to)
		return false;

	return true;
}

int TriasDataModel::findFrom(int to, Piece pc)
{
	int first, last;

	//
	// Check to see if there are pieces that are off the
	// board and can be moved to any spot
	//
	getPieceRange(pc, first, last);
	for (int i = first; i <= last; i++)
	{
		int pos = findPiece(i);
		if (isOff(pos))
			return pos;
	}

	//
	// Now check to see if there is a piece that is valid to move
	// to the desired location
	//
	for (int i = first; i <= last; i++)
	{
		int pos = findPiece(i);
		if (isValidMove(i, pos, to))
			return pos;
	}

	return -1;
}

void TriasDataModel::initBoard()
{
	std::fill(board_.begin(), board_.end(), NoPieceNumber);

	board_[BlackOffMinPosition] = FirstBlackPieceNumber;
	board_[BlackOffMinPosition + 1] = FirstBlackPieceNumber + 1;
	board_[BlackOffMinPosition + 2] = FirstBlackPieceNumber + 2;

	board_[WhiteOffMinPosition] = FirstWhitePieceNumber;
	board_[WhiteOffMinPosition + 1] = FirstWhitePieceNumber + 1;
	board_[WhiteOffMinPosition + 2] = FirstWhitePieceNumber + 2;

	previous_[0] = std::make_pair(-1, -1);
	previous_[1] = std::make_pair(-1, -1);

	emit changed();
}


bool TriasDataModel::win(TriasDataModel::Piece pc)
{
	for (int i = 0; i < winning_combinations_.size(); i++)
	{
		bool check = true;

		const std::array<int, 3>& comb = winning_combinations_[i];
		for (int j = 0; j < comb.size(); j++)
		{
			if (boardPiece(comb[j]) != pc)
			{
				check = false;
				break;
			}
		}

		if (check)
			return true;
	}

	return false;
}

TriasDataModel::MoveError TriasDataModel::move(int from, int to, bool domove)
{
	assert(to >= 0 && to < board_.size());
	assert(from >= 0 && from < board_.size());
	assert(isOccupied(from));

	int piece = board(from);
	const std::pair<int, int> prev = previous_[pieceNumberToIndex(piece)];

	MoveError ret = MoveError::Good;

	if (isOff(to))
	{
		//
		// Cannot move to an off board position
		//
		ret = MoveError::InvalidDestination;
	}
	else if (isOccupied(to))
	{
		ret = MoveError::LocationOccupied;
	}
	else if (prev.first == piece && prev.second == to)
	{
		ret = MoveError::BackAgain;
	}
	else
	{
		if (isAdjacent(from, to) || isOff(from))
		{
			//
			// Either we are moving from an off board position, or the from and to
			// positions are adjacent
			//
			if (domove)
			{
				board_[to] = board_[from];
				board_[from] = -1;
				previous_[pieceNumberToIndex(piece)] = std::make_pair(piece, from);
				emit changed();
			}
		}
		else
		{
			ret = MoveError::NotAdjacent;
		}
	}

	return ret;
}

bool TriasDataModel::isAdjacent(int from, int to) const
{
	if (from == CenterLocation || to == CenterLocation)
		return true;

	if (std::abs(from - to) == 1)
		return true;

	if (from == 7 && to == 0)
		return true;

	if (from == 0 && to == 7)
		return true;

	return false;
}
