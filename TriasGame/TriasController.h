#pragma once

#include "TriasDataModel.h"
#include <QtCore/QObject>
#include <QtCore/QTimer>
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
		PrepBlackPlay,
		BlackPlay,
		FinishBlackPlay,
		PrepWhitePlay,
		WhitePlay,
		FinishWhitePlay,
		Won,
		NewGame
	};

private:
	void moveRequested(int from, int to);
	void viewReady();
	void messageDisplayComplete(const QString& msg);
	void animationComplete(int from, int to);

	void playerTurnMessage();
	void nextTurn();

	void timerCallback();
	void displayMessage(const QString& msg);

	QString toString(GameState st);

private:
	std::shared_ptr<TriasDataModel> model_;
	BoardDisplayWidget* view_;
	GameState state_;
	bool msg_pending_;
	bool is_view_ready_;
	bool has_player_played_;

	IPlayer* white_player_;
	IPlayer* black_player_;
	IPlayer* current_player_;

	QTimer timer_;
};
