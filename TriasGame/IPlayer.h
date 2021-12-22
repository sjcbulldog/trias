#pragma once

#include "TriasDataModel.h"
#include <QtCore/QObject>
#include <memory>

class TriasDataModel;

class IPlayer : public QObject
{
	Q_OBJECT

public:
	IPlayer(std::shared_ptr<TriasDataModel> model, TriasDataModel::Piece pc) {
		model_ = model;
		pc_ = pc;
	}
	virtual bool isHuman() const = 0;
	virtual void yourTurn(bool b) = 0;
	virtual TriasDataModel::Piece piece() const {
		return pc_;
	}

signals:
	void move(int from, int to);

protected:
	std::shared_ptr<TriasDataModel> model() {
		return model_;
	}

	int firstPiece() const {
		return (pc_ == TriasDataModel::Piece::White) ? TriasDataModel::FirstWhitePieceNumber : TriasDataModel::FirstBlackPieceNumber;
	}

	int firstOffBoardPos() const {
		return (pc_ == TriasDataModel::Piece::White) ? TriasDataModel::WhiteOffMinPosition : TriasDataModel::BlackOffMinPosition;
	}

private:
	std::shared_ptr<TriasDataModel> model_;
	TriasDataModel::Piece pc_;
};
