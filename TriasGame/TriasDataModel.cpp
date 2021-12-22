#include "TriasDataModel.h"

TriasDataModel::TriasDataModel(QObject *parent) : QObject(parent)
{
	initBoard();
}

TriasDataModel::~TriasDataModel()
{
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

	std::fill(previous_.begin(), previous_.end(), -1);

	emit changed();
}

TriasDataModel::MoveError TriasDataModel::move(int from, int to, bool domove)
{
	assert(to >= 0 && to < board_.size());
	assert(from >= 0 && from < board_.size());
	assert(isOccupied(from));

	int piece = board(from);

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
	else if (previous_[piece] == to)
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
				previous_[piece] = from;
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
