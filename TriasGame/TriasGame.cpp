#include "TriasGame.h"
#include "BoardDisplayWidget.h"
#include "TriasDataModel.h"
#include "TriasController.h"
#include "HumanPlayer.h"
#include "ComputerPlayer.h"

TriasGame::TriasGame(QWidget *parent) : QMainWindow(parent)
{
	IPlayer *w, *b;

	model_ = std::make_shared<TriasDataModel>(nullptr);

	createWindows();

	controller_ = std::make_shared<TriasController>(nullptr);
	controller_->setDataModel(model_) ;
	controller_->setView(view_);

	b = new HumanPlayer(model_, TriasDataModel::Piece::Black, view_);
	w = new ComputerPlayer(model_, TriasDataModel::Piece::White);
	// w = new HumanPlayer(model_, TriasDataModel::Piece::White, view_);
	controller_->setPlayers(b, w);
}

void TriasGame::createWindows()
{
	view_ = new BoardDisplayWidget();
	setCentralWidget(view_);
	view_->setDataModel(model_);

	if (settings_.contains(GeometrySettings))
		restoreGeometry(settings_.value(GeometrySettings).toByteArray());

	if (settings_.contains(WindowStateSettings))
		restoreState(settings_.value(WindowStateSettings).toByteArray());
}

void TriasGame::closeEvent(QCloseEvent* ev)
{
	settings_.setValue(GeometrySettings, saveGeometry());
	settings_.setValue(WindowStateSettings, saveState());
	QMainWindow::closeEvent(ev);
}