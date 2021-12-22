#pragma once

#include "TriasDataModel.h"
#include <QtCore/QTimer>
#include <QtWidgets/QWidget>
#include <cmath>
#include <memory>


class BoardDisplayWidget : public QWidget
{
	Q_OBJECT

public:
	BoardDisplayWidget(QWidget *parent = Q_NULLPTR);
	~BoardDisplayWidget();

	void setDataModel(std::shared_ptr<TriasDataModel> model);
	void displayMessage(const QString& msg, int delay = -1);
	void animateMove(int from, int to);

signals:
	void move(int from, int to);
	void viewReady();
	void messageDisplayComplete();
	void animationComplete();

protected:
	void paintEvent(QPaintEvent* ev) override;
	void resizeEvent(QResizeEvent* ev) override;
	void mouseMoveEvent(QMouseEvent* ev) override;
	void mousePressEvent(QMouseEvent* ev) override;
	void mouseReleaseEvent(QMouseEvent* ev) override;
	void showEvent(QShowEvent* ev) override;

private:
	static constexpr const int MaximumPieceNumber = 14;

private:
	//
	// Positions on the board are numbered from 0 to 14.  Positions 0 through 8 are those on the
	// main board.  Positions 9 through 11 are those off the board on the bottom left and positions
	// 12 throught 14 are those off the board on the bottom right.
	//

	// Returns the radius of the main board
	int radius() { 
		//
		// 80 % of mininum dimension (width or height) is the diameter, then divide by 2
		// to get the radius
		//
		int mindim = std::min(width(), height());
		int diam = mindim * 8 / 10;
		return diam / 2;
	}

	// Returns the radius of the pieces on the board
	int radiusOfPieces() {
		return std::min(width(), height()) / 20;
	}

	// Returns the point that is the center of the main board
	QPoint centerOfBoard() {
		return QPoint(width() / 2, height() / 2);
	}

	// Returns the point that is the center of the position given by pos
	QPoint centerOfPos(int pos);


	// Return the distance in the X and Y direction to the board places that are at a fourty five
	// degree angle on the board to the center.  This value is cached so we don't do the square root
	// math on every redraw.  A resize sets the r54_ to -1 causing the next redraw to re compute this
	// value.
	int r45() {
		if (r45_ == -1) {
			double r = static_cast<double>(radius());
			r45_ = static_cast<int>(std::sqrt(r * r / 2.0));
		}

		return r45_;
	}

	// Returns true if the mouse is in a given position
	bool isPositionSelected(const QPoint &mouse, const QPoint& piece);

	// Draw a line between two places both of which are on the outside of the board circle
	void drawLine(QPainter &p, int from, int to);

	// Draw the complete board (with no pieces)
	void drawBoard(QPainter& p);

	// Draw the pieces on the board given by the data mocel
	void drawDataModel(QPainter& p);

	// Draw a pice on the board given its position
	void drawPiece(QPainter &p, int pos, QColor c);

	// Draw a piece at the coordinates given by center
	void drawPiece(QPainter& p, const QPoint& center, QColor c);

	// Return the point that is the center of the off board position for player markers
	// The which should be between 0 and 2 and the left is true for the left side of the board
	// and false for the right side of the board.
	QPoint centerOfOff(int which, bool left);

	// Compute the center of a position on the board
	QPoint centerOfBoardPos(int pos);

	// Convert a piece type to a color
	QColor pieceToColor(TriasDataModel::Piece pc) const {
		assert(pc == TriasDataModel::Piece::Black || pc == TriasDataModel::Piece::White);
		return (pc == TriasDataModel::Piece::Black) ? black_ : white_;
	}

	void messageDone();
	void animationStepDone();

private:
	std::shared_ptr<TriasDataModel> model_;
	QMetaObject::Connection data_model_connection_;


	//
	// Drawing support
	//
	QColor background_;
	QColor lines_;
	QColor landing_;
	QColor white_;
	QColor black_;
	int r45_;

	//
	// Drag and drop support
	//
	int dragging_;
	QColor drag_color_;
	QPoint drag_pos_;

	//
	// Animation support
	//
	int animating_;
	QTimer animation_timer_;
	int animate_to_;
	double dx_, dy_;
	double curx_, cury_;
	int animation_steps_;

	//
	// Message display support
	//
	bool display_message_;
	QString message_text_;
	int display_time_;
};
