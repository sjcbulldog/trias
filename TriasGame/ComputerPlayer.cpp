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
	if (moveOntoBoard())
		return;

	if (moveWithinBoard())
		return;

	assert(false);
}

int ComputerPlayer::doesCauseWin(int from, const std::vector<int>& adjacent)
{
	int to = -1;

	for (int i = 0; i < adjacent.size() && to == -1; i++)
	{
		model()->push();
		model()->move(from, adjacent[i]);
		
		if (model()->win(piece()))
			to = adjacent[i];

		model()->pop();
	}

	return to;
}

static int x = 0;
bool ComputerPlayer::moveWithinBoard()
{
	int from = -1;
	int to = -1;

	x++;

	//
	// Check first for ways to win
	//
	for (int piece = firstPiece(); piece < firstPiece() + 3; piece++)
	{
		from = model()->findPiece(piece);
		if (from > TriasDataModel::BoardMaxPosition)
			continue;

		std::vector<int> adjacent = model()->adjacent(from);

		//
		// Can I move this piece to a place to win
		//
		to = doesCauseWin(from, adjacent);
		if (to != -1)
			break;
	}

	if (to == -1)
	{
		//
		// Now check for ways to block
		//
		for (int piece = firstPiece(); piece < firstPiece() + 3; piece++)
		{
			from = model()->findPiece(piece);
			if (from > TriasDataModel::BoardMaxPosition)
				continue;

			std::vector<int> adjacent = model()->adjacent(from);
		}
	}

	if (to == -1)
	{
		//
		// Finally, find a random valid location
		//
		for (int piece = firstPiece(); piece < firstPiece() + 3; piece++)
		{
			from = model()->findPiece(piece);
			if (from > TriasDataModel::BoardMaxPosition)
				continue;

			std::vector<int> adjacent = model()->adjacent(from);

			for (int i = 0; i < adjacent.size() * 2; i++)
			{
				int first = distro2_(engine_);
				int second = distro2_(engine_);
				std::swap(adjacent[first], adjacent[second]);
			}

			for (int i = 0; i < adjacent.size(); i++)
			{
				if (!model()->isOccupied(adjacent[i]))
				{
					to = adjacent[i];
					break;
				}
			}

			if (from != -1 && to != -1)
				break;
		}
	}

	assert(from != -1 && to != -1);
	emit move(from, to);
	return true;
}

bool ComputerPlayer::moveOntoBoard()
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

	to = findBlockWinLocation(true);
	if (to == -1)
	{
		to = findBlockWinLocation(false);
		if (to == -1)
			to = findRandomLocation();
	}

	assert(from != -1 && to != -1);
	emit move(from, to);

	return true;
}

int ComputerPlayer::findBlockWinLocation(bool win)
{
	const auto& wins = model()->winningCombinations();
	TriasDataModel::Piece me = piece();
	TriasDataModel::Piece them = otherPiece();
	int desired = win ? 2 : -2;

	for (int i = 0; i < wins.size(); i++)
	{
		const auto& one = wins[i];

		int sum = 0;
		for (int j = 0; j < one.size(); j++) 
		{
			if (model()->boardPiece(one[j]) == me)
				sum++;
			else if (model()->boardPiece(one[j]) == them)
				sum--;
		}

		if (sum == desired)
		{
			//
			// We need to block
			//
			for (int j = 0; j < one.size(); j++)
			{
				if (model()->boardPiece(one[j]) == TriasDataModel::Piece::None)
					return one[j];
			}
		}
	}

	return -1;
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
