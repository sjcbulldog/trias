#include "BoardDisplayWidget.h"
#include "TriasDataModel.h"
#include <QtCore/QTimer>
#include <QtCore/QDebug>
#include <QtGui/QPainter>
#include <QtGui/QMouseEvent>

BoardDisplayWidget::BoardDisplayWidget(QWidget *parent) : QWidget(parent)
{
	setMinimumHeight(600);
	setMinimumWidth(600);

	model_ = nullptr;

	background_ = QColor(0xD8, 0xB3, 0x70);
	lines_ = QColor(78, 53, 36);
	landing_ = QColor(0xD8, 0xB3, 0x70);

	white_ = QColor(255, 255, 255);
	black_ = QColor(64, 64, 64);

	r45_ = -1;
	animating_ = -1;
	animate_to_ = -1;
	dragging_ = -1;
	display_message_ = false;
	display_time_ = 1000;

	(void)connect(&animation_timer_, &QTimer::timeout, this, &BoardDisplayWidget::animationStepDone);
}

BoardDisplayWidget::~BoardDisplayWidget()
{
}

void BoardDisplayWidget::showEvent(QShowEvent* ev)
{
	emit viewReady();
}

void BoardDisplayWidget::displayMessage(const QString& msg, int delay)
{
	if (delay == -1)
		delay = display_time_;

	QTimer::singleShot(delay, this, &BoardDisplayWidget::messageDone);
	message_text_ = msg;
	display_message_ = true;
	update();
}

void BoardDisplayWidget::messageDone()
{
	display_message_ = false;
	update();
	emit messageDisplayComplete(message_text_);
}

void BoardDisplayWidget::mouseMoveEvent(QMouseEvent* ev)
{
	if (dragging_ != -1)
	{
		drag_pos_ = ev->pos();
		update();
	}
}

bool BoardDisplayWidget::isPositionSelected(const QPoint &mouse, const QPoint& c)
{
	return (mouse - c).manhattanLength() < radiusOfPieces();
}

void BoardDisplayWidget::animateMove(int from, int to)
{
	static const int totalSteps = 50;

	animating_ = from;
	animate_to_ = to;
	drag_color_ = pieceToColor(model_->boardPiece(from));
	drag_pos_ = centerOfPos(from);

	QPoint topt = centerOfPos(to);

	curx_ = drag_pos_.x();
	cury_ = drag_pos_.y();
	dx_ = (topt.x() - drag_pos_.x()) / static_cast<double>(totalSteps);
	dy_ = (topt.y() - drag_pos_.y()) / static_cast<double>(totalSteps);
	animation_steps_ = totalSteps;

	animation_timer_.start(10);
}

void BoardDisplayWidget::animationStepDone()
{
	curx_ += dx_;
	cury_ += dy_;
	drag_pos_ = QPoint(static_cast<int>(curx_), static_cast<int>(cury_));
	animation_steps_--;

	if (animation_steps_ == 0)
	{
		int from = animating_;
		int to = animate_to_;

		animation_timer_.stop();
		animating_ = -1;
		animate_to_ = -1;
		emit animationComplete(from, to);
	}
	update();
}

void BoardDisplayWidget::mousePressEvent(QMouseEvent* ev)
{
	if (display_message_ || animating_ != -1)
		return;

	int downpos = -1;

	for (int pos = 0; pos <= model_->maxPosNumber() ; pos++)
	{
		if (isPositionSelected(ev->pos(), centerOfPos(pos)))
		{
			downpos = pos;
			break;
		}
	}

	if (downpos != -1)
	{
		if (model_->isOccupied(downpos))
		{
			dragging_ = downpos;
			drag_pos_ = centerOfPos(downpos);
			drag_color_ = pieceToColor(model_->boardPiece(downpos));
		}
	}
}

void BoardDisplayWidget::mouseReleaseEvent(QMouseEvent* ev)
{
	if (dragging_ != -1)
	{
		int uppos = -1;

		for (int pos = 0; pos <= model_->maxPosNumber(); pos++)
		{
			if (isPositionSelected(ev->pos(), centerOfPos(pos)))
			{
				uppos = pos;
				break;
			}
		}

		if (uppos != -1)
		{
			emit move(dragging_, uppos);
		}

		dragging_ = -1;
		update();
	}
}

void BoardDisplayWidget::setDataModel(std::shared_ptr<TriasDataModel> model) 
{
	if (model_ != nullptr)
		disconnect(data_model_connection_);

	model_ = model;

	if (model_ != nullptr)
	{
		auto fn = [this]() { this->update(); };
		data_model_connection_ = connect(model_.get(), &TriasDataModel::changed, fn);
	}
}

void BoardDisplayWidget::resizeEvent(QResizeEvent* ev)
{
	r45_ = -1;
}

QPoint BoardDisplayWidget::centerOfPos(int pos)
{
	assert(pos <= model_->maxPosNumber());

	QPoint ret;

	if (pos <= TriasDataModel::BoardMaxPosition)
	{
		ret = centerOfBoardPos(pos);
	}
	else if (pos >= TriasDataModel::BlackOffMinPosition && pos <= TriasDataModel::BlackOffMaxPosition)
	{
		ret = centerOfOff(pos - TriasDataModel::BlackOffMinPosition, true);
	}
	else if (pos >= TriasDataModel::WhiteOffMinPosition && pos <= TriasDataModel::WhiteOffMaxPosition)
	{
		ret = centerOfOff(pos - TriasDataModel::WhiteOffMinPosition, false);
	}

	return ret;
}

