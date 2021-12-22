#pragma once

#include "TriasDataModel.h"
#include <QObject>
#include <memory>
#include <array>
#include <vector>

class BoardDisplayWidget;
class IPlayer;

class TriasController : public QObject
{
	Q_OBJECT

public:
	TriasController(QObject *parent);
	~TriasController();

	void setDataModel(std::shared_ptr<TriasDataModel> model);
	void setView(BoardDisplayWidget *view);

	void setPlayers(IPlayer* black, IPlayer* white);

private:
	enum class GameState
	{
		Idle,
		Playing,
		Won,
		NewGame,
	};

private:
	void moveRequested(int from, int to);
	void viewReady();
	void messageDisplayComplete();

	void playerTurnMessage();
	void nextTurn();

private:
	std::shared_ptr<TriasDataModel> model_;
	BoardDisplayWidget* view_;
	GameState state_;

	IPlayer* white_player_;
	IPlayer* black_player_;
	IPlayer* current_player_;

};
