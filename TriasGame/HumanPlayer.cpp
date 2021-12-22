#include "HumanPlayer.h"
#include "BoardDisplayWidget.h"
#include "TriasDataModel.h"

HumanPlayer::HumanPlayer(std::shared_ptr<TriasDataModel> model, TriasDataModel::Piece pc, BoardDisplayWidget* view) : IPlayer(model, pc)
{
	view_ = view;
	my_turn_ = false;
}

HumanPlayer::~HumanPlayer()
{
}

void HumanPlayer::yourTurn(bool b)
{
	if (b)
	{
		view_connection_ = connect(view_, &BoardDisplayWidget::move, this, &HumanPlayer::moveRequested);
	}
	else
	{
		disconnect(view_connection_);
	}

	my_turn_ = b;
}

void HumanPlayer::moveRequested(int from, int to)
{
	if (my_turn_ && model()->boardPiece(from) == piece())
	{
		emit move(from, to);
	}
	else
	{
		view_->displayMessage("It is not your turn");
	}
}