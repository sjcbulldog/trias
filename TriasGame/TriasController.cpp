#include "TriasController.h"
#include "TriasDataModel.h"
#include "BoardDisplayWidget.h"
#include "IPlayer.h"
#include <QtCore/QDebug>

TriasController::TriasController(QObject *parent) : QObject(parent)
{
	view_ = nullptr;
	state_ = GameState::Idle;
	white_player_ = nullptr;
	black_player_ = nullptr;
	current_player_ = nullptr;
	state_ = GameState::Idle;
	msg_pending_ = false;
	is_view_ready_ = false;
	has_player_played_ = false;

	(void)connect(&timer_, &QTimer::timeout, this, &TriasController::timerCallback);
	timer_.start(200);
}

TriasController::~TriasController()
{
}

void TriasController::setPlayers(IPlayer* black, IPlayer* white)
{
	black_player_ = black;
	(void)connect(black, &IPlayer::move, this, &TriasController::moveRequested);

	white_player_ = white;
	(void)connect(white, &IPlayer::move, this, &TriasController::moveRequested);
}

void TriasController::setDataModel(std::shared_ptr<TriasDataModel> model)
{
	model_ = model;
}

void TriasController::setView(BoardDisplayWidget* view)
{
	view_ = view;
	(void)connect(view_, &BoardDisplayWidget::viewReady, this, &TriasController::viewReady);
	(void)connect(view_, &BoardDisplayWidget::messageDisplayComplete, this, &TriasController::messageDisplayComplete);
	(void)connect(view_, &BoardDisplayWidget::animationComplete, this, &TriasController::animationComplete);
}

void TriasController::viewReady()
{
	is_view_ready_ = true;
}

void TriasController::animationComplete(int from, int to)
{
	qDebug() << "animationComplete: from=" << from << ", to=" << to;
	has_player_played_ = true;
}

void TriasController::playerTurnMessage()
{
	if (current_player_->isHuman())
	{
		QString msg = "It is " + ::toString(current_player_->piece()).toLower() + "'s turn";
		displayMessage(msg);
	}
	else
	{
		msg_pending_ = false;
	}
}

void TriasController::displayMessage(const QString& msg)
{
	msg_pending_ = true;
	view_->displayMessage(msg);
}

void TriasController::timerCallback()
{
	GameState st = state_;

	switch (state_)
	{
	case GameState::Idle:
		if (is_view_ready_)
		{
			state_ = GameState::PrepBlackPlay;
		}
		break;

	case GameState::PrepBlackPlay:
		current_player_ = black_player_;
		current_player_->startTurn();
		state_ = GameState::BlackPlay;
		has_player_played_ = false;
		playerTurnMessage();
		break;

	case GameState::BlackPlay:
		if (!msg_pending_)
		{
			current_player_->turn();
			state_ = GameState::FinishBlackPlay;
		}
		break;

	case GameState::FinishBlackPlay:
		if (has_player_played_)
		{
			current_player_->endTurn();
			if (model_->win(current_player_->piece()))
			{
				model_->blackWon();
				state_ = GameState::Won;
				displayMessage("Black won");
			}
			else
			{
				current_player_ = nullptr;
				state_ = GameState::PrepWhitePlay;
			}
		}	
		break;

	case GameState::PrepWhitePlay:
		current_player_ = white_player_;
		current_player_->startTurn();
		state_ = GameState::WhitePlay;
		has_player_played_ = false;
		playerTurnMessage();
		break;

	case GameState::WhitePlay:
		if (!msg_pending_)
		{
			current_player_->turn();
			state_ = GameState::FinishWhitePlay;
		}
		break;

	case GameState::FinishWhitePlay:
		if (has_player_played_)
		{
			current_player_->endTurn();
			if (model_->win(current_player_->piece()))
			{
				model_->whiteWon();
				state_ = GameState::Won;
				displayMessage("White won");
			}
			else
			{
				current_player_ = nullptr;
				state_ = GameState::PrepBlackPlay;
			}
		}
		break;

	case GameState::Won:
		if (!msg_pending_)
		{
			model_->initBoard();
			state_ = GameState::NewGame;
			displayMessage("New Game");
		}
		break;

	case GameState::NewGame:
		if (!msg_pending_)
		{
			state_ = GameState::PrepBlackPlay;
		}
		break;
	}

	if (st != state_)
	{
		qDebug() << "State: " << toString(st) << " -> " << toString(state_);
	}
}

QString TriasController::toString(GameState st)
{
	QString ret = "UNKNOWN";

	switch (st)
	{
	case GameState::BlackPlay:
		ret = "BlackPlay";
		break;
	case GameState::FinishBlackPlay:
		ret = "FinishBlackPlay";
		break;
	case GameState::FinishWhitePlay:
		ret = "FinishWhitePlay";
		break;
	case GameState::Idle:
		ret = "Idle";
		break;
	case GameState::NewGame:
		ret = "NewGame";
		break;
	case GameState::PrepBlackPlay:
		ret = "PrepBlackPlay";
		break;
	case GameState::PrepWhitePlay:
		ret = "PrepWhitePlay";
		break;
	case GameState::WhitePlay:
		ret = "WhitePlay";
		break;
	case GameState::Won:
		ret = "Won";
		break;
	}

	return ret;
}

void TriasController::messageDisplayComplete(const QString &msg)
{
	msg_pending_ = false;
	qDebug() << "messageDisplayComplete: msg='" << msg << "'";
}


void TriasController::moveRequested(int from, int to)
{
	qDebug() << "moveRequested: from=" << from << ", to=" << to << ", piece=" << model_->board(from);

	assert(model_->boardPiece(from) == current_player_->piece());

	//
	// Check to see if the move is valid
	//
	TriasDataModel::MoveError err = model_->move(from, to);

	switch (err)
	{
	case TriasDataModel::MoveError::Good:
		// Yes the move is valid
		if (!current_player_->isHuman())
		{
			view_->animateMove(from, to);
			model_->move(from, to, true);
		}
		else
		{
			has_player_played_ = true;
			model_->move(from, to, true);
		}
		break;

	case TriasDataModel::MoveError::InvalidDestination:
		view_->displayMessage("Cannot move a piece off the board");
		break;

	case TriasDataModel::MoveError::LocationOccupied:
		qDebug() << "Location Occupied";
		view_->displayMessage("There is already a piece at that location");
		break;

	case TriasDataModel::MoveError::NotAdjacent:
		view_->displayMessage("That location is not adcacent to the current location");
		break;

	case TriasDataModel::MoveError::BackAgain:
		view_->displayMessage("You cannot move a piece back to its previous location");
		break;
	}
}
