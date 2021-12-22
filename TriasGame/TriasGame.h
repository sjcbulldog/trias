#pragma once

#include <QtWidgets/QMainWindow>

class BoardDisplayWidget;
class TriasDataModel;
class TriasController;

class TriasGame : public QMainWindow
{
    Q_OBJECT

public:
    TriasGame(QWidget *parent = Q_NULLPTR);

private:
    BoardDisplayWidget* view_;
    std::shared_ptr<TriasDataModel> model_;
    std::shared_ptr<TriasController> controller_;
};