QPoint BoardDisplayWidget::centerOfBoardPos(int pos)
{
	assert(pos >= 0 && pos <= 8);

	QPoint ret;
	QPoint c = centerOfBoard();
	int r = radius();

	switch (pos)
	{
	case 0:
		ret = QPoint(c.x(), c.y() - r);
		break;

	case 1:
		ret = QPoint(c.x() + r45(), c.y() - r45());
		break;

	case 2:
		ret = QPoint(c.x() + r, c.y());
		break;

	case 3:
		ret = QPoint(c.x() + r45(), c.y() + r45());
		break;

	case 4:
		ret = QPoint(c.x(), c.y() + r);
		break;

	case 5:
		ret = QPoint(c.x() - r45(), c.y() + r45());
		break;

	case 6:
		ret = QPoint(c.x() - r, c.y());
		break;

	case 7:
		ret = QPoint(c.x() - r45(), c.y() - r45());
		break;

	case 8:
		ret = c;
		break;
	}

	return ret;
}

void BoardDisplayWidget::drawLine(QPainter& p, int from, int to)
{
	assert(from >= 0 && from <= 7 && to >= 0 && to <= 7);

	p.save();
	QPen pen(lines_);
	pen.setWidth(4);
	p.setPen(pen);
	p.drawLine(centerOfPos(from), centerOfPos(to));
	p.restore();
}

void BoardDisplayWidget::paintEvent(QPaintEvent* ev)
{
	QPainter p(this);

	drawBoard(p);

	if (model_ != nullptr)
		drawDataModel(p);

	if (dragging_ != -1 || animating_ != -1)
	{
		drawPiece(p, drag_pos_, drag_color_);
	}

	if (display_message_)
	{
		static const int TextMargin = 8;

		QFont font = p.font();
		font.setBold(true);
		font.setPointSizeF(16.0);

		QFontMetrics fm(font);

		int twidth = fm.horizontalAdvance(message_text_);
		QRect r(width() / 2 - twidth / 2 - TextMargin, height() / 2 - fm.height() / 2 - TextMargin, twidth + TextMargin * 2, fm.height() + TextMargin * 2);

		QBrush br(QColor(0, 0, 0));
		p.setBrush(br);
		p.setPen(Qt::PenStyle::NoPen);
		p.drawRect(r);

		QPen pen(QColor(0, 255, 0));
		p.setPen(pen);
		p.setFont(font);
		p.setBrush(Qt::BrushStyle::NoBrush);

		int x = r.center().x() - fm.horizontalAdvance(message_text_) / 2;
		int y = r.center().y() + fm.height() / 2 - fm.descent();
		p.drawText(QPoint(x,y), message_text_);
	}
}

void BoardDisplayWidget::drawDataModel(QPainter& p)
{
	for (int pos = 0; pos <= model_->maxPosNumber(); pos++)
	{
		if (pos != dragging_ && pos != animate_to_ && model_->isOccupied(pos))
		{
			TriasDataModel::Piece pc = model_->boardPiece(pos);
			drawPiece(p, pos, pieceToColor(pc));
		}
	}
}

void BoardDisplayWidget::drawPiece(QPainter& p, int pos, QColor c)
{
	drawPiece(p, centerOfPos(pos), c);
}

void BoardDisplayWidget::drawPiece(QPainter& p, const QPoint& center, QColor c)
{
	p.save();
	int r = radiusOfPieces();
	p.setBrush(QBrush(c));
	p.drawEllipse(center, r, r);
	p.restore();
}

QPoint BoardDisplayWidget::centerOfOff(int which, bool left)
{
	int x, y;

	int m = radiusOfPieces() / 4;

	y = height() - radiusOfPieces() - m;
	if (left)
		x = (m + radiusOfPieces() * 2) * which + radiusOfPieces() + m;
	else
		x = width() - (m + radiusOfPieces() * 2) * which - radiusOfPieces() - m;

	return QPoint(x, y);
}

void BoardDisplayWidget::drawBoard(QPainter &p)
{
	p.save();

	// Draw background
	p.setBrush(QBrush(background_));
	p.drawRect(0, 0, width(), height());

	// Draw outside circle
	QPen pen(lines_);
	pen.setWidth(4);
	p.setPen(pen);
	p.setBrush(Qt::BrushStyle::NoBrush);
	p.drawEllipse(centerOfBoard(), radius(), radius());

	//
	// Draw the lines
	//
	drawLine(p, 0, 4);
	drawLine(p, 1, 5);
	drawLine(p, 2, 6);
	drawLine(p, 3, 7);

	// Draw landing spots
	p.setBrush(QBrush(landing_));
	for (int i = 0; i < 9; i++)
	{
		QPoint pt = centerOfPos(i);
		p.drawEllipse(pt, radiusOfPieces(), radiusOfPieces());
	}

	p.restore();
}
