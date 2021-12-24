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
	void yourTurn() override;

private:
	void moveOntoBoard();
	void moveWithinBoard();

	int findRandomLocation();

private:
	std::default_random_engine engine_;
	std::uniform_int_distribution<std::mt19937::result_type> distro8_;
	std::uniform_int_distribution<std::mt19937::result_type> distro2_;
};

