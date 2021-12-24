#pragma once
#include "IPlayer.h"
#include "TriasDataModel.h"

class BoardDisplayWidget;

class HumanPlayer : public IPlayer
{
	Q_OBJECT

public:
	HumanPlayer(std::shared_ptr<TriasDataModel> model, TriasDataModel::Piece pc, BoardDisplayWidget* view);
	virtual ~HumanPlayer();

	bool isHuman() const override { return true; }
	virtual void startTurn() override;
	virtual void turn() override;
	virtual void endTurn() override;

private:
	void moveRequested(int from, int to);

private:
	BoardDisplayWidget* view_;
	QMetaObject::Connection view_connection_;
};

