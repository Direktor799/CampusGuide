#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QComboBox>
#include <QTimer>
#include <QDebug>
#include "./ui_mainwindow.h"
#include "map.h"
#include "player.h"
#include "functions.h"
#include "route_label.h"
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void route_calcu();
    void map_switch();
    void move_switch(route_info* route);
    void move_cancel();
    void timer_update();
private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
