#include "ComputerPlayer.h"

ComputerPlayer::ComputerPlayer(std::shared_ptr<TriasDataModel> model, TriasDataModel::Piece pc) : IPlayer(model, pc), distro_(0, 8)
{
}

ComputerPlayer::~ComputerPlayer()
{
}

bool ComputerPlayer::isHuman() const
{
	return false;
}

void ComputerPlayer::yourTurn(bool b)
{
	if (!b)
		return;

	if (checkMoveOntoBoard())
		return;

	moveRandom();
}

bool ComputerPlayer::checkMoveOntoBoard()
{
	int from = -1;
	int to = -1;

	for (int piece = firstPiece(); piece < firstPiece() + 3; piece++)
	{
		int pos = model()->findPiece(piece);
		if (pos >= firstOffBoardPos() && pos < firstOffBoardPos() + 3)
		{
			from = pos;
			break;
		}
	}

	if (from == -1)
		return false;

	//
	// Move the piece at pos, to a random open position
	//
	while (true)
	{
		int pos = distro_(engine_);
		if (!model()->isOccupied(pos))
		{
			to = pos;
			break;
		}
	}

	emit move(from, to);

	return true;
}

bool ComputerPlayer::moveRandom()
{
	int from = -1;
	int to = -1;

	for (int piece = firstPiece(); piece < firstPiece() + 3; piece++)
	{
		from = model()->findPiece(piece);
		if (from > TriasDataModel::BoardMaxPosition)
			continue;

		for(int i = 0 ; i < 8 ; i++)
		{
			int pos = distro_(engine_);
			if (!model()->isOccupied(pos) && model()->move(from, pos) == TriasDataModel::MoveError::Good)
			{
				to = pos;
				break;
			}
		}

		if (from != -1 && to != -1)
			break;
	}

	emit move(from, to);
	return true;
}