#include "HumanPlayer.h"
#include "BoardDisplayWidget.h"
#include "TriasDataModel.h"
#include <QtCore/QDebug>

HumanPlayer::HumanPlayer(std::shared_ptr<TriasDataModel> model, TriasDataModel::Piece pc, BoardDisplayWidget* view) : IPlayer(model, pc)
{
	view_ = view;
}

HumanPlayer::~HumanPlayer()
{
}

void HumanPlayer::yourTurn()
{
	view_connection_ = connect(view_, &BoardDisplayWidget::move, this, &HumanPlayer::moveRequested);
}

void HumanPlayer::moveRequested(int from, int to)
{
	qDebug() << "HumanPlayer::moveRequested: from=" << from << ", to=" << to;
	emit move(from, to);
	disconnect(view_connection_);
}
