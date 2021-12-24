#include "ComputerPlayer.h"

ComputerPlayer::ComputerPlayer(std::shared_ptr<TriasDataModel> model, TriasDataModel::Piece pc) : IPlayer(model, pc), distro8_(0, 8), distro2_(0, 2)
{
}

ComputerPlayer::~ComputerPlayer()
{
}

bool ComputerPlayer::isHuman() const
{
	return false;
}

void ComputerPlayer::yourTurn()
{
	if (model()->hasPiecesOffBoard(piece()))
	{
		moveOntoBoard();
	}
	else
	{
		moveWithinBoard();
	}
}

void ComputerPlayer::moveOntoBoard()
{
	int from = -1, to = -1;

	//
	// Find the piece off the board to move
	//
	for (int piece = firstPiece(); piece < firstPiece() + 3; piece++)
	{
		int pos = model()->findPiece(piece);
		if (pos >= firstOffBoardPos() && pos < firstOffBoardPos() + 3)
		{
			from = pos;
			break;
		}
	}

	//
	// Now, see if we can move off the board to win
	//
	std::vector<std::array<int, 3>> towin = model()->searchForTwo(piece());
	if (towin.size() > 0)
	{
		const auto& entry = towin[0];
		to = entry[0];
	}

	if (to == -1)
	{
		//
		// Now, see if we can move off the board to block
		//
		std::vector<std::array<int, 3>> toblock= model()->searchForTwo(otherPiece());
		if (toblock.size() > 0)
		{
			const auto& entry = toblock[0];
			to = entry[0];
		}
	}

	if (to == -1)
	{
		to = findRandomLocation();
	}

	assert(from != -1 && to != -1);
	emit move(from, to);
}

void ComputerPlayer::moveWithinBoard()
{
	int to = -1, from = -1;

	//
	// First check for a move to win the game
	//
	std::vector<std::array<int, 3>> towin = model()->searchForTwo(piece());
	if (towin.size() > 0)
	{
		for (int i = 0; i < towin.size(); i++)
		{
			const auto& entry = towin[i];
			int pos = model()->findFrom(entry[0], piece());
			if (pos != -1)
			{
				from = pos;
				to = entry[0];
				break;
			}
		}
	}

	//
	// Now check for a move to block the other side
	//
	std::vector<std::array<int, 3>> toblock = model()->searchForTwo(otherPiece());
	if (toblock.size() > 0)
	{
		for (int i = 0; i < toblock.size(); i++)
		{
			const auto& entry = toblock[i];
			int pos = model()->findFrom(entry[0], piece());
			if (pos != -1)
			{
				from = pos;
				to = entry[0];
				break;
			}
		}
	}

	//
	// Now just find a random move
	//
	if (to == -1 || from == -1)
	{
		std::array<int, 3> pieces;
		for (int i = 0; i < pieces.size(); i++)
		{
			pieces[i] = firstPiece() + i;
		}

		for (int i = 0; i < 10; i++)
		{
			int one = distro2_(engine_);
			int two = distro2_(engine_);
			std::swap(pieces[one], pieces[two]);
		}

		for (int i = 0; i < pieces.size(); i++)
		{
			int pos = model()->findPiece(pieces[i]);
			std::vector<int> adj = model()->adjacent(pos);

			for (int j = 0; j < adj.size(); j++)
			{
				if (model()->isValidMove(pieces[i], pos, adj[j]))
				{
					from = pos;
					to = adj[j];
					break;
				}
			}

			if (from != -1 && to != -1)
				break;
		}
	}

	assert(from != -1 && to != -1);
	emit move(from, to);
}

int ComputerPlayer::findRandomLocation()
{
	int ret = -1;
	std::array<int, 9> spots;
	
	for (int i = 0; i < spots.size(); i++)
		spots[i] = i;

	for (int i = 0; i < 128; i++)
	{
		int first = distro8_(engine_);
		int second = distro8_(engine_);
		std::swap(spots[first], spots[second]);
	}

	for (int i = 0; i < spots.size(); i++)
	{
		if (!model()->isOccupied(spots[i]))
		{
			ret = spots[i];
			break;
		}
	}

	return ret;
}
