#include "TriasController.h"
#include "TriasDataModel.h"
#include "BoardDisplayWidget.h"
#include "IPlayer.h"

TriasController::TriasController(QObject *parent) : QObject(parent)
{
	view_ = nullptr;
	state_ = GameState::Idle;
	white_player_ = nullptr;
	black_player_ = nullptr;
	current_player_ = nullptr;
}

TriasController::~TriasController()
{
}

void TriasController::setPlayers(IPlayer* black, IPlayer* white)
{
	black->yourTurn(false);
	black_player_ = black;
	(void)connect(black, &IPlayer::move, this, &TriasController::moveRequested);

	white->yourTurn(false);
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
}

void TriasController::messageDisplayComplete()
{
	switch (state_)
	{
	case GameState::Won:
		model_->initBoard();
		black_player_->yourTurn(true);
		current_player_ = black_player_;
		white_player_->yourTurn(false);
		view_->displayMessage("New Game");
		state_ = GameState::NewGame;
		break;

	case GameState::NewGame:
		playerTurnMessage();
		state_ = GameState::Playing;
		break;
	}
}

void TriasController::moveRequested(int from, int to)
{
	assert(model_->boardPiece(from) == current_player_->piece());

	TriasDataModel::MoveError err = model_->move(from, to);
	switch (err)
	{
	case TriasDataModel::MoveError::Good:
		if (!current_player_->isHuman())
			view_->animateMove(from, to);
		model_->move(from, to, true);
		nextTurn();
		break;

	case TriasDataModel::MoveError::InvalidDestination:
		view_->displayMessage("Cannot move a piece off the board");
		break;

	case TriasDataModel::MoveError::LocationOccupied:
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


void TriasController::nextTurn()
{
	if (model_->win(current_player_->piece()))
	{
		QString msg = toString(current_player_->piece()) + " wins";
		view_->displayMessage(msg, 3000);
		state_ = GameState::Won;
	}
	else
	{
		current_player_->yourTurn(false);

		if (current_player_ == white_player_)
			current_player_ = black_player_;
		else
			current_player_ = white_player_;

		current_player_->yourTurn(true);
		playerTurnMessage();
	}
}

void TriasController::viewReady()
{
	state_ = GameState::Playing;
	current_player_ = black_player_;
	black_player_->yourTurn(true);

	playerTurnMessage();
}

void TriasController::playerTurnMessage()
{
	if (current_player_->isHuman())
	{
		QString msg = "It is " + toString(current_player_->piece()).toLower() + "'s turn";
		view_->displayMessage(msg);
	}
}
