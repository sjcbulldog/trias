#pragma once

#include <QtWidgets/QMainWindow>
#include <QtCore/QSettings>

class BoardDisplayWidget;
class TriasDataModel;
class TriasController;

class TriasGame : public QMainWindow
{
    Q_OBJECT

public:
    TriasGame(QWidget *parent = Q_NULLPTR);

protected:
    void closeEvent(QCloseEvent* ev) override;

private:
    static constexpr const char* GeometrySettings = "geometry";
    static constexpr const char* WindowStateSettings = "windowstate";

private:
    void createWindows();

private:
    BoardDisplayWidget* view_;
    std::shared_ptr<TriasDataModel> model_;
    std::shared_ptr<TriasController> controller_;
    QSettings settings_;
};
