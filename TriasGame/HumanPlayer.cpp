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

void HumanPlayer::startTurn()
{
	qDebug() << "HumanPlayer::startTurn()";
	view_connection_ = connect(view_, &BoardDisplayWidget::move, this, &HumanPlayer::moveRequested);
}

void HumanPlayer::turn()
{
}

void HumanPlayer::moveRequested(int from, int to)
{
	qDebug() << "HumanPlayer::moveRequested " << from << " " << to;
	emit move(from, to);
}

void HumanPlayer::endTurn()
{
	qDebug() << "HumanPlayer::startTurn()";
	disconnect(view_connection_);
}
