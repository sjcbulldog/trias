#pragma once
#include "IPlayer.h"
#include <random>


class ComputerPlayer : public IPlayer
{
	Q_OBJECT

public:
	ComputerPlayer(std::shared_ptr<TriasDataModel> model, TriasDataModel::Piece pc);
	virtual ~ComputerPlayer();

	bool isHuman() const override;
	void yourTurn(bool b) override;

private:
	bool moveOntoBoard();
	bool moveWithinBoard();

	int findBlockWinLocation(bool win);
	int findRandomLocation();

	int doesCauseWin(int from, const std::array<int, 3>& adjacent);

private:
	std::default_random_engine engine_;
	std::uniform_int_distribution<std::mt19937::result_type> distro8_;
	std::uniform_int_distribution<std::mt19937::result_type> distro2_;
};

